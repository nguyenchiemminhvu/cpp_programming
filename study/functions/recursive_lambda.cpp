#include <iostream>
#include <functional>

int main()
{
    std::string s = "abc";
    std::string t = "ahbgdc";

    std::function<bool(int, int)> is_subsequence;
    is_subsequence = [&](int is, int it) {
        if (is == s.length()) return true;
        if (it == t.length()) return false;

        if (s[is] == t[it])
        {
            return is_subsequence(is + 1, it + 1);
        }
        else
        {
            return is_subsequence(is, it + 1);
        }
    };
    std::cout << std::boolalpha << is_subsequence(0, 0) << std::endl;

    return 0;
}