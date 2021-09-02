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
            logger.debug("onRequestReception in registerRequestHandler");
            onRequestReception(std::move(req));});
        worker.registerRequestHandler([this](auto req){
            logger.debug("onWorkerReception in registerRequestHandler");
            onWorkerReception(std::move(req));});
    }
    std::queue<Request<ReqT, ResT>> requests;
    std::optional<Request<CombinedT, ReqT>> workers;
    void onRequestReception(Request<ReqT, ResT>&& req)
    {
        logger.debug("onRequestReception");
        requests.push(std::move(req));
        logger.debug("queue size: {}", requests.size());
        if(workers)
        {
            logger.debug("forwarding to worker");
            workers->respond(*requests.front());
            workers.reset();
        }
    }
    void onWorkerReception(Request<CombinedT, ReqT>&& workReq)
    {
        logger.debug("onWorkerReception");
        workers.reset();
        workers.emplace(std::move(workReq));
        
        if(isWorkerReady(**workers))
        {
            logger.debug("worker ready");
        }
        else
        {
            logger.debug("forwarding response");
            logger.debug("queue size: {}", requests.size());
            requests.front().respond(getWorkResponse(**workers));
            requests.pop();
        }
        if(not requests.empty())
        {
            logger.debug("taking new request");
            workers->respond(*requests.front());
            workers.reset();
        }
    }
};