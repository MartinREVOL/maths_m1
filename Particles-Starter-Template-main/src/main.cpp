#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <functional>
#include <vector>

// Dessine une courbe paramétrique en reliant des points successifs avec draw_line
void draw_parametric(std::function<glm::vec2(float)> const& parametric, int segments = 100, float thickness = 0.005f, glm::vec4 color = glm::vec4(1.f)) {
    for (int i = 0; i < segments; ++i) {
        float t1 = static_cast<float>(i) / segments;
        float t2 = static_cast<float>(i + 1) / segments;
        glm::vec2 p1 = parametric(t1);
        glm::vec2 p2 = parametric(t2);

        utils::draw_line(p1, p2, thickness, color);
    }
}

int main()
{
    gl::init("Courbe Paramétrique");
    gl::maximize_window();

    while (gl::window_is_open())
    {
        float dt = gl::delta_time_in_seconds();
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Exemple : un cercle centré à (0, 0) de rayon 0.5
        draw_parametric([](float t) {
            float angle = t * glm::two_pi<float>();
            return glm::vec2(0.5f * cos(angle), 0.5f * sin(angle));
        });

        // Tu peux tester aussi une courbe de Bézier :
        /*
        draw_parametric([](float t) {
            return utils::bezier3(
                {-0.5f, -0.5f},
                {-0.2f,  0.5f},
                gl::mouse_position(),
                { 0.5f,  0.5f},
                t
            );
        }, 100, 0.004f, glm::vec4(1.f, 0.f, 0.f, 1.f));
        */
    }

    return 0;
}
