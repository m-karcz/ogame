#pragma once

#include <boost/hana/define_struct.hpp>

struct RequirementListRequest
{
    BOOST_HANA_DEFINE_STRUCT(RequirementListRequest);
};

template<typename Entity>
struct EntitedRequirement
{
    BOOST_HANA_DEFINE_STRUCT(EntitedRequirement,
                             (Entity,entity),
                             (std::vector<Requirement>,requirements)
};

struct RequirementListResponse
{

};