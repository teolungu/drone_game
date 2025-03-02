#version 330

// 2D Random
float random(in vec2 st) 
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
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
    vec2 u = smoothstep(0.0, 1.0, f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a) * u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

in vec2 vertex_position;
flat in vec2 vertex_position_flat;

uniform float time_elapsed;
uniform vec2 dest_position;
uniform float dest_radius;

void main() {
    vec3 color = vec3(0, 0, 0);
    float dist = distance(vertex_position, dest_position);
    if (dist < dest_radius)
    {
        vec3 color0 = vec3(1, 0.706, 0);
        vec3 color1 = vec3(0.639, 0, 0.576);
        
        float t = dist / dest_radius;

        float t_pulse = sin(time_elapsed * 3.5) * 0.5 + 0.5;
        vec3 color_pulse = mix(color0, color1, t_pulse);

        if (t < t_pulse)
            color = mix(color0, color_pulse, t);
        else
            color = mix(color_pulse, color1, t);
    }
    else
    {
        vec3 color_dark = vec3(0.024, 0.412, 0.004);
        vec3 color_light = vec3(0.043, 0.878, 0);

        float t1 = noise(vertex_position_flat * 2) * 0.5 + 0.5;
        float t2 = noise(vertex_position_flat * 0.3) * 0.5 + 0.5;
        color = mix(color_dark, color_light, t1 * 0.3 + t2 * 0.7);
    }

    gl_FragColor = vec4(color, 1.0);
}
