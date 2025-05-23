#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/color_space.hpp>
#include <algorithm>
#include <cmath>

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    struct Particle
    {
        glm::vec2 position;
        glm::vec2 velocity;
        float mass;
        float gravityForce = 0.f;
        float lifetime;
        float age = 0.f;

        glm::vec4 startColor;
        glm::vec4 endColor;

        Particle()
        {
            position = glm::vec2(
                utils::rand(-gl::window_aspect_ratio(), gl::window_aspect_ratio()),
                utils::rand(-1.f, 1.f)
            );

            float angle = utils::rand(0.f, glm::two_pi<float>());
            float speed = utils::rand(0.1f, 0.3f);
            velocity = speed * glm::vec2(cos(angle), sin(angle));

            mass = utils::rand(0.1f, 0.3f);
            lifetime = utils::rand(2.f, 12.f);

            startColor = glm::vec4(utils::rand(0.f, 1.f), utils::rand(0.f, 1.f), utils::rand(0.f, 1.f), 1.f);
            endColor = glm::vec4(utils::rand(0.f, 1.f), utils::rand(0.f, 1.f), utils::rand(0.f, 1.f), 1.f);
        }

        void update(float dt, /*const glm::vec2& target,*/ float k, float damping)
        {
            glm::vec2 gravity = glm::vec2(0.f, -gravityForce);
            //glm::vec2 springForce = -k * (position - target);
            glm::vec2 dampingForce = -damping * velocity;

            glm::vec2 force = mass * gravity + /*springForce +*/ dampingForce;
            glm::vec2 acceleration = force / mass;

            velocity += acceleration * dt;
            position += velocity * dt;

            age += dt;
        }

        float current_radius() const
        {
            float x = glm::clamp((lifetime - age) / 2.f, 0.f, 1.f);
            float bounce = std::abs(std::sin(10.f * x * glm::pi<float>()) * (1.f - x));
            return 0.015f * x + 0.005f * bounce;
        }

        glm::vec4 current_color() const
        {
            float t = glm::clamp(age / lifetime, 0.f, 1.f);
            float easedT = std::pow(t, 2.f);
            return glm::mix(startColor, endColor, easedT);
        }

    };

    std::vector<Particle> particles;
    for (int i = 0; i < 100; ++i)
    {
        particles.emplace_back();
    }

    while (gl::window_is_open())
    {
        float dt = gl::delta_time_in_seconds();

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::vec2 mouse = gl::mouse_position();
        float springStiffness = 0.5f;
        float damping = 0.1f;

        // Supprimer les particules mortes
        particles.erase(
            std::remove_if(particles.begin(), particles.end(), [](const Particle& p) {
                return p.age > p.lifetime;
            }),
            particles.end()
        );

        for (Particle& p : particles)
        {
            p.update(dt, /*mouse,*/ springStiffness, damping);
            utils::draw_disk(p.position, p.current_radius(), p.current_color());
        }
    }
}
