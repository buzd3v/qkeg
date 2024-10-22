#pragma once

#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>

namespace glm
{
template <typename T>
void to_json(nlohmann::json &j, const glm::vec<2, T> &obj);

template <typename T>
void to_json(nlohmann::json &j, const glm::vec<3, T> &obj);

template <typename T>
void to_json(nlohmann::json &j, const glm::vec<4, T> &obj);

template <typename T>
void from_json(const nlohmann::json &j, glm::vec<2, T> &obj);

template <typename T>
void from_json(const nlohmann::json &j, glm::vec<3, T> &obj);

template <typename T>
void from_json(const nlohmann::json &j, glm::vec<4, T> &obj);
}; // namespace glm

#include "JsonGLM.inl"