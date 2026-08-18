#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <memory>

class engine
{
public:
    template <typename T>
    explicit engine(T obj)
        : p_concept(std::make_unique<engine_model<T>>(std::move(obj)))
    {
    }

    engine(const engine& other)
        : p_concept(other.p_concept ? other.p_concept->clone() : nullptr)
    {
    }

    engine& operator=(const engine& other)
    {
        if (this != &other)
        {
            p_concept = other.p_concept ? other.p_concept->clone() : nullptr;
        }
        return *this;
    }

    engine(engine&&) noexcept = default;
    engine& operator=(engine&&) noexcept = default;

    void start()
    {
        if (p_concept)
        {
            p_concept->start();
        }
    }

    void stop()
    {
        if (p_concept)
        {
            p_concept->stop();
        }
    }

private:
    class engine_concept
    {
    public:
        virtual ~engine_concept() = default;
        virtual void start() = 0;
        virtual void stop() = 0;

        virtual std::unique_ptr<engine_concept> clone() const = 0;
    };

    template <typename T>
    class engine_model : public engine_concept
    {
    public:
        explicit engine_model(T obj)
            : engine_(std::move(obj))
        {
        }

        void start() override
        {
            engine_.start();
        }

        void stop() override
        {
            engine_.stop();
        }

        std::unique_ptr<engine_concept> clone() const override
        {
            return std::make_unique<engine_model<T>>(engine_);
        }
    
    private:
        T engine_;
    };

    std::unique_ptr<engine_concept> p_concept;
};

class position_engine
{
public:
    void start()
    {
        std::cout << "position engine start" << std::endl;
    }

    void stop()
    {
        std::cout << "position engine stop" << std::endl;
    }
};

#endif // ENGINE_H