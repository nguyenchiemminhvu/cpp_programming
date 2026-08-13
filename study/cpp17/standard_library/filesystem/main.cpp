#include <iostream>
#include <string>
#include <experimental/filesystem>

using fs = std::experimental::filesystem;

void test_paths()
{
    try
    {
        bool exists = fs::exists("CMakeLists.txt");
        std::cout << "CMakeLists.txt exists: " << std::boolalpha << exists << std::endl;

        exists = fs::exists("nonexistent_file.txt");
        std::cout << "nonexistent_file.txt exists: " << std::boolalpha << exists << std::endl;
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }

    std::error_code ec;
    fs::file_status status = fs::status("CMakeLists.txt", ec);
    if (ec)
    {
        std::cerr << "Error getting file status: " << ec.message() << std::endl;
    }
}

void test_directory_iterator()
{
    try
    {
        fs::path dir_path = ".";
        if (fs::exists(dir_path) && fs::is_directory(dir_path))
        {
            std::cout << "Contents of directory: " << dir_path << std::endl;
            for (const auto& entry : fs::directory_iterator(dir_path))
            {
                std::cout << entry.path() << std::endl;
            }
        }
        else
        {
            std::cerr << "Directory does not exist or is not a directory: " << dir_path << std::endl;
        }
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

void test_recursive_directory_iterator()
{
    try
    {
        fs::path dir_path = ".";
        if (fs::exists(dir_path) && fs::is_directory(dir_path))
        {
            std::cout << "Recursive contents of directory: " << dir_path << std::endl;
            for (const auto& entry : fs::recursive_directory_iterator(dir_path))
            {
                std::cout << entry.path() << std::endl;
            }
        }
        else
        {
            std::cerr << "Directory does not exist or is not a directory: " << dir_path << std::endl;
        }
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

int main()
{
    test_paths();
    test_directory_iterator();
    test_recursive_directory_iterator();

    return 0;
}
