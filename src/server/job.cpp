#include <Poco/UUIDGenerator.h>
#include <Poco/UUID.h>

#include <ctime>
#include <algorithm>

#include "job.h"
#include "json.h"

using namespace Batyr;


Job::Job()
    :   status(FAILED),
        timeAdded(std::chrono::system_clock::now())
{
    // generate an UUID as id for the job
    Poco::UUIDGenerator & uuidGen = Poco::UUIDGenerator::defaultGenerator();
    Poco::UUID uuid(uuidGen.createRandom());
    id = uuid.toString();

    // remove all dashes from the string for nicer looking URLs ;)
    std::remove(id.begin(), id.end(), '-');
}


void
Job::toJsonValue(rapidjson::Value & targetValue, rapidjson::Document::AllocatorType & allocator) const
{
    targetValue.SetObject();
    targetValue.AddMember("id", id.c_str(), allocator);

    rapidjson::Value vTimeAdded;
    Batyr::Json::toValue(vTimeAdded, timeAdded, allocator);
    targetValue.AddMember("timeAdded", vTimeAdded, allocator);

    if (isDone()) {
        rapidjson::Value vTimeFinished;
        Batyr::Json::toValue(vTimeFinished, timeFinished, allocator);
        targetValue.AddMember("timeFinished", vTimeFinished, allocator);
    }

    const char * statusString;
    switch (status) {
        case QUEUED:
            statusString = "queued";
            break;
        case IN_PROCESS:
            statusString = "in_process";
            break;
        case FINISHED:
            statusString = "finished";
            break;
        case FAILED:
            statusString = "failed";
            break;
    }
    targetValue.AddMember("status", statusString, allocator);


    if (!errorMessage.empty()) {
        targetValue.AddMember("errorMessage", errorMessage.c_str(), allocator);
    }
}


std::string
Job::toString() const
{
    // usage of rapidjson:
    // http://www.thomaswhitton.com/blog/2013/06/27/json-c-plus-plus-examples/

    rapidjson::Document data;
    toJsonValue(data, data.GetAllocator());
    return Batyr::Json::stringify(data);
}


std::ostream&
Batyr::operator<< (std::ostream& stream, const Job& r)
{
    stream << r.toString();

    return stream;
}

