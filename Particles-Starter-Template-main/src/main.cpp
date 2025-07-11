#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/color_space.hpp>
#include <algorithm>
#include <cmath>
#include <glm/gtx/norm.hpp>
#include <unordered_map>
#include <array>

// int main()
// {
//     gl::init("Particles!");
//     gl::maximize_window();
//     glEnable(GL_BLEND);
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE);

//     struct Particle
//     {
//         glm::vec2 position;
//         glm::vec2 velocity;
//         float mass;
//         float gravityForce = 0.f;
//         float lifetime;
//         float age = 0.f;

//         glm::vec4 startColor;
//         glm::vec4 endColor;

//         Particle()
//         {
//             position = glm::vec2(
//                 utils::rand(-gl::window_aspect_ratio(), gl::window_aspect_ratio()),
//                 utils::rand(-1.f, 1.f)
//             );

//             float angle = utils::rand(0.f, glm::two_pi<float>());
//             float speed = utils::rand(0.1f, 0.3f);
//             velocity = speed * glm::vec2(cos(angle), sin(angle));

//             mass = utils::rand(0.1f, 0.3f);
//             lifetime = utils::rand(2.f, 12.f);

//             startColor = glm::vec4(utils::rand(0.f, 1.f), utils::rand(0.f, 1.f), utils::rand(0.f, 1.f), 1.f);
//             endColor = glm::vec4(utils::rand(0.f, 1.f), utils::rand(0.f, 1.f), utils::rand(0.f, 1.f), 1.f);
//         }

//         void update(float dt, float k, float damping)
//         {
//             glm::vec2 gravity = glm::vec2(0.f, -gravityForce);
//             glm::vec2 dampingForce = -damping * velocity;

//             glm::vec2 force = mass * gravity + dampingForce;
//             glm::vec2 acceleration = force / mass;

//             velocity += acceleration * dt;
//             position += velocity * dt;

//             age += dt;
//         }

//         float current_radius() const
//         {
//             float x = glm::clamp((lifetime - age) / 2.f, 0.f, 1.f);
//             float bounce = std::abs(std::sin(10.f * x * glm::pi<float>()) * (1.f - x));
//             return 0.015f * x + 0.005f * bounce;
//         }

//         glm::vec4 current_color() const
//         {
//             float t = glm::clamp(age / lifetime, 0.f, 1.f);
//             float easedT = std::pow(t, 2.f);
//             return glm::mix(startColor, endColor, easedT);
//         }
//     };

//     std::vector<Particle> particles;

//     // Liste de cercles (centre, rayon)
//     std::vector<std::pair<glm::vec2, float>> circles = {
//         {{0.3f, 0.2f}, 0.2f},
//         {{-0.6f, -0.2f}, 0.15f},
//         {{0.0f, 0.7f}, 0.1f},
//         {{-0.2f, 0.3f}, 0.12f},
//         {{0.6f, -0.6f}, 0.18f}
//     };

//     // Liste de segments (start, end)
//     std::vector<std::pair<glm::vec2, glm::vec2>> static_segments = {
//         {{-0.7f, -0.8f}, {-0.3f, -0.2f}},
//         {{-0.1f, -0.7f}, { 0.2f, -0.2f}},
//         {{-0.9f,  0.0f}, {-0.3f,  0.6f}},
//         {{ 0.5f, -0.5f}, { 0.9f,  0.3f}},
//         {{ 0.0f,  0.4f}, { 0.5f,  0.9f}}
//     };

//     glm::vec2 static_start2 = {0.0f, -0.5f};

//     // Génération initiale de particules valides
//     while (particles.size() < 100)
//     {
//         Particle p;
//         bool inside_collider = false;

//         for (auto const& [center, radius] : circles)
//         {
//             if (glm::distance(p.position, center) < radius)
//             {
//                 inside_collider = true;
//                 break;
//             }
//         }

//         if (!inside_collider)
//             particles.push_back(p);
//     }

//     while (gl::window_is_open())
//     {
//         glm::vec4 lineColor = {1.f, 1.f, 1.f, 1.f};

