#include <iostream>
#include <random>

int main()
{
    // 1. Fetch a seed value from the system hardware
    std::random_device rd;
    
    // 2. Initialize the engine with the seed
    std::mt19937 gen(rd());
    
    // 3. Define a uniform integer distribution mapping
    std::uniform_int_distribution<int> attrib_dist(1, 6);

    // Roll the dice 5 times
    for (size_t i = 0; i < 5; ++i)
    {
        std::cout << "Dice output: " << attrib_dist(gen) << "\n";
    }
    return 0;
}
