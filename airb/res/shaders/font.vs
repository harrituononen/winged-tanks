#version 330

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

out vec2 fs_uv;

uniform mat4 projection;
uniform mat3 transform;
uniform mat3 scale;

void main()
{
    gl_Position = projection * vec4(scale * transform * vec3(position, 1.0), 1.0);
    fs_uv = uv;
}
