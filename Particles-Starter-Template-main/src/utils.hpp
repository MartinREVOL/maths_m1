#pragma once
#include "glm/glm.hpp"
#include <optional>

namespace utils {

float rand(float min, float max);
void  draw_disk(glm::vec2 position, float radius, glm::vec4 const& color);
void  draw_line(glm::vec2 start, glm::vec2 end, float thickness, glm::vec4 const& color);
// inline glm::vec2 intersection;

// Détection d'intersection entre deux segments
std::optional<glm::vec2> segment_intersection(glm::vec2 p1, glm::vec2 p2, glm::vec2 q1, glm::vec2 q2);

std::optional<glm::vec2> segment_circle_intersection(glm::vec2 p1, glm::vec2 p2, glm::vec2 center, float radius);

int rand_int(int min, int max);

} // namespace utils
