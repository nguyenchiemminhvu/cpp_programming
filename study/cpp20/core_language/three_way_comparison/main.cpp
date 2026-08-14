#include <iostream>
#include <string>
#include <cstdint>
#include <queue>
#include <compare>

class event
{
public:
    event(uint32_t id, int priority, int timestamp)
        : id_(id), priority_(priority), timestamp_(timestamp)
    {

    }

    // getters
    inline uint32_t id() const { return id_; }
    inline int priority() const { return priority_; }
    inline int timestamp() const { return timestamp_; }

    auto operator<=>(const event& other) const
    {
        if (priority_ != other.priority_)
        {
            return other.priority_ <=> priority_; // DESCENDING order
        }
        else if (timestamp_ != other.timestamp_)
        {
            return timestamp_ <=> other.timestamp_; // ASCENDING order
        }
        else
        {
            return id_ <=> other.id_; // ASCENDING order
        }
    }

private:
    uint32_t id_;
    int priority_;
    int timestamp_;
};

class event_queue
{
public:
    void push(const event& e)
    {
        queue_.push(e);
    }

    bool empty() const
    {
        return queue_.empty();
    }

    event pop()
    {
        event e = queue_.top();
        queue_.pop();
        return e;
    }

private:
    std::priority_queue<event> queue_;
};

int main()
{
    event_queue eq;
    eq.push(event(1, 2, 100));
    eq.push(event(2, 1, 200));
    eq.push(event(3, 2, 50));

    while (!eq.empty())
    {
        event e = eq.pop();
        std::cout << "Processed event with ID: " << e.id() 
                  << ", Priority: " << e.priority() 
                  << ", Timestamp: " << e.timestamp() 
                  << std::endl;
    }

    return 0;
}
