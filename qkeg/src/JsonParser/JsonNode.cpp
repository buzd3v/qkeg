#include <JsonParser/JsonNode.h>
#include <fmt/format.h>

JsonNode::JsonNode(const nlohmann::json &data) : data(data), overrideMode(false) {}

JsonNode::JsonNode(const nlohmann::json &data, std::string name)
    : data(data), name(std::move(name)), overrideMode(false)
{
}

JsonNode JsonNode::getNode(const std::string &key) const
{
    auto it = data.find(key);
    if (it != data.end())
    {
        JsonNode l(it.value(), fmt::format("{}.{}", name, key));
        l.setOverride(overrideMode);
        return l;
    }
    throw std::runtime_error(fmt::format("{} doesn't have a key {}", name, key));
}

JsonNode JsonNode::getNode(std::size_t i) const
{
    if (i < data.size())
    {
        JsonNode l(data[i], fmt::format("{}[{}]", name, i));
        l.setOverride(overrideMode);
        return l;
    }
    throw std::runtime_error(fmt::format("Can't get index {} from {}: out of bounds", i, name));
}

std::unordered_map<std::string, JsonNode> JsonNode::getKeyValueMap() const
{
    std::unordered_map<std::string, JsonNode> map(data.size());
    for (const auto &[key, value] : data.items())
    {
        JsonNode Node(value, fmt::format("{}.{}", name, key));
        Node.setOverride(overrideMode);
        map.emplace(key, std::move(Node));
    }
    return map;
}

std::unordered_map<std::string, std::string> JsonNode::getKeyValueMapString() const
{
    std::unordered_map<std::string, std::string> map(data.size());
    for (const auto &[key, value] : data.items())
    {
        map.emplace(key, value);
    }
    return map;
}

std::unordered_map<std::string, int> JsonNode::getKeyValueMapInt() const
{
    std::unordered_map<std::string, int> map(data.size());
    for (const auto &[key, value] : data.items())
    {
        map.emplace(key, value);
    }
    return map;
}

std::vector<JsonNode> JsonNode::getVector() const
{
    std::vector<JsonNode> v;
    v.reserve(data.size());
    int i = 0;
    for (auto &value : data)
    {
        JsonNode Node(value, fmt::format("{}[{}]", name, i));
        Node.setOverride(overrideMode);
        v.push_back(std::move(Node));
        ++i;
    }
    return v;
}
