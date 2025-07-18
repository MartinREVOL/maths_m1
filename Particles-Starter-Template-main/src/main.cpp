#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <functional>

// Fonction générique pour dessiner une courbe paramétrique
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
    gl::init("Courbes de Bézier");
    gl::maximize_window();

    while (gl::window_is_open()) {
        float dt = gl::delta_time_in_seconds();
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Bézier d'ordre 1 (segment)
        draw_parametric([](float t) {
            return utils::bezier1({-0.9f, -0.8f}, {-0.4f, 0.8f}, t);
        }, 1, 0.01f, glm::vec4(1.f, 0.f, 0.f, 1.f)); // rouge, segment

        // Bézier d'ordre 2 (quadratique)
        draw_parametric([](float t) {
            return utils::bezier2({-0.2f, -0.8f}, gl::mouse_position(), {0.2f, -0.8f}, t);
        }, 100, 0.006f, glm::vec4(0.f, 1.f, 0.f, 1.f)); // vert

        // Bézier d'ordre 3 (cubique)
        draw_parametric([](float t) {
            return utils::bezier3({-0.5f, -0.5f}, {-0.2f, 0.5f}, gl::mouse_position(), {0.5f, 0.5f}, t);
        }, 100, 0.005f, glm::vec4(1.f, 1.f, 0.f, 1.f)); // jaune

        // Points de contrôle
        utils::draw_disk({-0.9f, -0.8f}, 0.01f, glm::vec4(1.f, 0.f, 0.f, 1.f));
        utils::draw_disk({-0.4f,  0.8f}, 0.01f, glm::vec4(1.f, 0.f, 0.f, 1.f));

        utils::draw_disk({-0.2f, -0.8f}, 0.01f, glm::vec4(0.f, 1.f, 0.f, 1.f));
        utils::draw_disk({ 0.2f, -0.8f}, 0.01f, glm::vec4(0.f, 1.f, 0.f, 1.f));

        utils::draw_disk({-0.5f, -0.5f}, 0.01f, glm::vec4(1.f, 1.f, 0.f, 1.f));
        utils::draw_disk({ 0.5f,  0.5f}, 0.01f, glm::vec4(1.f, 1.f, 0.f, 1.f));
        utils::draw_disk(gl::mouse_position(), 0.01f, glm::vec4(1.f, 0.5f, 0.2f, 1.f)); // point mobile
    }

    return 0;
}
