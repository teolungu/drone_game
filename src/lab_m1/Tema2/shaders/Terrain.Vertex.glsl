#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// 2D Random
float random(in vec2 st) 
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float lerp(float a, float b, float f) 
{
	return a + f * (b - a);
}

float mix2(float x00, float x10, float x01, float x11, vec2 uv)
{
    float u = clamp(uv.x, 0.0, 1.0);
    float v = clamp(uv.y, 0.0, 1.0);

    float h0 = lerp(x00, x10, u);
    float h1 = lerp(x01, x11, u);
    return lerp(h0, h1, v);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise(in vec2 st) 
{
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 uv = smoothstep(0.0, 1.0, f);

    // Mix 4 coorners percentages
    return mix2(a, b, c, d, uv);
}

// Output to frag shader
out vec2 vertex_position;
flat out vec2 vertex_position_flat;
uniform float noise_multiplier;
uniform float height_multiplier;

void main()
{
    vertex_position = vec2(v_position.x, v_position.z);
    vertex_position_flat = vec2(v_position.x, v_position.z);

    float height = noise(vertex_position * noise_multiplier) * height_multiplier;
    vec3 position = v_position + vec3(0, height, 0);

	gl_Position = Projection * View * Model * vec4(position, 1.0);
}
