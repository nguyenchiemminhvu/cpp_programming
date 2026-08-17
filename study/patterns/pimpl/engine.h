#ifndef ENGINE_H
#define ENGINE_H

#include <memory>

class engine
{
public:
    engine() { std::cout << "engine created" << std::endl; }
    ~engine() { std::cout << "engine destroyed" << std::endl; }

    void start();
    void stop();

private:
    class engine_impl;

    std::unique_ptr<engine_impl> pimpl;
};

#endif // ENGINE_H