#include "engine.h"

class engine::engine_impl
{
public:
    engine_impl() { std::cout << "engine_impl created" << std::endl; }
    ~engine_impl() { std::cout << "engine_impl destroyed" << std::endl; }

    void start() { std::cout << "engine_impl started" << std::endl; }
    void stop() { std::cout << "engine_impl stopped" << std::endl; }
};

engine::engine()
    : pimpl(std::make_unique<engine_impl>())
{
}

engine::~engine() = default;

void engine::start()
{
    pimpl->start();
}

void engine::stop()
{
    pimpl->stop();
}