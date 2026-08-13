#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <execution>
#include <chrono>

int main()
{
    std::vector<int> vec(10000000);
    std::iota(vec.begin(), vec.end(), 1);

    auto start_seq = std::chrono::high_resolution_clock::now();
    std::sort(vec.begin(), vec.end(), std::greater<int>());
    auto end_seq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_seq = end_seq - start_seq;
    std::cout << "Sequential sort took " << duration_seq.count() << " seconds." << std::endl;

    auto start_par = std::chrono::high_resolution_clock::now();
    std::sort(std::execution::par, vec.begin(), vec.end());
    auto end_par = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_par = end_par - start_par;
    std::cout << "Parallel sort took " << duration_par.count() << " seconds." << std::endl;
    
    std::transform(std::execution::par_unseq, vec.begin(), vec.end(), vec.begin(), [](auto val) noexcept { return val * 2; });

    return 0;
}
