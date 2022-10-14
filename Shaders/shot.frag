//Standard for fragment and vertex shaders
#version 330 core

//The color of the vertex
out vec4 FragColor;

//Changes color in RGBA format (0-1)
void main()
{
	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}