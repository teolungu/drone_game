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

flat out vec2 vertex_position;
out vec3 vertex_normal;

void main()
{
    vertex_position = vec2(v_position.x, v_position.z);
	vertex_normal = v_normal;

	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
