#include <iostream>

enum Color
{
    Red,
    Green,
    Blue
};

enum class Direction
{
    North,
    South,
    East,
    West
};

int mix_direction(Direction dir1, Direction dir2)
{
    // build full 8 directions by mixing two directions
    // also check invalid combinations
    if ((dir1 == Direction::North && dir2 == Direction::South) ||
        (dir1 == Direction::South && dir2 == Direction::North) ||
        (dir1 == Direction::East && dir2 == Direction::West) ||
        (dir1 == Direction::West && dir2 == Direction::East))
    {
        std::cout << "Invalid combination of directions." << std::endl;
        return -1; // Indicate invalid combination
    }

    return (1 << static_cast<int>(dir1)) | (1 << static_cast<int>(dir2)); // Return bitmask of mixed directions
}

int main()
{
    int colorValue = Red; // Implicit conversion from enum to int
    std::cout << "Color value: " << colorValue << std::endl;

    Direction dir = Direction::North; // Scoped enum, no implicit conversion to int
    // int dirValue = dir; // Error: cannot convert 'Direction' to 'int
    int dirValue = static_cast<int>(dir); // Explicit conversion using static_cast
    std::cout << "Direction value: " << dirValue << std::endl;

    Direction dir1 = Direction::North;
    Direction dir2 = Direction::South;
    int mixedDirections = mix_direction(dir1, dir2);
    if (mixedDirections != -1)
    {
        std::cout << "Mixed directions bitmask: " << mixedDirections << std::endl;
    }

    dir1 = Direction::North;
    dir2 = Direction::East;
    mixedDirections = mix_direction(dir1, dir2);
    if (mixedDirections != -1)
    {
        std::cout << "Mixed directions bitmask: " << mixedDirections << std::endl;
    }

    return 0;
}
