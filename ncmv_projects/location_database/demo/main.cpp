/**
 * @file main.cpp
 * @brief Demo / integration test for the Location Manager Database.
 *
 * Simulates the following production scenario:
 *
 *   1. A "GNSS Parser" thread periodically writes PVT + Satellite + IMU data
 *      to the database and calls commit_epoch().
 *
 *   2. Three "Location Feature" clients subscribe with different masks:
 *        - navigation_feature  : wants PVT (full position solution)
 *        - dr_feature          : wants PVT + IMU (dead-reckoning inputs)
 *        - telemetry_feature   : wants PVT + Satellites + IMU (full picture)
 *
 *   3. Each feature demonstrates both push (subscription callback) and
 *      pull (explicit read) patterns.
 *
 *   4. The demo also exercises:
 *        - Lifecycle state machine (uninitialized -> running -> stopped -> reset)
 *        - Fine-grained write_field / read_field API
 *        - Attempted write while stopped (should throw)
 *        - Unsubscribe mid-run
 */

#include "locmgr/db/LocationDatabase.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

using namespace locmgr::db;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::string current_timestamp()
{
    using namespace std::chrono;
    auto now = system_clock::now().time_since_epoch();
    auto ms  = duration_cast<milliseconds>(now).count();
    std::ostringstream ss;
    ss << "[" << ms << "] ";
    return ss.str();
}

#define LOG(tag, msg) \
    std::cout << current_timestamp() << "[" << (tag) << "] " << (msg) << "\n"

// ---------------------------------------------------------------------------
// Feature A - navigation_feature
// Wants: full PVT (position + velocity + time)
// ---------------------------------------------------------------------------
class navigation_feature : public i_field_update_listener
{
public:
    explicit navigation_feature(i_location_database_reader& db) : m_db(db) {}

    void on_fields_updated(const field_mask& /*updated*/) override
    {
        ++m_callback_count;
        const pvt_data pvt = m_db.read_pvt();  // pull snapshot on callback
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(6)
           << "PVT callback #" << m_callback_count
           << "  lat="  << pvt.latitude
           << "  lon="  << pvt.longitude
           << "  alt="  << pvt.altitude_msl << "m"
           << "  spd="  << pvt.speed        << "m/s"
           << "  fix="  << static_cast<int>(pvt.fix_type);
        LOG("NavFeature", ss.str());
    }

    int callback_count() const { return m_callback_count.load(); }

private:
    i_location_database_reader& m_db;
    std::atomic<int>            m_callback_count { 0 };
};

// ---------------------------------------------------------------------------
// Feature B - dr_feature
// Wants: PVT + IMU
// ---------------------------------------------------------------------------
class dr_feature : public i_field_update_listener
{
public:
    explicit dr_feature(i_location_database_reader& db) : m_db(db) {}

    void on_fields_updated(const field_mask& /*updated*/) override
    {
        ++m_callback_count;
        const imu_data imu = m_db.read_imu_data();
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(4)
           << "DR callback #" << m_callback_count
           << "  accel=(" << imu.accel_x << "," << imu.accel_y << "," << imu.accel_z << ")"
           << "  gyro=("  << imu.gyro_x  << "," << imu.gyro_y  << "," << imu.gyro_z  << ")"
           << "  temp="   << imu.temperature << "degC";
        LOG("DrFeature", ss.str());
    }

    int callback_count() const { return m_callback_count.load(); }

private:
    i_location_database_reader& m_db;
    std::atomic<int>            m_callback_count { 0 };
};

// ---------------------------------------------------------------------------
// Feature C - telemetry_feature
// Wants: PVT + Satellites + IMU  (needs ALL three groups per epoch)
// ---------------------------------------------------------------------------
class telemetry_feature : public i_field_update_listener
{
public:
    explicit telemetry_feature(i_location_database_reader& db) : m_db(db) {}

    void on_fields_updated(const field_mask& /*updated*/) override
    {
        ++m_callback_count;
        const satellite_data sat = m_db.read_satellite_data();
        std::ostringstream ss;
        ss << "Telemetry callback #" << m_callback_count
           << "  sats_in_view=" << static_cast<int>(sat.total_count)
           << "  used_in_fix="  << static_cast<int>(sat.used_in_fix)
           << "  avg_cno="      << sat.avg_cno << " dBHz"
           << "  max_cno="      << sat.max_cno << " dBHz";
        LOG("TelemetryFeature", ss.str());
    }

