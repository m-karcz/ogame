#include "ITime.hpp"

struct Time : ITime
{
    void setTimeTo(const Timestamp& reference) override
    {
        timeOfReferenceTimeBeingSet = getNow();
        referenceTime = reference;
    }
    Timestamp getTimestamp() override
    {
        auto now = getNow();
        return now - timeOfReferenceTimeBeingSet + referenceTime;
    }
private:
    Timestamp getNow()
    {
        return std::chrono::time_point_cast<Duration>(std::chrono::steady_clock::now());
    }
    Timestamp referenceTime{};
    Timestamp timeOfReferenceTimeBeingSet{};
};

