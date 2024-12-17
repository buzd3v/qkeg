#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

#include "JsonColor.h"
#include "JsonGLM.h"

class JsonNode
{
  public:
    explicit JsonNode(const nlohmann::json &data);
    JsonNode(const nlohmann::json &data, std::string name);

    void setOverride(bool b) { overrideMode = b; };
    bool isOverride() { return overrideMode; }

    const nlohmann::json &getJson() const { return data; }
    const std::string    &getName() const { return name; }
    std::size_t           size() const { return data.size(); }

    JsonNode getNode(const std::string &key) const;
    JsonNode getNode(std::size_t i) const;

    std::unordered_map<std::string, JsonNode>    getKeyValueMap() const;
    std::unordered_map<std::string, std::string> getKeyValueMapString() const;
    std::unordered_map<std::string, int>         getKeyValueMapInt() const;
    std::vector<JsonNode>                        getVector() const;

  public:
    template <typename T>
    void getIfExists(const std::string &valueName, T &obj) const;

    template <typename T>
    void get(const std::string &valueName, T &obj) const;

    template <typename T>
    void get(const std::string &valueName, T &obj, const T &defaultValue) const;

    template <typename T>
    bool hasKey(const T &key) const;

    template <typename T>
    std::vector<T> asVectorOf() const;

  private:
    template <typename T>
    void get_impl(const std::string &valueName, T &obj, const T *defaultValuePtr = nullptr) const;

    const nlohmann::json &data;
    std::string           name;
    bool                  overrideMode{false};
};

#include "JsonNode.inl"
namespace nlohmann
{
template <>
struct adl_serializer<std::filesystem::path>
{
    static void to_json(nlohmann::json &j, const std::filesystem::path &path) { j = path.string(); }

    static void from_json(const nlohmann::json &j, std::filesystem::path &path)
    {
        path = std::filesystem::path(j.get<std::string>());
    }
};
} // namespace nlohmann