    int callback_count() const { return m_callback_count.load(); }

private:
    i_location_database_reader& m_db;
    std::atomic<int>            m_callback_count { 0 };
};

// ---------------------------------------------------------------------------
// Simulated GNSS Parser (runs in its own thread)
// ---------------------------------------------------------------------------
class simulated_gnss_parser
{
public:
    simulated_gnss_parser(i_location_database_writer& writer, int epoch_count)
        : m_writer(writer), m_epoch_count(epoch_count)
    {}

    void run()
    {
        for (int i = 0; i < m_epoch_count; ++i)
        {
            // --- Build synthetic pvt_data ---
            pvt_data pvt;
            pvt.latitude         = 52.520008 + i * 0.0001;   // Berlin-ish, drifting north
            pvt.longitude        = 13.404954 + i * 0.00005;
            pvt.altitude_msl     = 34.0 + i * 0.5;
            pvt.altitude_hae     = pvt.altitude_msl + 48.0;
            pvt.heading_motion   = (i * 10) % 360;
            pvt.heading_vehicle  = pvt.heading_motion;
            pvt.speed            = 13.8 + (i % 5) * 0.5;    // ~50 km/h
            pvt.horizontal_acc   = 1.2;
            pvt.vertical_acc     = 2.1;
            pvt.speed_acc        = 0.3;
            pvt.heading_acc      = 1.0;
            pvt.timestamp_utc_ms = 1700000000000LL + i * 1000;
            pvt.fix_type         = 3;      // 3D fix
            pvt.fix_flags        = 0x01;   // gnss_fix_ok
            pvt.num_sv_used      = 12;
            pvt.pdop             = 1.8f;
            pvt.hdop             = 1.2f;
            pvt.vdop             = 1.4f;

            // --- Build synthetic satellite_data ---
            satellite_data sat;
            sat.total_count        = 28;
            sat.used_in_fix        = 12;
            sat.avg_cno            = 36.5f + (i % 3) * 0.5f;
            sat.max_cno            = 45.0f;
            sat.constellation_mask = 0x0F; // GPS+GLONASS+Galileo+BeiDou

            // --- Build synthetic imu_data ---
            imu_data imu;
            imu.accel_x      = 0.01 * i;
            imu.accel_y      = -0.02 * i;
            imu.accel_z      = 9.81;
            imu.gyro_x       = 0.001 * i;
            imu.gyro_y       = 0.002;
            imu.gyro_z       = -0.001;
            imu.temperature  = 25.0f + (i % 10) * 0.1f;
            imu.calib_status = 0x03; // fully calibrated

            // --- Write to database (order is not significant) ---
            m_writer.write_pvt(pvt);
            m_writer.write_satellite_data(sat);
            m_writer.write_imu_data(imu);

            // --- Commit this epoch: triggers subscriber notifications ---
            m_writer.commit_epoch();

            LOG("GnssParser", "epoch " + std::to_string(i + 1) + " committed");

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

private:
    i_location_database_writer& m_writer;
    int                         m_epoch_count;
};

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    std::cout << "=== Location Manager Database Demo ===\n\n";

    // -----------------------------------------------------------------------
    // 1. Create database
    // -----------------------------------------------------------------------
    location_database db;
    LOG("Main", "state = " + db.get_state_name()); // uninitialized

    // -----------------------------------------------------------------------
    // 2. Attempt write before start (should throw)
    // -----------------------------------------------------------------------
    try {
        pvt_data dummy;
        db.write_pvt(dummy);
        LOG("Main", "ERROR: expected exception was NOT thrown");
    } catch (const std::logic_error& ex) {
        LOG("Main", std::string("Expected exception caught: ") + ex.what());
    }

    // -----------------------------------------------------------------------
    // 3. Start database
    // -----------------------------------------------------------------------
    bool started = db.start();
    LOG("Main", std::string("db.start() = ") + (started ? "true" : "false") +
        "  state = " + db.get_state_name());

    // -----------------------------------------------------------------------
    // 4. Create feature instances (they hold a reference to reader interface)
    // -----------------------------------------------------------------------
    i_location_database_reader& reader = db;

    navigation_feature nav_feature(reader);
    dr_feature         dr_feat    (reader);
    telemetry_feature  tel_feature(reader);

    // -----------------------------------------------------------------------
    // 5. Subscribe with different field_masks
    // -----------------------------------------------------------------------
    const field_mask pvt_mask = make_field_mask({
        field_id::PVT_LATITUDE, field_id::PVT_LONGITUDE,
        field_id::PVT_ALTITUDE_MSL, field_id::PVT_SPEED,
        field_id::PVT_FIX_TYPE, field_id::PVT_TIMESTAMP_UTC
    });

    const field_mask dr_mask = make_field_mask({
        field_id::PVT_SPEED, field_id::PVT_HEADING_MOTION,
        field_id::IMU_ACCEL_X, field_id::IMU_ACCEL_Y, field_id::IMU_ACCEL_Z,
        field_id::IMU_GYRO_X,  field_id::IMU_GYRO_Y,  field_id::IMU_GYRO_Z
    });

    const field_mask tel_mask = make_field_mask({
        field_id::PVT_LATITUDE, field_id::PVT_LONGITUDE,
        field_id::SAT_COUNT, field_id::SAT_USED_IN_FIX, field_id::SAT_AVG_CN0,
        field_id::IMU_ACCEL_X, field_id::IMU_TEMPERATURE
    });

    const uint64_t nav_token = db.subscribe(&nav_feature, pvt_mask);
    const uint64_t dr_token  = db.subscribe(&dr_feat,     dr_mask);
    const uint64_t tel_token = db.subscribe(&tel_feature, tel_mask);

    LOG("Main", "Subscriptions registered: nav=" + std::to_string(nav_token)
              + "  dr="  + std::to_string(dr_token)
              + "  tel=" + std::to_string(tel_token));

    // -----------------------------------------------------------------------
    // 6. Run the simulated GNSS parser in a background thread (5 epochs)
    // -----------------------------------------------------------------------
    simulated_gnss_parser parser(db, 5);
    std::thread parser_thread([&parser]{ parser.run(); });

    parser_thread.join();
    LOG("Main", "Parser finished");

    // -----------------------------------------------------------------------
    // 7. Demonstrate pull reads after parser is done
    // -----------------------------------------------------------------------
    {
        const pvt_data pvt = db.read_pvt();
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(6)
           << "Final PVT: lat=" << pvt.latitude
           << "  lon="   << pvt.longitude
           << "  speed=" << pvt.speed << " m/s"
           << "  fix="   << static_cast<int>(pvt.fix_type);
        LOG("Main", ss.str());
    }

    // Fine-grained field read example
    {
        double sat_count = 0.0;
        if (db.read_field(field_id::SAT_COUNT, sat_count))
            LOG("Main", "SAT_COUNT (fine-grained) = " +
                std::to_string(static_cast<int>(sat_count)));
    }

    // -----------------------------------------------------------------------
    // 8. Unsubscribe the DR feature mid-scenario
    // -----------------------------------------------------------------------
    db.unsubscribe(dr_token);
    LOG("Main", "Unsubscribed dr_feature (token=" + std::to_string(dr_token) + ")");

    // -----------------------------------------------------------------------
    // 9. Print callback statistics
    // -----------------------------------------------------------------------
    LOG("Main", "nav_feature  callbacks: " + std::to_string(nav_feature.callback_count()));
    LOG("Main", "dr_feature   callbacks: " + std::to_string(dr_feat.callback_count()));
    LOG("Main", "tel_feature  callbacks: " + std::to_string(tel_feature.callback_count()));

    // -----------------------------------------------------------------------
    // 10. Lifecycle: stop -> attempt write -> reset -> start again
    // -----------------------------------------------------------------------
    db.stop();
    LOG("Main", "After stop: state = " + db.get_state_name()); // stopped

    try {
        pvt_data dummy;
        db.write_pvt(dummy);
        LOG("Main", "ERROR: write after stop should have thrown");
    } catch (const std::logic_error& ex) {
        LOG("Main", std::string("Expected exception after stop: ") + ex.what());
    }

    db.reset();
    LOG("Main", "After reset: state = " + db.get_state_name()); // uninitialized

    db.start();
    LOG("Main", "After re-start: state = " + db.get_state_name()); // running

    LOG("Main", "--- Demo complete ---");
    return 0;
}
