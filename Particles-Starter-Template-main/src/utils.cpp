#include "utils.hpp"
#include <random>
#include "opengl-framework/opengl-framework.hpp"

namespace utils {

static auto& generator()
{
    thread_local std::default_random_engine gen{std::random_device{}()};
    return gen;
}

float rand(float min, float max)
{
    return std::uniform_real_distribution<float>{min, max}(generator());
}

static auto make_square_mesh() -> gl::Mesh
{
    return gl::Mesh{gl::Mesh_Descriptor{
        .vertex_buffers = {
            gl::VertexBuffer_Descriptor{
                .layout = {gl::VertexAttribute::Position2D(0), gl::VertexAttribute::UV(1)},
                .data   = {
                    -1.f, -1.f, 0.f, 0.f,
                    +1.f, -1.f, 1.f, 0.f,
                    +1.f, +1.f, 1.f, 1.f,
                    -1.f, +1.f, 0.f, 1.f
                }
            }
        },
        .index_buffer = {0, 1, 2, 0, 2, 3},
    }};
}

static auto make_disk_shader() -> gl::Shader
{
    return gl::Shader{
        gl::Shader_Descriptor{
            .vertex = gl::ShaderSource::Code({R"GLSL(
#version 410

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_uv;

uniform vec2 u_position;
uniform float u_radius;
uniform float u_inverse_aspect_ratio;

out vec2 v_uv;

void main()
{
    vec2 position = u_position + u_radius * in_position;
    gl_Position = vec4(position * vec2(u_inverse_aspect_ratio, 1.), 0., 1.);
    v_uv = in_uv;
}
)GLSL"}),
            .fragment = gl::ShaderSource::Code({R"GLSL(
#version 410

out vec4 out_color;
in vec2 v_uv;
uniform vec4 u_color;

void main()
{
    vec2 dir = v_uv - vec2(0.5);
    if (dot(dir, dir) > 0.25)
        discard;
    out_color = u_color;
}
)GLSL"}),
        }
    };
}

void draw_disk(glm::vec2 position, float radius, glm::vec4 const& color)
{
    static auto square_mesh = make_square_mesh();
    static auto disk_shader = make_disk_shader();

    disk_shader.bind();
    disk_shader.set_uniform("u_position", position);
    disk_shader.set_uniform("u_radius", radius);
    disk_shader.set_uniform("u_inverse_aspect_ratio", 1.f / gl::framebuffer_aspect_ratio());
    disk_shader.set_uniform("u_color", color);
    square_mesh.draw();
}

static auto make_line_shader() -> gl::Shader
{
    return gl::Shader{
        gl::Shader_Descriptor{
            .vertex = gl::ShaderSource::Code({R"GLSL(
#version 410

uniform vec2 u_start;
uniform vec2 u_end;
uniform float u_thickness;
uniform float u_inverse_aspect_ratio;

const vec2 quadOffsets[4] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0,  1.0)
);

void main() {
    vec2 dir = normalize(u_end - u_start);
    vec2 normal = vec2(-dir.y, dir.x);
    vec2 middle = (u_start + u_end) * 0.5;
    vec2 pos = middle
             + quadOffsets[gl_VertexID].x * (u_end - u_start) * 0.5
             + quadOffsets[gl_VertexID].y * normal * u_thickness * 0.5;
    gl_Position = vec4(pos * vec2(u_inverse_aspect_ratio, 1.), 0., 1.);
}
)GLSL"}),
            .fragment = gl::ShaderSource::Code({R"GLSL(
#version 410

out vec4 out_color;
uniform vec4 u_color;

void main()
{
    out_color = u_color;
}
)GLSL"}),
        }
    };
}

void draw_line(glm::vec2 start, glm::vec2 end, float thickness, glm::vec4 const& color)
{
    static auto line_mesh = make_square_mesh();
    static auto line_shader = make_line_shader();
    line_shader.bind();
    line_shader.set_uniform("u_start", start);
    line_shader.set_uniform("u_end", end);
    line_shader.set_uniform("u_thickness", thickness);
    line_shader.set_uniform("u_inverse_aspect_ratio", 1.f / gl::framebuffer_aspect_ratio());
    line_shader.set_uniform("u_color", color);
    line_mesh.draw();
}

std::optional<glm::vec2> segment_intersection(
    glm::vec2 p1, glm::vec2 p2,
    glm::vec2 q1, glm::vec2 q2)
{
    glm::vec2 r = p2 - p1;
    glm::vec2 s = q2 - q1;

    float rxs = r.x * s.y - r.y * s.x;
    glm::vec2 qp = q1 - p1;
    float qpxr = qp.x * r.y - qp.y * r.x;

    // Si rxs == 0, les segments sont colinéaires ou parallèles (on ignore ce cas comme demandé)
    if (std::abs(rxs) < 1e-8f) return std::nullopt;

    float t = (qp.x * s.y - qp.y * s.x) / rxs;
    float u = qpxr / rxs;

    if (t >= 0.f && t <= 1.f && u >= 0.f && u <= 1.f)
        return p1 + t * r;

    return std::nullopt;
}

std::optional<glm::vec2> segment_circle_intersection(glm::vec2 p1, glm::vec2 p2, glm::vec2 center, float radius)
{
    glm::vec2 d = p2 - p1;
    glm::vec2 f = p1 - center;

    float a = glm::dot(d, d);
    float b = 2 * glm::dot(f, d);
    float c = glm::dot(f, f) - radius * radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return std::nullopt; // pas d'intersection

    discriminant = std::sqrt(discriminant);

    float t1 = (-b - discriminant) / (2 * a);
    float t2 = (-b + discriminant) / (2 * a);

    if (t1 >= 0.f && t1 <= 1.f)
        return p1 + t1 * d;
    else if (t2 >= 0.f && t2 <= 1.f)
        return p1 + t2 * d;

    return std::nullopt;
}

int rand_int(int min, int max)
{
    return static_cast<int>(std::floor(utils::rand(static_cast<float>(min), static_cast<float>(max) + 1.f)));
}

// Interpolation linéaire
glm::vec2 lerp(glm::vec2 a, glm::vec2 b, float t) {
    return (1 - t) * a + t * b;
}

// Bézier d'ordre 1 : segment
glm::vec2 bezier1(glm::vec2 p0, glm::vec2 p1, float t) {
    return lerp(p0, p1, t);
}

// Bézier d'ordre 2 : quadratique
glm::vec2 bezier2(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float t) {
    glm::vec2 a = lerp(p0, p1, t);
    glm::vec2 b = lerp(p1, p2, t);
    return lerp(a, b, t);
}

// Bézier d'ordre 3 : cubique
glm::vec2 bezier3(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t) {
    glm::vec2 a = lerp(p0, p1, t);
    glm::vec2 b = lerp(p1, p2, t);
    glm::vec2 c = lerp(p2, p3, t);

    glm::vec2 d = lerp(a, b, t);
    glm::vec2 e = lerp(b, c, t);

    return lerp(d, e, t);
}

static int binomial_coeff(int n, int k) {
    if (k == 0 || k == n) return 1;
    int res = 1;
    for (int i = 1; i <= k; ++i) {
        res *= (n - (k - i));
        res /= i;
    }
    return res;
}

// Bézier ordre 1 (segment)
glm::vec2 bezier1_bernstein(glm::vec2 p0, glm::vec2 p1, float t) {
    return (1 - t) * p0 + t * p1; // même chose que De Casteljau
}

// Bézier ordre 2 (quadratique)
glm::vec2 bezier2_bernstein(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float t) {
    float u = 1 - t;
    return u * u * p0 + 2 * u * t * p1 + t * t * p2;
}

// Bézier ordre 3 (cubique)
glm::vec2 bezier3_bernstein(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t) {
    float u = 1 - t;
    return u * u * u * p0
         + 3 * u * u * t * p1
         + 3 * u * t * t * p2
         + t * t * t * p3;
}

glm::vec2 bezier3_derivative(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t) {
    float u = 1.f - t;
    return 3.f * u * u * (p1 - p0)
         + 6.f * u * t * (p2 - p1)
         + 3.f * t * t * (p3 - p2);
}



} // namespace utils
