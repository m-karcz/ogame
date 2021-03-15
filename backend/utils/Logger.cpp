#include "Logger.hpp"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#ifdef __EMSCRIPTEN__
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <sstream>
#include <emscripten.h>
#endif //__EMSCRIPTEN__

std::shared_ptr<spdlog::logger> Logger::chooseLogger()
{
#ifdef __EMSCRIPTEN__
    struct Emsbuf : std::stringbuf
    {
        virtual int sync()
        {
            std::stringstream ss{this->str()};
            std::string line;
            while(std::getline(ss, line))
            {
                EM_ASM({
                    console.log(UTF8ToString($0));
                }, line.data());
            }
            this->str("");
            return 0;
        }
    };
    static Emsbuf buffer;
    static std::ostream stream(&buffer);
    return std::make_shared<spdlog::logger>("basic", std::make_shared<spdlog::sinks::ostream_sink_st>(stream));
#else
    return spdlog::basic_logger_st("basic","eloelo.txt");
#endif //__EMSCRIPTEN__
}