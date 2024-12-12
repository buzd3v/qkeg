#include "ECS/ComponentLoader.h"

void ComponentLoader::makeComponent(entt::handle handle, std::string componentName, const JsonNode &componentNode)
{
    if (auto it = marker.find(componentName); it != marker.end())
    {
        const auto &func = it->second;
        func(handle, componentNode);
    }
    else
    {
        throw std::runtime_error(fmt::format("Component {} isn't registered yet!", componentName));
    }
}