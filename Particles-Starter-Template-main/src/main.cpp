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

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    float radius = 0.008f;

    Particle(glm::vec2 pos) : position(pos), velocity(0.f) {}

    void update(float dt) {
        position += velocity * dt;
    }

    glm::vec4 color() const {
        return glm::vec4(1.f);
    }
};

std::vector<Particle> particles;

void spawn_rain_particles(float aspect_ratio) {
    for (int i = 0; i < 5; ++i) {
        float x = utils::rand(-aspect_ratio, aspect_ratio);
        float y = 1.1f; // juste au-dessus de la fenêtre
        particles.emplace_back(glm::vec2(x, y));
    }
}

int main() {
    gl::init("Champ de force autour d'une courbe de Bézier");
    gl::maximize_window();

    while (gl::window_is_open()) {
        float dt = gl::delta_time_in_seconds();
        float aspect = gl::framebuffer_aspect_ratio();
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Bézier cubique dynamique
        glm::vec2 p0 = {-0.5f, -0.5f};
        glm::vec2 p1 = {-0.2f,  0.5f};
        glm::vec2 p2 = gl::mouse_position(); // interactif
        glm::vec2 p3 = { 0.5f,  0.5f};

        // Affichage de la courbe
        draw_parametric([&](float t) {
            return utils::bezier3_bernstein(p0, p1, p2, p3, t);
        }, 100, 0.005f, glm::vec4(1.f, 0.f, 0.f, 1.f));

        // Spawner une pluie de particules
        spawn_rain_particles(aspect);

        // Mettre à jour les particules
        for (auto& p : particles) {
            // Trouver le t correspondant au point le plus proche sur la courbe
            float t_closest = utils::find_closest_t_on_bezier(p.position, p0, p1, p2, p3);
            glm::vec2 curve_point = utils::bezier3_bernstein(p0, p1, p2, p3, t_closest);
            glm::vec2 tangent = utils::bezier3_derivative(p0, p1, p2, p3, t_closest);
            glm::vec2 normal = glm::normalize(glm::vec2(-tangent.y, tangent.x));

            glm::vec2 delta = p.position - curve_point;
            float distance = glm::length(delta);

            // Force de répulsion décroissante (type gaussien)
            float strength = 5.f * std::exp(-distance * 10.f);
            p.velocity += normal * strength * dt;

            // Gravité simple vers le bas
            p.velocity += glm::vec2(0.f, -0.4f) * dt;

            // Update position
            p.update(dt);

            // Affichage
            utils::draw_disk(p.position, p.radius, p.color());
        }

        // Nettoyage des particules hors écran
        particles.erase(std::remove_if(particles.begin(), particles.end(), [](auto const& p) {
            return p.position.y < -1.2f;
        }), particles.end());
    }

    return 0;
}