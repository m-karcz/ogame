#include "ITime.hpp"
#include "Logger.hpp"

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
    void shiftTimeBy(const Duration& duration) override
    {
        setTimeTo(getTimestamp() + duration);
    }
private:
    Timestamp getNow()
    {
        return std::chrono::time_point_cast<Duration>(std::chrono::system_clock::now());
    }
    Timestamp referenceTime{};
    Timestamp timeOfReferenceTimeBeingSet{};
};

