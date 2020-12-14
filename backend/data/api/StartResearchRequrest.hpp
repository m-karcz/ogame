#pragma once

//#include "Research.hpp"
#include <string>
#include <boost/hana/define_struct.hpp>
#include "Status.hpp"

struct StartResearchRequest
{
    BOOST_HANA_DEFINE_STRUCT(StartResearchRequest,
                             (std::string, researchName));
};

struct StartResearchResponse
{
    BOOST_HANA_DEFINE_STRUCT(StartResearchResponse,
                             (Status, status));
};