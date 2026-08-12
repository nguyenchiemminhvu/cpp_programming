#include <iostream>
#include <pthread.h>

class DiagnosticSession
{
public:
    // Explicit constructor configures thread-safety resources
    explicit DiagnosticSession(uint16_t session_id) noexcept 
        : session_id_(session_id), is_active_(true)
    {
        pthread_mutex_init(&session_lock_, nullptr);
    }

    // RAII teardown
    ~DiagnosticSession() noexcept
    {
        pthread_mutex_destroy(&session_lock_);
    }

    // Strict automotive rule: Pin resource to its allocated address space
    DiagnosticSession(const DiagnosticSession&) = delete;
    DiagnosticSession& operator=(const DiagnosticSession&) = delete;
    DiagnosticSession(DiagnosticSession&&) = delete;
    DiagnosticSession& operator=(DiagnosticSession&&) = delete;

    uint16_t get_id() const noexcept { return session_id_; }

private:
    uint16_t session_id_;
    bool is_active_;
    pthread_mutex_t session_lock_;
};

// Zero-overhead factory wrapper passing a prvalue
DiagnosticSession initialize_diagnostic_hub(uint16_t requested_id) noexcept
{
    // Evaluates a prvalue initialization instruction
    return DiagnosticSession(requested_id); 
}

int main()
{
    // Construct directly into stack memory frame without any intermediate copies/moves
    DiagnosticSession active_session = initialize_diagnostic_hub(0x003CU);
    
    static_assert(!std::is_move_constructible_v<DiagnosticSession>, "Safety failure: session must be unmovable!");
    return active_session.get_id() == 0x003CU ? 0 : 1;
}
