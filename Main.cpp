/************************************************************
This code was written for the completion of the requirement:
GDPHYSX XX22 - Physics Engine Phase 1.

NAME: GALURA, MARC LAWRENCE C.
SECTION: XX22
DATE: OCTOBER 8, 2022
*************************************************************/

//Headers used for the program
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

//Defines the type for the output window screen
GLFWwindow* window;

//Variables for the ouput window size
float width = 800.0f;
float height = 800.0f;

//Integer variable to identify the type and state of the projectile
int projectileType;
int projectileState = 0;

//Variable for the fuse timer of the firework
float fuse;

//Creates an Identity matrix
glm::mat4 identity = glm::mat4(1.0f);

//Creates the perspective projection matrix
glm::mat4 projection = glm::perspective(glm::radians(60.0f), height / width, 0.1f, 1000.0f);

//Camera position and WorldUp value for camera orientation
glm::vec3 cameraPos = glm::vec3(-3.5f, 5, -15.0f);
glm::vec3 WorldUp = glm::vec3(0, 1, 0);
glm::vec3 centerPos = glm::vec3(0.0f, 3.0f, 0.0f);

//Front, Right, and UP values of the camera
glm::vec3 F = glm::normalize(centerPos - cameraPos);
glm::vec3 R = glm::normalize(glm::cross(F, WorldUp));
glm::vec3 U = glm::normalize(glm::cross(R, F));

//View matrix for the camera and user POV
glm::mat4 view = glm::lookAt(cameraPos, cameraPos + F, WorldUp);

//Function to randomly generate float values lesser than 0.005f
float floatRNG()
{
	//Sets the number to 1.0f so the while loop will always be triggered to generate a new value below 0.005f
	float randomnum = 1.0f;

	//While loop to generate a value below 0.01f to simulate
	while (randomnum >= 0.01f)
	{
		randomnum = float(rand()) / RAND_MAX;
	}

	return randomnum;
}

//Function to randomly generate integer values
int intRNG()
{
	int randomNum;

	//Retrieves a random integer lesser than 11 but greater than 0
	randomNum = rand() % 11;

	return randomNum;
}

//Class that holds the variables for the particles
class Particle
{
public:
	//Variables for the particle damping
	float damping;

	//Unsigned int variable to determine how many isntances of the object will occur
	unsigned int instances;

	//Variables for the particle position, velocity, and acceleration
	glm::vec3 position = glm::vec3(0.0f, 1.5f, -10.0f);
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::mat4 transform;

	//Variables that hold the relative path to the mesh and attributes to the object
	std::string path;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning, error;
	tinyobj::attrib_t attributes;

	std::vector<GLuint> mesh_indices;

	//Variables used for initializing shaders and buffers
	std::stringstream vertStrStream;
	std::stringstream fragStrStream;

	//Variables for the shader program and buffers
	GLuint shaderProgram;
	GLuint VAO, VBO, EBO;

	//Function that loads the object
	void objLoad()
	{
		//Iterate through all of the vertices and push the indices to the vector
		bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str());

		for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
		{
			mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
		}
	}

	//Function that initializes the shaders for the object
	void initShader()
	{	
		std::string vertStr = vertStrStream.str();
		std::string fragStr = fragStrStream.str();

		const char* vertSrc = vertStr.c_str();
		const char* fragSrc = fragStr.c_str();

		GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertShader, 1, &vertSrc, NULL);
		glShaderSource(fragShader, 1, &fragSrc, NULL);
		glCompileShader(vertShader);
		glCompileShader(fragShader);

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertShader);
		glAttachShader(shaderProgram, fragShader);

		glLinkProgram(shaderProgram);
	}

	//Function that initializes the buffers for the object
	void initBuff()
	{
		//Generates the VAO, VBO, and EBO buffers
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		//Binds buffers to the VAO and VBO
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData
		(
			GL_ARRAY_BUFFER,
			sizeof(tinyobj::real_t) * attributes.vertices.size(),
			attributes.vertices.data(),
			GL_DYNAMIC_DRAW
		);

		//Binds buffer to EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData
		(
			GL_ELEMENT_ARRAY_BUFFER,
			sizeof(GLuint) * mesh_indices.size(),
			mesh_indices.data(),
			GL_DYNAMIC_DRAW
		);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	//Function that draws the object
	void drawObj()
	{
		//applying the matrices to the shader program
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		//Draws the object
		glDrawElements	
		(
			GL_TRIANGLES,
			mesh_indices.size(),
			GL_UNSIGNED_INT,
			0
		);
	}
};

