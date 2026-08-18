#include <iostream>
#include <vector>
#include <cstdint>

struct can_frame
{
    int id;
    std::vector<uint8_t> data;
};

void process_can_frame(const can_frame& frame)
{
    std::cout << "process_can_frame(const can_frame& frame) called" << std::endl;
}

void process_can_frame(can_frame&& frame)
{
    std::cout << "process_can_frame(can_frame&& frame) called" << std::endl;
}

void forward_can_frame(can_frame&& frame)
{
    // frame param has a name, so it is an lvalue, we need to use std::move to cast it to rvalue reference

    process_can_frame(std::move(frame)); // forward rvalue reference
}

can_frame create_can_frame()
{
    can_frame frame;
    frame.id = 1;
    frame.data = {0x01, 0x02, 0x03};
    return frame; // returns an rvalue
}

int main()
{
    int temp = 0;
    int& lvalue_ref = temp; // lvalue reference can bind to lvalue
    // int& lvalue_ref = 10; // lvalue reference can not bind to rvalue
    const int& lvalue_ref2 = 10; // const lvalue reference can bind to rvalue
    int&& rvalue_ref = 10; // rvalue reference can only bind to rvalue

    can_frame frame1;
    process_can_frame(frame1); // lvalue
    process_can_frame(create_can_frame()); // rvalue
    process_can_frame(std::move(frame1)); // rvalue

    forward_can_frame(create_can_frame()); // rvalue

    return 0;
}