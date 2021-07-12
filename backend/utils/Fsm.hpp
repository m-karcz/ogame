#pragma once

#include <variant>
#include <tuple>
#include <queue>
#include <optional>
#include "Overloaded.hpp"

template<typename...T>
struct TypeList
{};

template<typename T>
struct Transition
{};

template<typename...T>
using Transitions = std::variant<T...>;

struct NoTransition
{};

template<typename T>
using PossibleTransition = std::variant<NoTransition, T>;

template<typename...T>
using PossibleTransitions = std::variant<NoTransition, T...>;

template<template<typename...> class Wrapper, typename...Types>
Wrapper<Types...> wrapTypeListImpl(const TypeList<Types...>&);

template<template<typename...> class Wrapper, typename TypeListT>
using WrapTypeList = decltype(wrapTypeListImpl<Wrapper>(std::declval<TypeListT>()));

#define MARK_UNEXPECTED(EventType) template<typename T> NoTransition process(const T&, const EventType&) { throw "no elo"; }

template<typename EventList, typename StateList, typename Context>
struct Fsm
{
    Fsm(const Context& ctx) : ctx{ctx}
    {}
    using EventVariant = WrapTypeList<std::variant, EventList>;
    using StateVariant = WrapTypeList<std::variant, StateList>;
    void processEvent(const EventVariant& event)
    {
        if(isAlreadyProcessing)
        {
            events.push(event);
        }
        else
        {
            isAlreadyProcessing = true;
            while(not events.empty())
            {
                auto eventCopy = events.front();
                events.pop();
                visitEvent(eventCopy);
            }
            isAlreadyProcessing = false;
        }
    }
    
    void visitEvent(const EventVariant& eventVariant)
    {
        std::optional<StateVariant> nextState;
        std::visit([&, this](const auto& event){
            std::visit([&, this](auto& state){
                nextState = considerTransition(state.process(ctx, event));
            }, currentState);
        }, eventVariant);
        if(nextState)
        {
            currentState = std::move(*nextState);
        }
    }

    std::optional<StateVariant> considerTransition(const NoTransition&)
    {
        return std::nullopt;
    }
    template<typename...T>
    std::optional<StateVariant> considerTransition(const Transitions<T...> transitions)
    {
        //possible place to optimisation
        return std::visit(
            [this](const auto& transition){return considerTransition(transition); },
            transitions);
    }
    template<typename T>
    std::optional<StateVariant> considerTransition(const Transition<T> transition)
    {   
        return T{};
    }
    Context ctx;
    std::queue<EventVariant> events{};
    bool isAlreadyProcessing = false;
    StateVariant currentState{};
};