//Firework subclass that holds the variables and functions to simulate a firework explosion
class Firework : public Particle
{
public:
	//Array for velocity values of the firework explosions
	glm::vec3 velocities[11];

	//Function to create random velocity values to each index in the array
	void randomVelocity()
	{
		for (int i = 0; i < intRNG(); i++)
		{
			velocities[i] = glm::vec3(floatRNG(), floatRNG(), floatRNG());
		}
	}
};

//Initializes shot from the particle class
Particle shot;
Firework spark;

//Ensures that GLFW registers the space key as one input instead of querying every frame
void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Allows the user to exit the program with the escape key
	if (key == GLFW_KEY_ESCAPE && action == 1)
		glfwSetWindowShouldClose(window, true);

	//Resets velocity and position values when pressed, applies respective damping, velocity, and acceleration
	if (key == GLFW_KEY_1 && action == 1) //Bullet
	{
		projectileType = 1;
		projectileState = 0;
		shot.position = glm::vec3(0.0f, 1.5f, -10.0f);

		shot.damping = 0.99f;
		shot.velocity = glm::vec3(0.0f, 0.0f, 0.035f);
		shot.acceleration = glm::vec3(0.0f, -0.005f, 0.0f);
	}

	if (key == GLFW_KEY_2 && action == 1) //Artillery
	{
		projectileType = 2;
		projectileState = 0;
		shot.position = glm::vec3(0.0f, 1.5f, -10.0f);

		shot.damping = 0.99f;
		shot.velocity = glm::vec3(0.0f, 0.030f, 0.040f);
		shot.acceleration = glm::vec3(0.0f, -0.020f, 0.0f);
	}

	if (key == GLFW_KEY_3 && action == 1) //Fireball
	{
		projectileType = 3;
		projectileState = 0;
		shot.position = glm::vec3(0.0f, 1.5f, -10.0f);

		shot.damping = 0.9f;
		shot.velocity = glm::vec3(0.0f, 0.0f, 0.01f);
		shot.acceleration = glm::vec3(0.0f, 0.006f, 0.0f);
	}

	if (key == GLFW_KEY_4 && action == 1) //Laser
	{
		projectileType = 4;
		projectileState = 0;
		shot.position = glm::vec3(0.0f, 1.5f, -10.0f);

		shot.damping = 0.99f;
		shot.velocity = glm::vec3(0.0f, 0.0f, 0.1f);
		shot.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	if (key == GLFW_KEY_5 && action == 1) //Firework
	{
		projectileType = 5;
		projectileState = 0;
		shot.position = glm::vec3(0.0f, 1.5f, -10.0f);

		shot.damping = 0.99f;
		shot.velocity = glm::vec3(0.0f, 0.0f, 0.05f);
		shot.acceleration = glm::vec3(0.0f, 0.0f, -0.002f);

		spark.randomVelocity();
		spark.acceleration = glm::vec3(0.0f, -0.02f, 0.0f);
	}

	//Activates the projectile, resets the position when pressed again, resets the time value, resets the fuse timer
	if (key == GLFW_KEY_SPACE && action == 1)
	{
		projectileState = 1;
		shot.position = glm::vec3(0.0f, 1.5f, -10.0f);

		fuse = float(rand()) / RAND_MAX + 0.5f;
	}
}

