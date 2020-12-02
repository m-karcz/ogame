#include <cstdio>
#include <thread>

#include <iostream>
#include <fstream>
#include <algorithm>

void putDebug(std::string str)
{
    std::ofstream debug2("debug2.txt", std::ios::out);
    debug2 << str;
}
int main()
{
    while(true)
    {
        std::string line;
        /*while(std::getline(std::cin, line) and line != "req")
        {}*/
        do {
            std::getline(std::cin, line);
            if(line == "exit")
            {
                putDebug("exited");
                return 0;
            }
        }while(line != "req");
        std::string path{tmpnam(nullptr)};
        std::cout << path << EOF << std::endl;
        while(std::getline(std::cin, line) and line != "ready")
        {}
        //std::this_thread::sleep_for(std::chrono::seconds{1});
        std::string content;
        {
            std::ifstream contentFile(path.c_str(), std::ios::in);
            std::copy(std::istreambuf_iterator<char>{contentFile},
                      std::istreambuf_iterator<char>{},
                      std::back_inserter(content));
        }
        //putDebug(path);
        //putDebug(content);
        while(isspace(content.back()))
        {
            content.pop_back();
        }
        if(content == "exit")
        {
            return 0;
        }
        std::reverse(content.begin(), content.end());
        {
            std::ofstream outputFile(path.c_str(), std::ios::out | std::ios::trunc);
            std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(outputFile));

         /*   std::ofstream outputFile2("debug.txt", std::ios::out | std::ios::trunc);
            //std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(outputFile2));
            outputFile2 << content.size();*/
        }
        std::cout << "resp" << std::endl;
    }

}
