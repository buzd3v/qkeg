#include <fmt/format.h>
#include <stdexcept>

template <typename T>
void JsonNode::getIfExists(const std::string &valueName, T &obj) const
{
    if (!hasKey(valueName))
    {
        return;
    }
    get(valueName, obj);
}

template <typename T>
void JsonNode::get(const std::string &valueName, T &obj) const
{
    get_impl(valueName, obj);
}

template <typename T>
void JsonNode::get(const std::string &valueName, T &obj, const T &defaultValue) const
{
    get_impl(valueName, obj, &defaultValue);
}

template <typename T>
bool JsonNode::hasKey(const T &key) const
{
    return data.find(key) != data.end();
}

template <typename T>
void JsonNode::get_impl(const std::string &valueName, T &obj, const T *defaultValuePtr) const
{
    static_assert(!std::is_enum_v<T>, "Use getEnumValue instead!");

    if (const auto it = data.find(valueName); it != data.end())
    {
        try
        {
            obj = it.value().get<T>();
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error(fmt::format("Can't get field '{}' from '{}' : {} ", valueName, name, e.what()));
        }
    }
    else
    {
        if (overrideMode)
        {
            return; // nothing to do
        }
        if (!defaultValuePtr) // if defaultValuePtr not found
        {
            throw std::runtime_error(fmt::format("can't get field '{}' from '{}': the key doesn't "
                                                 "exist and no default value was provided",
                                                 valueName,
                                                 name));
        }
        obj = *defaultValuePtr;
    }
}

template <typename T>
std::vector<T> JsonNode::asVectorOf() const
{
    std::vector<T> v;
    v.reserve(data.size());
    for (const auto &value : data)
    {
        v.push_back(value);
    }
    return v;
}
