#include "LoadConfiguration.hpp"
#include "Configuration.hpp"
#include "toJson.hpp"
#include <fstream>
#include "Logger.hpp"
#include <filesystem>

Configuration loadConfiguration(std::string_view path)
{
    std::ifstream file{std::string{path}.c_str()};
    return deserializeTo<Configuration>(Json::parse(file));
}