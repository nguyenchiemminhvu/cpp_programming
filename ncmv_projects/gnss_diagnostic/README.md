- [gnss\_diagnostic](#gnss_diagnostic)
  - [1. Problem Analysis](#1-problem-analysis)
    - [Key challenges](#key-challenges)
    - [Observations](#observations)
  - [2. Architecture Overview](#2-architecture-overview)
    - [Threading model](#threading-model)
  - [3. Design Patterns Used](#3-design-patterns-used)
  - [4. Folder Structure](#4-folder-structure)
  - [5. Diagrams](#5-diagrams)
  - [6. Usage Guide](#6-usage-guide)
    - [6.1 Integrating with Location Service](#61-integrating-with-location-service)
    - [6.2 Adding a new DTC condition](#62-adding-a-new-dtc-condition)
    - [6.3 How recovery works](#63-how-recovery-works)
    - [6.4 Adding a new recovery step](#64-adding-a-new-recovery-step)
  - [7. Design Principles in This Context](#7-design-principles-in-this-context)

# gnss_diagnostic

DTC monitoring & recovery module for the **GNSS subsystem** of an automotive Location Service.

## 1. Problem Analysis

### Key challenges

1. **Multiple, heterogeneous DTC conditions** with independent debounce
   timings (PVT timeout, antenna short, antenna open, more to come).
   Putting timing logic next to each condition leads to duplicated,
   subtly-different debouncers across the codebase.
2. **Escalation policy** crosses module boundaries — a chip reset is
   handled by the GNSS HW Adapter, but a platform reboot is handled by
   the platform Health Service. The escalation must coordinate both
   without leaking either dependency into individual monitors.
3. **One-shot terminal state** — once a platform reboot has been
   requested, *no further* recovery may run, even if more DTCs fire.
   This is a system-wide invariant; it cannot be enforced if recovery
   logic is scattered.
4. **Power-mode awareness** — in LPM the chip is off, so events stop
   arriving. A naive monitor would mis-trigger a "no PVT" DTC every time
   the vehicle parks.
5. **Testability** — debounce intervals are seconds long; running real
   wall-clock time in tests is unacceptable. Every time-dependent path
   must be drivable from a fake clock.
6. **Chip-agnostic** — a vendor swap (u-blox → Quectel → MediaTek) must
   not touch the diagnostic code.

### Observations

- DTCs always have the same lifecycle: *raw signal* → *debounce* →
  *DTC set/clear*. This pattern is worth factoring.
- Escalation is a tiny state machine, not a general workflow engine.
- Power-mode handling is orthogonal to monitoring; it should live in a
  separate adapter so it is easy to mock or reuse.

---

## 2. Architecture Overview

The module is structured around three concerns, each with its own
collaborator graph:

```
┌─────────────────────────────────────────────────────────────────────┐
│                  gnss_diagnostic_manager  (Facade)                  │
│                                                                     │
│   ┌──────────────────────┐      ┌──────────────────────────────┐    │
│   │  i_dtc_monitor       │      │      recovery_manager        │    │
│   │  (Strategy +         │ ───▶ │    (escalation State)        │    │
│   │   Template Method)   │      │                              │    │
│   └──────────────────────┘      └──────────────┬───────────────┘    │
│                                                │                    │
│                                                ▼                    │
│                                  i_recovery_action (Strategy)       │
│                                  - chip_reset_action                │
│                                  - platform_reboot_action           │
└─────────────────────────────────────────────────────────────────────┘
              ▲ subscribes                    ▲ pauses / resumes
              │                               │
   i_gnss_event_source (Observer)    power_mode_controller
   (provided by GNSS HW Adapter)
```

Boundaries with the rest of the platform are *only* abstractions:
`i_dtc_service`, `i_health_service`, `i_chip_controller`,
`i_gnss_event_source`, `i_clock`. Concrete adapters live elsewhere; this
keeps the library chip-agnostic, IPC-agnostic and unit-testable.

### Threading model

The library is **single-threaded by contract**. The host application is
expected to:

1. Deliver `on_pvt` / `on_antenna` callbacks on one thread (typically the
   Location Service thread).
2. Drive `gnss_diagnostic_manager::on_tick(now)` from the same thread on
   a fixed cadence (e.g. 100 ms).

This avoids locks inside debounce timers and recovery state machines,
which would otherwise be the easiest place to introduce race conditions
in a real automotive ECU.

---

## 3. Design Patterns Used

| Pattern               | Where                                            | Why                                                                                                                |
|-----------------------|--------------------------------------------------|--------------------------------------------------------------------------------------------------------------------|
| **Facade**            | `gnss_diagnostic_manager`                        | Single integration point for the Location Service. Hides monitor wiring, observer plumbing, recovery escalation.   |
| **Observer**          | `i_gnss_event_source` / `i_gnss_event_listener`  | Allows multiple monitors to react to the same PVT / antenna stream without the HW adapter knowing them.            |
| **Strategy**          | `i_dtc_monitor`, `i_recovery_action`             | Each DTC condition and each recovery step is an interchangeable algorithm. Adding new ones doesn't touch the core. |
| **Template Method**   | `dtc_monitor_base::evaluate()` calls the hook `condition_currently_failing()` | Common debounce loop is written once; subclasses only express the *raw* failing condition.                         |
| **State**             | `recovery_manager` ( idle → armed → level1_done → terminal ) | Escalation logic is naturally a state machine; modeling it as one keeps the "no recovery after reboot" invariant trivial. |
| **Adapter**           | `chip_reset_action`, `platform_reboot_action`, `i_dtc_service` impls | Translate generic recovery-action semantics into concrete platform calls (UBX-CFG-RST, HealthService IPC, etc.).   |
| **Dependency Injection** | All `i_*` interfaces                          | Production wiring vs. test wiring (fake clock, fake DTC service) is just a different composition.                  |

## 4. Folder Structure

```
gnss_diagnostic/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── class_diagram.puml
│   ├── sequence_normal.puml
│   ├── sequence_recovery.puml
│   └── sequence_lpm.puml
├── include/
│   └── gnss_diagnostic/
│       ├── dtc_codes.h
│       ├── dtc_config.h
│       ├── gnss_event_types.h
│       ├── i_clock.h
│       ├── i_dtc_service.h
│       ├── i_health_service.h
│       ├── i_chip_controller.h
│       ├── i_gnss_event_source.h
│       ├── i_dtc_monitor.h
│       ├── i_recovery_action.h
│       ├── dtc_monitor_base.h
│       ├── pvt_timeout_monitor.h
│       ├── antenna_short_monitor.h
│       ├── antenna_open_monitor.h
│       ├── chip_reset_action.h
│       ├── platform_reboot_action.h
│       ├── recovery_manager.h
│       ├── power_mode_controller.h
│       └── gnss_diagnostic_manager.h
└── src/
    ├── dtc_monitor_base.cpp
    ├── pvt_timeout_monitor.cpp
    ├── antenna_short_monitor.cpp
    ├── antenna_open_monitor.cpp
    ├── chip_reset_action.cpp
    ├── platform_reboot_action.cpp
    ├── recovery_manager.cpp
    ├── power_mode_controller.cpp
    └── gnss_diagnostic_manager.cpp
```

---

## 5. Diagrams

PlantUML sources are in [docs/](docs/). Render with:

```bash
plantuml -tsvg docs/*.puml
```

- [docs/class_diagram.puml](docs/class_diagram.puml) — full class diagram
- [docs/sequence_normal.puml](docs/sequence_normal.puml) — normal → DTC triggered
- [docs/sequence_recovery.puml](docs/sequence_recovery.puml) — escalation L1 → L2 → terminal
- [docs/sequence_lpm.puml](docs/sequence_lpm.puml) — LPM entry / exit

---

## 6. Usage Guide

### 6.1 Integrating with Location Service

```cpp
#include "gnss_diagnostic/gnss_diagnostic_manager.h"
#include "gnss_diagnostic/pvt_timeout_monitor.h"
#include "gnss_diagnostic/antenna_short_monitor.h"
#include "gnss_diagnostic/antenna_open_monitor.h"
#include "gnss_diagnostic/chip_reset_action.h"
#include "gnss_diagnostic/platform_reboot_action.h"
#include "gnss_diagnostic/recovery_manager.h"
#include "gnss_diagnostic/power_mode_controller.h"

using namespace gnss_diagnostic;

// 1. Build adapters (concrete classes live outside this library).
steady_clock_adapter           clk;
diag_service_adapter           dtc_svc(diag_service_handle);
health_service_adapter         health(health_service_handle);
ublox_chip_controller          chip(ubx_io);
ublox_gnss_event_source        events(ubx_io);

// 2. Build recovery actions (Strategy).
std::vector<std::unique_ptr<i_recovery_action>> actions;
actions.emplace_back(std::make_unique<chip_reset_action>(chip));
actions.emplace_back(std::make_unique<platform_reboot_action>(health));

// 3. Build recovery manager.
recovery_config rcfg{ std::chrono::seconds(10), std::chrono::seconds(20) };
auto rec = std::make_unique<recovery_manager>(clk, rcfg, std::move(actions));

// 4. Build the facade.
gnss_diagnostic_manager mgr(events, clk, std::move(rec));

// 5. Register monitors (configurable per-DTC durations).
mgr.register_monitor(std::make_unique<pvt_timeout_monitor>(
    dtc_config{ dtc_codes::pvt_stream_timeout,
                std::chrono::seconds(5),
                std::chrono::seconds(5) },
    dtc_svc, clk, std::chrono::milliseconds(500)));

mgr.register_monitor(std::make_unique<antenna_short_monitor>(
    dtc_config{ dtc_codes::antenna_short_to_gnd,
                std::chrono::seconds(2),
                std::chrono::seconds(3) },
    dtc_svc, clk));

mgr.register_monitor(std::make_unique<antenna_open_monitor>(
    dtc_config{ dtc_codes::antenna_open_circuit,
                std::chrono::seconds(2),
                std::chrono::seconds(3) },
    dtc_svc, clk));

// 6. Hook power mode and start ticking.
power_mode_controller pmc(mgr);
mgr.start();

// In the Location Service main loop, every 100 ms:
mgr.on_tick(clk.now());

// On power-mode events from PowerMgmt service:
pmc.notify(power_mode::low_power);
// ...later
pmc.notify(power_mode::active);
```

### 6.2 Adding a new DTC condition

1. Define the DTC code in `dtc_codes.h`.
2. Subclass `dtc_monitor_base`.
3. Override `condition_currently_failing(now)` and (optionally)
   `on_pvt` / `on_antenna` to capture the raw signal.
4. Register the new monitor with `gnss_diagnostic_manager::register_monitor()`.

No other file needs to be modified — that is the Open/Closed principle
at work.

```cpp
class jamming_monitor final : public dtc_monitor_base {
public:
    using dtc_monitor_base::dtc_monitor_base;
    void on_pvt(const pvt_event& ev) override { last_cn0_ = derive_cn0(ev); }
protected:
    bool condition_currently_failing(time_point) const override {
        return last_cn0_ < cn0_floor_dbhz_;
    }
private:
    double last_cn0_ = 99.0;
    static constexpr double cn0_floor_dbhz_ = 25.0;
};
```

### 6.3 How recovery works

1. Any monitor sets a DTC → `recovery_manager::on_dtc_set` is called →
   state moves `idle → armed`, timer starts.
2. If *any* DTC is still active after `level1_timeout`, the level-1
   action (`chip_reset_action`) is executed → state moves `armed →
   level1_done`.
3. If a DTC is still active `level2_timeout` after the chip reset, the
   level-2 action (`platform_reboot_action`) is executed → state moves
   `level1_done → terminal`. From this point on, the manager performs
   **no further recovery** until a full system restart.
4. If all DTCs clear at any point, state goes back to `idle` and the
   escalation can run again the next time something fails.
5. While the system is in LPM, both monitors and the recovery manager are
   paused; timers are re-armed from the resumed `now`, so LPM duration
   is *not* counted toward the escalation deadlines.

### 6.4 Adding a new recovery step

Implement `i_recovery_action`, define a new value in
`recovery_level`, push it into the vector passed to `recovery_manager`,
and extend the state machine if the new step needs its own state. The
existing actions (`chip_reset_action`, `platform_reboot_action`) are
unaffected.

---

## 7. Design Principles in This Context

- **Centralised DTC ownership** — the only places where
  `i_dtc_service::set_dtc / clear_dtc` is called are
  `dtc_monitor_base::evaluate()` and `dtc_monitor_base::reset()`. Grep
  for `set_dtc(` outside this file should return empty in the entire
  codebase.
- **No global state** — every collaborator is injected. Tests can wire
  up fake clocks, fake event sources and fake DTC services to drive
  multi-second debounce windows in microseconds of test time.
- **Chip-agnostic by construction** — the only thing the diagnostic
  library knows about the GNSS chip is `bool reset_chip()`. UBX, NMEA,
  AT-command, custom binary protocols are all hidden behind
  `i_chip_controller`.
- **Production fail-safe behaviour** — terminal state is sticky; we
  don't loop reboots, we don't fight the platform, and we don't silently
  retry after exhausting the escalation budget.
- **Microservice-friendly** — the library has no IPC dependencies, no
  threads, no heap allocations on the hot path beyond initial
  registration. It can be linked into a Location Service microservice
  without dragging in vsomeip/gRPC/AUTOSAR specifics.
- **Easy to simulate** — all timing flows through `i_clock::now()`. Unit
  tests fast-forward time deterministically and assert exact
  set/clear/escalation transitions.