//Main code of the program
int main(void)
{
	//Determines if the glfw library has been initialized
	if (!glfwInit())
		return -1;

	//Creates a windowed mode window and its OpenGL context
	window = glfwCreateWindow(width, height, "PHYSICS ENGINE PHASE#1-GALURA_MARC", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//Sets the current window's context to current
	glfwMakeContextCurrent(window);
	gladLoadGL();

	//Sets the path for the respective obj file and calls the objLoad() function
	shot.path = "3D/Ball.obj";
	spark.path = "3D/Ball.obj";
	shot.objLoad();
	spark.objLoad();

	//Initializes the vertex and fragment shaders that will be used for the 3D object selected
	std::fstream vertstr("Shaders/shot.vert");
	std::fstream fragstr("Shaders/shot.frag");

	shot.vertStrStream << vertstr.rdbuf();
	shot.fragStrStream << fragstr.rdbuf();
	spark.vertStrStream << vertstr.rdbuf();
	spark.fragStrStream << fragstr.rdbuf();

	shot.initShader();
	shot.initBuff();
	spark.initShader();
	spark.initBuff();

	//Retrieves the time
	float lastTime = glfwGetTime();

	//Loops until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		//Rendering of the object
		glClear(GL_COLOR_BUFFER_BIT);

		//Retrieves the time 
		float currTime = glfwGetTime();
		float deltaTime = currTime - lastTime;

		//Calles the function that allows for a singular input of the space key
		glfwSetKeyCallback(window, input_callback);

		//Manipulates the size and position of the object
		shot.transform = glm::translate(identity, shot.position);
		shot.transform = glm::scale(shot.transform, glm::vec3(0.2f));
		spark.transform = glm::translate(identity, spark.position);
		spark.transform = glm::scale(spark.transform, glm::vec3(0.1f));

		//Resets the time when the projectile is fired again
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			lastTime = currTime;

		//Identifies if the cue for projectile activation is detected, shoots the projectile with the selected mode
		if (projectileState == 1)
		{
			shot.position += ((shot.velocity * shot.damping) + (shot.acceleration * deltaTime)) * deltaTime;
			spark.position += ((spark.velocity * spark.damping) + (spark.acceleration * deltaTime)) * deltaTime;
		}

		//Draws only the projectile when the type is from 1-4
		if (projectileType >= 1 && projectileType <= 4)
		{
			//Uses the shader program and draws the vertices of the object
			glUseProgram(shot.shaderProgram);
			glBindVertexArray(shot.VAO);

			//Calls the function to draw the object
			shot.drawObj();

			//Repeats the object animation if it goes out of bounds
			if (shot.position.y <= -5.0f || shot.position.z >= 200)
			{
				shot.position = glm::vec3(0.0f, 1.5f, -10.0f);
				lastTime = currTime;
			}
		}

		//Draws the projectile and the firework sparks after explosion
		if (projectileType == 5)
		{
			glUseProgram(spark.shaderProgram);
			glBindVertexArray(spark.VAO);

			//Shot will only appear before the fuse timer is passed
			if (deltaTime <= fuse)
				shot.drawObj();

			//Sets the initial position of the sparks to the last position of the fuse before despawn
			else if (deltaTime == fuse)
				spark.position = shot.position;

			//Sets different velocities 
			else if (deltaTime > fuse)
			{
				for (int k = 0; k < intRNG(); k++)
				{
					spark.velocity = spark.velocities[k];
					spark.drawObj();
				}
			}
		}

		//Swaps the front and back buffers
		glfwSwapBuffers(window);

		//Poll for and process events
		glfwPollEvents();
	}

	//Resets values after use
	glDeleteVertexArrays(1, &shot.VAO);
	glDeleteBuffers(1, &shot.VBO);
	glDeleteBuffers(1, &shot.EBO);

	glDeleteVertexArrays(1, &spark.VAO);
	glDeleteBuffers(1, &spark.VBO);
	glDeleteBuffers(1, &spark.EBO);

	//Ends the program
	glfwTerminate();
	return 0;
}
/***********************************************************************************************************************

This is to certify that this project is my own work, based on my personal efforts in studying and applying the concepts
learned. I have constructed the functions and their respective algorithms and corresponding code by myself. The program
was run, tested, and debugged by my own efforts.  I further certify that I have not copied in part or whole or otherwise
plagiarized the work of other students and/or persons.

										  Marc Lawrence C. Galura, DLSU ID# 12023817

***********************************************************************************************************************/