#include "RnDService.hpp"
#include "IStorageDb.hpp"

TimeForwardResponse RnDService::handle(const TimeForwardRequest& req)
{
    time.shiftTimeBy(req.duration);
    return {.status = Ok{} };
}

ClearDatabaseResponse RnDService::handle(const ClearDatabaseRequest&)
{
    storageDb.simulateVersionBreak();
    //storageDb.clearAndRecreate();
    return {};
}
