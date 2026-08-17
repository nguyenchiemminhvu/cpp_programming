#include <stdio.h>

class file_handle
{
public:
    file_handle(const char* filename)
    {
        file = fopen(filename, "w");
        if (!file)
        {
            throw std::runtime_error("Failed to open file");
        }
    }

    ~file_handle()
    {
        // RAII: Resource Acquisition Is Initialization
        // The destructor automatically releases the resource (file) when the object goes out of scope
        if (file)
        {
            fclose(file);
        }
    }

    void write(const char* data)
    {
        if (file)
        {
            fputs(data, file);
        }
    }

private:
    FILE* file;
};

int main()
{
    try
    {
        file_handle fh("example.txt");
        fh.write("Hello, RAII!");
    }
    catch (const std::exception& e)
    {
        printf("Error: %s\n", e.what());
    }

    return 0;
}