//         float dt = gl::delta_time_in_seconds();
//         glClearColor(0.f, 0.f, 0.f, 1.f);
//         glClear(GL_COLOR_BUFFER_BIT);
//         float springStiffness = 0.5f;
//         float damping = 0.01f;

//         glm::vec2 mouse = gl::mouse_position();
//         glm::vec2 dynamic_start = glm::vec2(-mouse.x, -mouse.y);
//         glm::vec2 dynamic_end   = glm::vec2(+mouse.x, +mouse.y);

//         // Supprimer les particules mortes ou hors écran
//         particles.erase(
//             std::remove_if(particles.begin(), particles.end(), [](const Particle& p) {
//                 float aspect = gl::window_aspect_ratio();
//                 bool out = p.position.x < -aspect || p.position.x > aspect
//                         || p.position.y < -1.f || p.position.y > 1.f;
//                 return p.age > p.lifetime || out;
//             }),
//             particles.end()
//         );

//         // Re-remplir si certaines sont mortes
//         while (particles.size() < 100)
//         {
//             Particle p;
//             bool inside_collider = false;

//             for (auto const& [center, radius] : circles)
//             {
//                 if (glm::distance(p.position, center) < radius)
//                 {
//                     inside_collider = true;
//                     break;
//                 }
//             }

//             if (!inside_collider)
//                 particles.push_back(p);
//         }

//         for (Particle& p : particles)
//         {
//             glm::vec2 old_position = p.position;
//             p.update(dt, springStiffness, damping);
//             glm::vec2 new_position = p.position;

//             // Collisions cercles
//             for (auto const& [center, radius] : circles)
//             {
//                 auto hit = utils::segment_circle_intersection(old_position, new_position, center, radius);
//                 if (hit.has_value())
//                 {
//                     glm::vec2 normal = glm::normalize(hit.value() - center);
//                     glm::vec2 travel = new_position - old_position;
//                     float t_hit = glm::distance(old_position, hit.value()) / glm::length(travel);
//                     float remaining = 1.f - t_hit;

//                     p.velocity = glm::reflect(p.velocity, normal);
//                     p.position = hit.value() + remaining * p.velocity * dt;
//                     break;
//                 }
//             }

//             // Collisions segments
//             for (auto const& [start, end] : static_segments)
//             {
//                 auto intersection = utils::segment_intersection(start, end, old_position, new_position);
//                 if (intersection.has_value())
//                 {
//                     glm::vec2 wall = end - start;
//                     glm::vec2 normal = glm::normalize(glm::vec2(-wall.y, wall.x));
//                     glm::vec2 travel = new_position - old_position;
//                     float t_hit = glm::distance(old_position, intersection.value()) / glm::length(travel);
//                     float remaining = 1.f - t_hit;

//                     p.velocity = glm::reflect(p.velocity, normal);
//                     p.position = intersection.value() + remaining * p.velocity * dt;
//                     break;
//                 }
//             }

//             bool has_intersection = false;

//             // Intersection avec les segments statiques
//             for (auto const& [start, end] : static_segments)
//             {
//                 auto inter = utils::segment_intersection(start, end, static_start2, dynamic_end);
//                 if (inter.has_value())
//                 {
//                     has_intersection = true;
//                     utils::draw_disk(inter.value(), 0.03f, {1.f, 0.f, 0.f, 1.f});
//                 }
//             }

//             // Intersection avec les cercles
//             for (auto const& [center, radius] : circles)
//             {
//                 auto inter = utils::segment_circle_intersection(static_start2, dynamic_end, center, radius);
//                 if (inter.has_value())
//                 {
//                     has_intersection = true;
//                     utils::draw_disk(inter.value(), 0.03f, {1.f, 0.f, 0.f, 1.f});
//                 }
//             }

//             // Changer la couleur de la ligne si au moins une intersection
//             lineColor = has_intersection ? glm::vec4(1.f, 1.f, 0.f, 1.f): glm::vec4(1.f, 1.f, 1.f, 1.f);


//             utils::draw_disk(p.position, p.current_radius(), p.current_color());
//         }

