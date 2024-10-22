#include "JsonParser/JsonFile.h"
#include <fstream>
JsonFile::JsonFile()
{
    data = nlohmann::json{};
}

JsonFile::JsonFile(nlohmann::json data) : data(std::move(data)) {}

JsonFile::JsonFile(const std::filesystem::path &p)
{
    std::ifstream file(p);
    if (!file.good())
    {
        good = false;
        return;
    }
    file >> data;
    path = p;
}

JsonNode JsonFile::getRootNode() const
{
    assert(good, "File is bad");
    return JsonNode(data, path.string());
}

nlohmann::json &JsonFile::getRawData()
{
    assert(good, "Data is bad");
    return data;
}

const nlohmann::json &JsonFile::getRawData() const
{
    assert(good, "Data is bad");
    return data;
}
