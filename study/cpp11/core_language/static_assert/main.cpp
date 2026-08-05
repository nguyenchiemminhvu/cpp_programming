#include <iostream>

template <typename T, int N>
class array
{
public:
    static_assert(N > 0, "N must be greater than 0");

private:
    T data[N];
};

template <typename T>
class string
{
public:
    static_assert(std::is_same<T, char>::value || std::is_same<T, wchar_t>::value, "T must be char or wchar_t");

private:
    T* data;
};

template <typename T, size_t alignment>
class aligned_storage
{
public:
    static_assert(alignment > 0 && (alignment & (alignment - 1)) == 0, "alignment must be a power of 2");
    static_assert(sizeof(T) <= alignment, "T must fit within the specified alignment");
};

int main()
{
    static_assert(sizeof(int) == 4, "int must be 4 bytes");
    static_assert(sizeof(void*) == 8, "void* must be 8 bytes");

    array<int, 5> arr; // OK
    // array<int, 0> arr2; // Error: N must be greater than 0

    string<char> str; // OK
    // string<int> str2; // Error: T must be char or wchar_t
    return 0;
}
