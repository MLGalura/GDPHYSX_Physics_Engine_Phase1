//Standard for fragment and vertex shaders
#version 330 core

//Calls OpenGL to assign the vector 3 to a position "aPos"
layout(location = 0) in vec3 aPos;

//Assignment of Transformation Matrix
uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;

//Creates a single column matrix in the current position
void main()
{
	gl_Position = projection * view * transform * vec4(aPos, 1);
}