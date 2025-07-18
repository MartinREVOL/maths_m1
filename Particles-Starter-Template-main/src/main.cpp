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

// Courbe de cœur : t ∈ [0, 1]
glm::vec2 heart_curve(float t) {
    float angle = t * glm::two_pi<float>(); // t ∈ [0, 1] → angle ∈ [0, 2π]
    float x = 16.f * std::pow(std::sin(angle), 3.f);
    float y = 13.f * std::cos(angle) - 5.f * std::cos(2.f * angle) - 2.f * std::cos(3.f * angle) - std::cos(4.f * angle);
    return glm::vec2(x, y) / 18.f; // Normalisation dans [-1,1]
}

// Approximation de la tangente (dérivée numérique centrée)
glm::vec2 heart_tangent(float t, float epsilon = 0.001f) {
    glm::vec2 p1 = heart_curve(glm::clamp(t - epsilon, 0.f, 1.f));
    glm::vec2 p2 = heart_curve(glm::clamp(t + epsilon, 0.f, 1.f));
    return glm::normalize(p2 - p1);
}

// Descente de gradient pour trouver le t de la courbe du cœur le plus proche d’un point donné
float find_closest_t_on_heart(glm::vec2 point) {
    float t = 0.5f;
    float lr = 0.01f;
    int max_iter = 100;

    for (int i = 0; i < max_iter; ++i) {
        glm::vec2 curve_pos = heart_curve(t);
        glm::vec2 tangent = heart_tangent(t);
        glm::vec2 diff = curve_pos - point;

        // Gradient de ||C(t) - p||² = 2 * dot(diff, tangent)
        float grad = 2.f * glm::dot(diff, tangent);
        t -= lr * grad;
        t = glm::clamp(t, 0.f, 1.f);
        if (std::abs(grad) < 1e-5f) break;
    }

    return t;
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
        float y = 1.1f;
        particles.emplace_back(glm::vec2(x, y));
    }
}

void draw_heart_outline() {
    draw_parametric(heart_curve, 300, 0.005f, glm::vec4(1.f, 0.f, 0.f, 1.f));
}

int main() {
    gl::init("Champ de force autour d'un cœur");
    gl::maximize_window();

    while (gl::window_is_open()) {
        float dt = gl::delta_time_in_seconds();
        float aspect = gl::framebuffer_aspect_ratio();

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_heart_outline();
        spawn_rain_particles(aspect);

        for (auto& p : particles) {
            float t_closest = find_closest_t_on_heart(p.position);
            glm::vec2 curve_point = heart_curve(t_closest);
            glm::vec2 tangent = heart_tangent(t_closest);
            glm::vec2 normal = glm::normalize(glm::vec2(-tangent.y, tangent.x));

            glm::vec2 delta = p.position - curve_point;
            float distance = glm::length(delta);

            float strength = 5.f * std::exp(-distance * 10.f);
            p.velocity += normal * strength * dt;

            p.velocity += glm::vec2(0.f, -0.4f) * dt; // gravité
            p.update(dt);

            utils::draw_disk(p.position, p.radius, p.color());
        }

        particles.erase(std::remove_if(particles.begin(), particles.end(), [](auto const& p) {
            return p.position.y < -1.2f;
        }), particles.end());
    }

    return 0;
}