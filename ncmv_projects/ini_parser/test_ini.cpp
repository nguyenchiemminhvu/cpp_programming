#include "ini_parser.h"

#include <iostream>
#include <string>

int main()
{
    const std::string INI_FILE = "sample.ini";

    ini_parser::ini_parser parser;
    try
    {
        parser.load(INI_FILE);
    }
    catch (const ini_parser::config_error& e)
    {
        std::cerr << "Configuration error: " << e.what() << std::endl;
    }
    catch (const ini_parser::io_error& e)
    {
        std::cerr << "I/O error: " << e.what() << std::endl;
    }
    catch (const ini_parser::parse_error& e)
    {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }

    std::cout << "INI file processed successfully." << std::endl;

    if (parser.has("Application.name"))
    {
        ini_parser::config_value app_obj = parser.get("Application.name");
        std::cout << "Application Name: " << app_obj.as_string() << std::endl;
    }
    else
    {
        std::cout << "Key does not exist in section." << std::endl;
    }

    if (parser.has("Database.port"))
    {
        ini_parser::config_value port_val = parser.get("Database.port");
        std::cout << "Database Port: " << port_val.as_int() << std::endl;
    }
    else
    {
        std::cout << "Key does not exist in section." << std::endl;
    }

    if (parser.has("Logging.level"))
    {
        ini_parser::config_value log_level = parser.get("Logging.level");
        std::cout << "Logging Level: " << log_level.as_string() << std::endl;
    }
    else
    {
        std::cout << "Key does not exist in section." << std::endl;
    }

    parser.set("Application.version", ini_parser::config_value("2.0.1"));
    if (parser.save("updated_sample.ini"))
    {
        std::cout << "Updated INI file saved successfully." << std::endl;
    }
    else
    {
        std::cout << "Failed to save updated INI file." << std::endl;
    }

    return 0;
}