//         for (auto const& [center, radius] : circles)
//             utils::draw_disk(center, radius, {1.f, 0.f, 1.f, 1.f});

//         for (auto const& [start, end] : static_segments)
//             utils::draw_line(start, end, 0.01f, {1.f, 1.f, 1.f, 1.f});

//         utils::draw_line(static_start2, dynamic_end, 0.01f, lineColor);
//     }
// }

int main()
{
    gl::init("Poisson Sampling Progressif");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    struct Particle {
        glm::vec2 position;
        float lifetime;
        float age = 0.f;
        float radius = 0.01f;

        Particle(glm::vec2 pos)
            : position(pos), lifetime(utils::rand(4.f, 8.f)) {}

        void update(float dt) { age += dt; }

        bool is_dead() const { return age > lifetime; }

        glm::vec4 color() const {
            float alpha = glm::clamp(1.f - age / lifetime, 0.f, 1.f);
            return glm::vec4(1.f, 1.f, 1.f, alpha);
        }
    };

    std::vector<Particle> particles;

    float r = 0.05f;
    float cell_size = r / std::sqrt(2.f);
    float aspect = gl::window_aspect_ratio();
    glm::vec2 min = {-aspect, -1.f};
    glm::vec2 max = { aspect,  1.f};
    glm::vec2 size = max - min;

    int grid_w = static_cast<int>(size.x / cell_size) + 1;
    int grid_h = static_cast<int>(size.y / cell_size) + 1;

    std::vector<std::vector<int>> grid(grid_w * grid_h);

    auto grid_index = [&](glm::vec2 p) -> glm::ivec2 {
        glm::vec2 rel = (p - min) / cell_size;
        return glm::ivec2(rel);
    };

    auto in_bounds = [&](glm::vec2 p) -> bool {
        return p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y;
    };

    auto is_valid = [&](glm::vec2 p) -> bool {
        glm::ivec2 gi = grid_index(p);
        int gx = gi.x, gy = gi.y;
        for (int y = gy - 2; y <= gy + 2; ++y) {
            for (int x = gx - 2; x <= gx + 2; ++x) {
                if (x < 0 || y < 0 || x >= grid_w || y >= grid_h) continue;
                for (int idx : grid[y * grid_w + x]) {
                    if (glm::distance(particles[idx].position, p) < r)
                        return false;
                }
            }
        }
        return true;
    };

    auto add_particle = [&]() {
        for (int tries = 0; tries < 30; ++tries) {
            float x = utils::rand(min.x, max.x);
            float y = utils::rand(min.y, max.y);
            glm::vec2 p = {x, y};
            if (is_valid(p)) {
                int new_idx = (int)particles.size();
                particles.emplace_back(p);
                glm::ivec2 gi = grid_index(p);
                grid[gi.y * grid_w + gi.x].push_back(new_idx);
                break;
            }
        }
    };

    while (gl::window_is_open())
    {
        float dt = gl::delta_time_in_seconds();
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Supprimer les particules mortes
        for (int i = (int)particles.size() - 1; i >= 0; --i) {
            if (particles[i].is_dead()) {
                glm::ivec2 gi = grid_index(particles[i].position);
                auto& cell = grid[gi.y * grid_w + gi.x];
                cell.erase(std::remove(cell.begin(), cell.end(), i), cell.end());

                // swap avec la dernière pour garder les indices valides
                if (i != particles.size() - 1) {
                    std::swap(particles[i], particles.back());

                    glm::ivec2 gi_old = grid_index(particles[i].position);
                    auto& cell_old = grid[gi_old.y * grid_w + gi_old.x];
                    for (int& idx : cell_old) {
                        if (idx == (int)particles.size() - 1) {
                            idx = i;
                            break;
                        }
                    }
                }

                particles.pop_back();
            }
        }

        // Essayer d’ajouter une particule par frame
        add_particle();

        // Mise à jour + rendu
        for (auto& p : particles) {
            p.update(dt);
            utils::draw_disk(p.position, p.radius, p.color());
        }
    }
}