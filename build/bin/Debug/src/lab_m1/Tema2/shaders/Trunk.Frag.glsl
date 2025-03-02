#version 330

flat in vec2 vertex_position;
in vec3 vertex_normal;

// 2D Random
float random(in vec2 st) 
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main() {
    vec3 color_dark = vec3(0.31, 0.192, 0);
    vec3 color_light = vec3(0.722, 0.443, 0);

    vec3 color = mix(color_light, color_dark, random(vertex_position) * 0.5 + 0.5);
    gl_FragColor = vec4(color, 1.0);
}
