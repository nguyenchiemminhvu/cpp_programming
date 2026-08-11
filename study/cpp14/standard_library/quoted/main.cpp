#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

int main()
{
    std::stringstream ss;
    std::string text = "CAN-Bus \"Critical\" Error";

    // Serialize string with embedded quotes and spaces cleanly
    ss << std::quoted(text);
    std::cout << "Serialized: " << ss.str() << "\n"; // Outputs: "CAN-Bus \"Critical\" Error"

    std::string parsed_text;
    // Deserialize back into standard form automatically unescaping characters
    ss >> std::quoted(parsed_text);
    std::cout << "Deserialized: " << parsed_text << "\n";

    return 0;
}
