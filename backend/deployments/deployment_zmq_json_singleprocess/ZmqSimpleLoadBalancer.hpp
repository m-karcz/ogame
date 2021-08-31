#pragma once

#include "ZmqHelper.hpp"
#include <optional>
#include <queue>
#include <iostream>

template<typename ReqT, typename ResT, typename CombinedT>
struct ZmqSimpleLoadBalancer
{
    ZmqSimpleLoadBalancer(ZmqRouter<ReqT, ResT>& requester, ZmqRouter<CombinedT, ReqT>& worker)
    {
        requester.registerRequestHandler([this](auto req){
            std::cout << "onRequestReception" << std::endl;
            onRequestReception(std::move(req));});
        worker.registerRequestHandler([this](auto req){
            std::cout << "onWorkerReception" << std::endl;
            onWorkerReception(std::move(req));});
    }
    std::queue<Request<ReqT, ResT>> requests;
    std::optional<Request<CombinedT, ReqT>> workers;
    void onRequestReception(Request<ReqT, ResT>&& req)
    {
        std::cout << "received request" << std::endl;
        requests.push(std::move(req));
        if(workers)
        {
            workers->respond(*requests.front());
            workers.reset();
        }
    }
    void onWorkerReception(Request<CombinedT, ReqT>&& workReq)
    {
        std::cout << "received worker message" << std::endl;
        //workers = std::move(workReq);
        workers.reset();
        workers.emplace(std::move(workReq));
        
        if(isWorkerReady(**workers))
        {
        }
        else
        {
            requests.front().respond(getWorkResponse(**workers));
            requests.pop();
        }
        if(not requests.empty())
        {
            workers->respond(*requests.front());
            workers.reset();
        }
    }
};