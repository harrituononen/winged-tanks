#version 330

in vec2 fs_uv;

out vec4 out_color;

uniform sampler2D tex;
uniform vec4 color;

void main()
{
    out_color = vec4(color.xyz, texture(tex, fs_uv).r - 1.0 + color.w);
}
