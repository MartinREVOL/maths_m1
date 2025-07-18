#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <functional>
#include <vector>

void draw_parametric(std::function<glm::vec2(float)> const& parametric, int segments = 100, float thickness = 0.005f, glm::vec4 color = glm::vec4(1.f)) {
    for (int i = 0; i < segments; ++i) {
        float t1 = static_cast<float>(i) / segments;
        float t2 = static_cast<float>(i + 1) / segments;
        glm::vec2 p1 = parametric(t1);
        glm::vec2 p2 = parametric(t2);
        utils::draw_line(p1, p2, thickness, color);
    }
}

int main() {
    gl::init("Point le plus proche sur une courbe de Bézier");
    gl::maximize_window();

    while (gl::window_is_open()) {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Courbe de Bézier cubique
        glm::vec2 p0 = {-0.5f, -0.5f};
        glm::vec2 p1 = {-0.2f,  0.5f};
        glm::vec2 p2 = gl::mouse_position(); // point de contrôle interactif
        glm::vec2 p3 = { 0.5f,  0.5f};

        draw_parametric([&](float t) {
            return utils::bezier3_bernstein(p0, p1, p2, p3, t);
        }, 100, 0.005f, glm::vec4(1.f, 0.f, 0.f, 1.f));

        // Point cible fixe
        glm::vec2 target = glm::vec2(0.2f, -0.6f);
        utils::draw_disk(target, 0.015f, glm::vec4(1.f, 0.f, 1.f, 1.f)); // rose

        // Calcul du point le plus proche
        float closest_t = utils::find_closest_t_on_bezier(target, p0, p1, p2, p3);
        glm::vec2 closest_point = utils::bezier3_bernstein(p0, p1, p2, p3, closest_t);
        utils::draw_disk(closest_point, 0.015f, glm::vec4(0.f, 1.f, 1.f, 1.f)); // cyan

        // Ligne entre point cible et plus proche point
        utils::draw_line(target, closest_point, 0.002f, glm::vec4(1.f));
    }

    return 0;
}
