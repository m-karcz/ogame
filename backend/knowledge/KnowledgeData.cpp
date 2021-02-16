#include "KnowledgeData.hpp"
#include "toJson.hpp"
#include <string_view>

std::string_view rawKnowledge{
    #include KNOWLEDGE_PATH
};

Knowledge makeKnowledge()
{
    Json j = Json::parse(rawKnowledge);
    auto result = deserializeTo<Knowledge>(j);
    return result;
}


const Knowledge knowledgeData = makeKnowledge();

