#include "server_http.hpp"
#include <iostream>
#include <future>
#include "RndRequestVariant.hpp"
#include <nlohmann/json.hpp>
#include "JsonDeserialization.hpp"
#include "StorageDb.hpp"
#include "Service.hpp"
#include "LoginRequest.hpp"
#include "RegisterRequest.hpp"
#include "StorageRequest.hpp"
#include "BuildRequest.hpp"
#include "TimeForwardRequest.hpp"
#include "toJson.hpp"
#include "RnDTime.hpp"
#include "Time.hpp"
#include <boost/program_options.hpp>
#include "Logger.hpp"
#include <spdlog/spdlog.h>


using namespace std;
using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
namespace po = boost::program_options;

int main(int argc, char** argv)
{
    po::options_description desc;
    desc.add_options()
            ("rnd", po::value<std::string>()->implicit_value("yes")->default_value("no"));
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po:notify(vm);

    HttpServer server;
    RnDTime rndTime;
    Time realTime;
    const bool isRnd = vm["rnd"].as<std::string>() == "yes";
    ITime& time = isRnd ? (ITime&)rndTime : (ITime&)realTime;
    std::unique_ptr<Service> service;
    server.config.port = 8080;
    //spdlog::error("no elo");
    //spdlog::get("basicelo")->error("no elo");
    //spdlog::get("basicelo")->flush();
    logger.error("no elo");
    logger.flush();
    server.resource["^/test"]["POST"] = [&](shared_ptr<HttpServer::Response> resp, shared_ptr<HttpServer::Request> req)
    {
        /*std::cout << "sth came" << std::endl;
        auto j = nlohmann::json::parse(req->content);
        std::string dump = j.dump();
        std::cout << "parsed" << std::endl;
        std::cout << j.dump() << std::endl;

        nlohmann::json respJson;
        for(auto i : service->handleRequests(deserializeRequests<RequestVariant>(j)))
        {
            std::visit([&](auto& r){
                nlohmann::json respPart;
                respPart["type"] = boost::core::demangle(typeid(r).name());
                respPart["data"] = r;
                respJson.push_back(respPart);}, i);
        }
        std::string content = respJson.dump();
        *resp << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n";
        *resp << content;*/
    };
    server.resource["^/on_planet"]["POST"] = [&](shared_ptr<HttpServer::Response> resp, shared_ptr<HttpServer::Request> req)
    {
        std::cout << "sth came" << std::endl;
        auto j = nlohmann::json::parse(req->content);
        std::string dump = j.dump();
        std::cout << "parsed" << std::endl;
        logger.debug("Handling req: {}", j.dump());
        std::cout << j.dump() << std::endl;

        OnPlanetRequest parsedRequest{
            .playerId = j["playerId"],
            .planet = j["planet"],
            .requests = deserializeRequests<typename OnPlanetRequest::Variant>(j["requests"])
        };

        logger.debug("Successfuly decoded");

        nlohmann::json respJson;
        for(auto i : service->onPlanetRequest(parsedRequest))
        {
            std::cout << "handled" << std::endl;
            std::visit([&](auto& r){
                nlohmann::json respPart;
                respPart["type"] = boost::core::demangle(typeid(r).name());
                respPart["data"] = r;
                respJson.push_back(respPart);}, i);
            std::cout << "written" << std::endl;
        }
        std::string content = respJson.dump();
        std::cout << "content to send:" << content << std::endl;
        *resp << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n";
        *resp << content;
    };
    if(isRnd)
    {
        server.resource["^/rnd"]["POST"] = [&](shared_ptr<HttpServer::Response> resp, shared_ptr<HttpServer::Request> req)
        {
            auto rndReq = deserializeRequest<RnDRequestVariant>(nlohmann::json::parse(req->content));
            service->handleRequest(rndReq);
            nlohmann::json respJson;
            respJson["status"] = "ok";
            std::string content = respJson.dump();
            *resp << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n";
            *resp << content;
        };

        //server.resource["^/test"]["POST"];
    }
    server.resource["^/general"]["POST"] = [&](shared_ptr<HttpServer::Response> resp, shared_ptr<HttpServer::Request> req)
    {
        auto rndReq = deserializeRequest<GeneralRequest>(nlohmann::json::parse(req->content));
        auto respVariant = service->handleRequest(rndReq);
        nlohmann::json respJson = std::visit([](auto& r)->nlohmann::json{return r;}, respVariant);
        //std::visit([&]())
        //respJson["status"] = "ok";
        std::string content = respJson.dump();
        *resp << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n";
        *resp << content;
    };

    thread server_thread([&]()
                         {
                             sqlite::StorageDb db{""};
                             service = std::make_unique<Service>(Service{db, time});
                             cout << "server works" << endl;
                            server.start();
                         });

    server_thread.join();
}
