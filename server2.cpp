#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include "Service.hpp"
#include "RnDService.hpp"
#include "Time.hpp"
#include "RnDTime.hpp"
#include <chrono>
#include "Logger.hpp"
#include "StorageDb.hpp"
#include "OnPlanetRequest.hpp"
#include "toJson.hpp"
#include <map>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
using tcp = net::ip::tcp;
using namespace std::chrono_literals;

using RequestMap = std::map<std::string, std::function<Json(Json)>>;

struct http_connection : public std::enable_shared_from_this<http_connection>
{
    http_connection(RequestMap& map, tcp::socket socket) : map{map}, socket{std::move(socket)}
    {}
    tcp::socket socket;
    beast::flat_buffer buffer{8192};
    //http::request<http::dynamic_body> request;
    http::request<http::string_body> request;
    http::response<http::dynamic_body> response;
    net::steady_timer deadline{socket.get_executor(), 60s};
    RequestMap& map;


    void start()
    {
        read_request();
    }

    void read_request()
    {
        auto self = shared_from_this();
        http::async_read(
            socket,
            buffer,
            request,
            [self](beast::error_code ec, size_t bytes)
            {
                boost::ignore_unused(bytes);
                if(not ec)
                {
                    self->process_request();
                }
            }
        );
    }

    void process_request()
    {
        response.version(request.version());
        response.keep_alive(false);

        switch(request.method())
        {
            case http::verb::post:
            {
                logger.debug("processing req");
                auto body = nlohmann::json::parse(request.body());
                logger.debug("msg: {}", body.dump());
                auto it = map.find(std::string{request.target()});
                if(it != map.end())
                {
                    auto resp = it->second(body);
                    response.result(http::status::ok);
                    response.set(http::field::content_type, "application/json");
                    beast::ostream(response.body()) << resp;

                    auto self = shared_from_this();

                    response.content_length(response.body().size());

                    http::async_write(socket,
                                      response,
                                      [self](beast::error_code ec, std::size_t)
                                      {
                                            self->socket.shutdown(tcp::socket::shutdown_send, ec);
                                      });
                }

                break;
            }
            default:
            {
                response.result(http::status::bad_request);
                response.set(http::field::content_type, "text/plain");
                beast::ostream(response.body()) << "Invalid";
                break;
            }
        }
    }

};

void http_server(RequestMap& map, tcp::acceptor& acceptor, tcp::socket& socket)
{
    acceptor.async_accept(socket,
            [&](beast::error_code ec)
            {
                if(not ec)
                {
                    std::make_shared<http_connection>(map, std::move(socket))->start();
                    http_server(map, acceptor, socket);
                }
            });

}

int main()
{
    RnDTime rndTime;
    ITime& time = rndTime;
    sqlite::StorageDb storage{""};
    Service service{storage, time};
    RnDService rndService{rndTime};

    RequestMap map;

    map["/general"] = [&](Json j){
        return serializeFrom(service.handleRequest(deserializeTo<GeneralRequest>(j)))["data"];
    };
    map["/on_planet"] = [&](Json j)
    {
        return serializeFrom(service.onPlanetRequest(deserializeTo<OnPlanetRequest>(j)));
    };
    map["/rnd"] = [&](Json j)
    {
        rndService.handleRequest(deserializeTo<RnDRequestVariant>(j));
        nlohmann::json respJson;
        respJson["status"] = "ok";
        return respJson;
    };

    auto address = net::ip::make_address("0.0.0.0");
    unsigned short port = 8080;

    net::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {address, port}};
    tcp::socket socket{ioc};

    http_server(map, acceptor, socket);

    ioc.run();

}