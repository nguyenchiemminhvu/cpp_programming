#include <cstdint>
#include <iostream>

namespace hal_configs
{
    constinit uint32_t uart_baudrate = 9600U;
    constinit uint32_t uart_buffer_size = 256U;
    constinit uint32_t default_timeout_ms = 1000U;
    constinit uint32_t max_retries = 5U;

    thread_local constinit uint32_t parser_counter = 0U;
}

void apply_hal_configurations(uint32_t baudrate, uint32_t buffer_size, uint32_t timeout_ms, uint32_t retries)
{
    hal_configs::uart_baudrate = baudrate;
    hal_configs::uart_buffer_size = buffer_size;
    hal_configs::default_timeout_ms = timeout_ms;
    hal_configs::max_retries = retries;
}

int main()
{
    std::cout << "Initial HAL Configurations:" << std::endl;
    std::cout << "UART Baudrate: " << hal_configs::uart_baudrate << std::endl;
    std::cout << "UART Buffer Size: " << hal_configs::uart_buffer_size << std::endl;
    std::cout << "Default Timeout (ms): " << hal_configs::default_timeout_ms << std::endl;
    std::cout << "Max Retries: " << hal_configs::max_retries << std::endl;

    apply_hal_configurations(115200U, 512U, 2000U, 3U);

    std::cout << "\nUpdated HAL Configurations:" << std::endl;
    std::cout << "UART Baudrate: " << hal_configs::uart_baudrate << std::endl;
    std::cout << "UART Buffer Size: " << hal_configs::uart_buffer_size << std::endl;
    std::cout << "Default Timeout (ms): " << hal_configs::default_timeout_ms << std::endl;
    std::cout << "Max Retries: " << hal_configs::max_retries << std::endl;

    return 0;
}
