#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>

#include "JsonParser/JsonNode.h"

class JsonFile
{
  public:
    JsonFile();
    JsonFile(const std::filesystem::path &p);
    JsonFile(nlohmann::json data);

    bool isGood() { return good; }

    JsonNode              getRootNode() const;
    nlohmann::json       &getRawData();
    const nlohmann::json &getRawData() const;

  private:
    bool                  good{true};
    nlohmann::json        data;
    std::filesystem::path path;
};