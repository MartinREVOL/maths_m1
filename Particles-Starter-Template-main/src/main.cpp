#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/constants.hpp>
#include <cmath>

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Structure de particule avec position aléatoire
    struct Particle
    {
        glm::vec2 position;
        glm::vec2 velocity;
        float mass;
        float gravityForce = 1.f;

        Particle()
        {
            position = glm::vec2(
                utils::rand(-gl::window_aspect_ratio(), gl::window_aspect_ratio()),
                utils::rand(-1.f, 1.f)
            );

            // Générer angle et vitesse aléatoires
            float angle = utils::rand(0.f, glm::two_pi<float>()); // 0 à 2π
            float speed = utils::rand(0.2f, 0.5f); // vitesses entre 0.2 et 0.5 unités/sec

            velocity = speed * glm::vec2(cos(angle), sin(angle));

            mass = utils::rand(0.1f, 0.3f);
        }

        void update(float dt, const glm::vec2& target, float k)
        {
            glm::vec2 gravity = glm::vec2(0.f, -gravityForce);
            glm::vec2 springForce = -k * (position - target);

            glm::vec2 force = mass * gravity + springForce;
            glm::vec2 acceleration = force / mass;

            velocity += acceleration * dt;
            position += velocity * dt;
        }

    };

    // Création d'un tableau de particules
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

        for (Particle& p : particles)
        {
            p.update(dt, mouse, springStiffness);
            utils::draw_disk(p.position, 0.01f, glm::vec4(1.f, 1.f, 1.f, 1.f));
        }
    }
}
