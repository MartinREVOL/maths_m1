#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <functional>
#include <vector>

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

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    float radius = 0.01f;

    Particle(glm::vec2 pos, glm::vec2 vel)
        : position(pos), velocity(vel) {}
};


std::vector<Particle> particles;

void spawn_particles_on_bezier() {
    particles.clear();

    glm::vec2 p0 = {-0.5f, -0.5f};
    glm::vec2 p1 = {-0.2f,  0.5f};
    glm::vec2 p2 = gl::mouse_position();
    glm::vec2 p3 = { 0.5f,  0.5f};

    int count = 50;
    for (int i = 0; i < count; ++i) {
        float t = static_cast<float>(i) / (count - 1);
        glm::vec2 pos = utils::bezier3_bernstein(p0, p1, p2, p3, t);
        glm::vec2 tangent = utils::bezier3_derivative(p0, p1, p2, p3, t);

        // Calcul de la normale (perpendiculaire à la tangente)
        glm::vec2 normal = glm::normalize(glm::vec2(-tangent.y, tangent.x));

        // Vitesse dans la direction de la normale, amplitude arbitraire
        float speed = 0.2f;
        glm::vec2 velocity = normal * speed;

        particles.emplace_back(pos, velocity);
    }
}



int main() {
    gl::init("50 Particules sur une courbe de Bézier");
    gl::maximize_window();

    // Spawner une fois au lancement
    spawn_particles_on_bezier();

    while (gl::window_is_open()) {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Courbe affichée en rouge
        glm::vec2 p0 = {-0.5f, -0.5f};
        glm::vec2 p1 = {-0.2f,  0.5f};
        glm::vec2 p2 = gl::mouse_position();
        glm::vec2 p3 = { 0.5f,  0.5f};

        draw_parametric([&](float t) {
            return utils::bezier3_bernstein(p0, p1, p2, p3, t);
        }, 100, 0.005f, glm::vec4(1.f, 0.f, 0.f, 1.f));

        for (auto& p : particles) {
            p.position += p.velocity * gl::delta_time_in_seconds();
            utils::draw_disk(p.position, p.radius, glm::vec4(1.f));
        }

    }

    return 0;
}
