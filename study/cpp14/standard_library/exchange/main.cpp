#include <iostream>
#include <utility>

class port
{
public:
    port(int fd) : fd(fd) {}
    port(const port&) = delete; // Disable copy constructor
    port& operator=(const port&) = delete; // Disable copy assignment

    port(port&& other) noexcept
        : fd(std::exchange(other.fd, -1))
    {
    }

    int get_fd() const { return fd; }

private:
    int fd;
};

int main()
{
    int val = 42;
    std::cout << "Before exchange: " << val << std::endl;

    int old_val = std::exchange(val, 100);
    std::cout << "After exchange: " << val << std::endl;
    std::cout << "Old value: " << old_val << std::endl;

    port p1(10);
    std::cout << "p1 fd: " << p1.get_fd() << std::endl;
    port p2(std::move(p1));
    std::cout << "p2 fd: " << p2.get_fd() << std::endl;
    std::cout << "p1 fd after move: " << p1.get_fd() << std::endl; // p1's fd is now -1 after move

    return 0;
}
