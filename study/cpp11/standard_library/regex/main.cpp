#include <iostream>
#include <vector>
#include <regex>

bool match_pattern(const std::string& text, const std::string& pattern)
{
    std::regex pat(pattern);
    return std::regex_match(text, pat);
}

std::vector<std::string> find_matches(const std::string& text, const std::string& pattern)
{
    std::vector<std::string> matches;
    std::regex pat(pattern);
    std::sregex_iterator begin(text.begin(), text.end(), pat);
    std::sregex_iterator end;
    while (begin != end)
    {
        matches.push_back(begin->str());
        ++begin;
    }

    return matches;
}

std::string replace_matches(const std::string& text, const std::string& pattern, const std::string& replacement)
{
    std::regex pat(pattern);
    return std::regex_replace(text, pat, replacement);
}

void test_match_email()
{
    std::string email = "example@example.com";
    std::string email_pattern = R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)";
    bool is_match = match_pattern(email, email_pattern);
    std::cout << "Email match: " << is_match << std::endl;
}

void test_match_phone_number()
{
    std::string phone_number = "+1-800-555-1234";
    std::string phone_pattern = R"(\+?\d{1,3}-\d{3}-\d{3}-\d{4})";
    bool is_match = match_pattern(phone_number, phone_pattern);
    std::cout << "Phone number match: " << is_match << std::endl;
}

void test_match_url()
{
    std::string url = "https://www.example.com";
    std::string url_pattern = R"(https?://([\w.-]+)(:\d+)?(/[\w./-]*)?)";
    bool is_match = match_pattern(url, url_pattern);
    std::cout << "URL match: " << is_match << std::endl;
}

void test_match_date()
{
    std::string date = "2023-06-15";
    std::string date_pattern = R"(\d{4}-\d{2}-\d{2})";
    bool is_match = match_pattern(date, date_pattern);
    std::cout << "Date match: " << is_match << std::endl;
}

void test_match_ipv4_address()
{
    std::string ipv4_address = "192.168.0.1";
    std::string ipv4_pattern = R"((\d{1,3}\.){3}\d{1,3})";
    bool is_match = match_pattern(ipv4_address, ipv4_pattern);
    std::cout << "IPv4 address match: " << is_match << std::endl;
}

void test_find_matches()
{
    std::string text = "The quick brown fox jumps over the lazy dog. The quick brown fox is fast.";
    std::string pattern = R"(\b\w{5}\b)"; // Match words with exactly 5 letters
    std::vector<std::string> matches = find_matches(text, pattern);
    std::cout << "Found matches: ";
    for (const auto& match : matches)
    {
        std::cout << match << " ";
    }
    std::cout << std::endl;
}

void test_replace_matches()
{
    std::string text = "The quick brown fox jumps over the lazy dog.";
    std::string pattern = R"(\b\w{5}\b)"; // Match words with exactly 5 letters
    std::string replacement = "*****";
    std::string result = replace_matches(text, pattern, replacement);
    std::cout << "Replaced text: " << result << std::endl;
}

int main()
{
    test_match_email();
    test_match_phone_number();
    test_match_url();
    test_match_date();
    test_match_ipv4_address();
    test_find_matches();
    test_replace_matches();

    return 0;
}
