#version 330

in vec2 fs_uv;
in vec4 fs_color;

out vec4 color;

uniform sampler2D tex;

void main()
{
    color = texture(tex, fs_uv) * fs_color;
}
