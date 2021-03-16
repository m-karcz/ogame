#include "LoadConfiguration.hpp"
#include "Configuration.hpp"
#include "toJson.hpp"
#include <fstream>
#include "Logger.hpp"
#include <filesystem>

Configuration loadConfiguration(std::string_view path)
{
    for(auto i : std::filesystem::directory_iterator("/"))
    {
        logger.debug(i.path().c_str());
    }
    std::ifstream file{std::string{path}.c_str()};
    return deserializeTo<Configuration>(Json::parse(file));
}