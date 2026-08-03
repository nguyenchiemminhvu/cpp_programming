#include <iostream>

class network_config
{
public:
    network_config() : network_config("localhost", 8080) {}
    network_config(const std::string& host, int port) : host_(host), port_(port) {}
    network_config(const network_config& other) : network_config(other.host_, other.port_) {}
    network_config(network_config&& other) : network_config(std::move(other.host_, other.port_)) {}

private:
    std::string host_;
    int port_;
};

int main()
{
    return 0;
}