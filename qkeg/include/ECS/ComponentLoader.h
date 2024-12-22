#pragma once

#include "JsonParser/JsonNode.h"
#include <entt/entt.hpp>
#include <fmt/format.h>
#include <string>
#include <type_traits>

#include <unordered_map>

class ComponentLoader
{
  public:
    template <typename ComponentType>
    void registerComponent(std::string componentName);

    template <typename Func>
    void registerComponent(std::string componentName, Func func);

    bool isRegistered(std::string componentName) { return marker.contains(componentName); }

    void makeComponent(entt::handle handle, std::string componentName, const JsonNode &componentNode);

  private:
    std::unordered_map<std::string, std::function<void(entt::handle &, const JsonNode &)>> marker;
};

template <typename ComponentType>
inline void ComponentLoader::registerComponent(std::string componentName)
{
    auto [compName, inserted] = marker.emplace(componentName, [](entt::handle &handle, const JsonNode &compNode) {
        if (!handle.all_of<ComponentType>())
        {
            handle.emplace<ComponentType>();
        }
    });
    if (!inserted)
    {
        fmt::println("The component {} is already be registered!", componentName);
    }
}

template <typename Func>
inline void ComponentLoader::registerComponent(std::string componentName, Func func)
{
    using ComponentType = std::remove_cvref_t<entt::nth_argument_t<1u, Func>>;
    static_assert(std::is_same_v<entt::nth_argument_t<2u, Func>, const JsonNode &>,
                  "the third argument of the lambda should be const JsonDataLoader&");
    static_assert(std::is_same_v<entt::nth_argument_t<0u, Func>, entt::handle>,
                  "the first argument of the lambda should be entt::handle");

    const auto [temp, inserted] = marker.emplace(componentName, [func](entt::handle &handle, const JsonNode &compNode) {
        auto &comp = handle.get_or_emplace<ComponentType>();
        func(handle, comp, compNode);
    });
    if (!inserted)
    {
        fmt::println("The component {} is already be registered!", componentName);
    }
}
