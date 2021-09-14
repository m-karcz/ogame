#pragma once

#include <optional>
#include <queue>
#include <iostream>
#include "Event.hpp"
#include "Logger.hpp"

template<typename ReqT, typename ResT, typename CombinedT>
struct SimpleLoadBalancer
{
    SimpleLoadBalancer(IAsyncResponser<ReqT, ResT>& requester, IAsyncResponser<CombinedT, ReqT>& worker)
    {
        requester.registerRequestHandler([this](auto req){
            onRequestReception(std::move(req));});
        worker.registerRequestHandler([this](auto req){
            onWorkerReception(std::move(req));});
    }
    std::queue<Request<ReqT, ResT>> requests;
    std::optional<Request<CombinedT, ReqT>> workers;
    void onRequestReception(Request<ReqT, ResT>&& req)
    {
        requests.push(std::move(req));
        if(workers)
        {
            workers->respond(*requests.front());
            workers.reset();
        }
    }
    void onWorkerReception(Request<CombinedT, ReqT>&& workReq)
    {
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