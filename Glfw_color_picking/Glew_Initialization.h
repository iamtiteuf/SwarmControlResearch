#ifndef ENVIR_INITIALIZE_H
#define ENVIR_INITIALIZE_H

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include <glm/gtx/hash.hpp>
#include <vector>
#include <memory>
#include <Eigen/Dense>

extern float G_const;
extern glm::vec4 Background_Color;
extern glm::vec3 Camera_Position;
extern int SwarmSize;
extern float min_drift;
extern float min_distance;
extern float dt;
extern float avg_raduis;
extern enum Linetype;
extern double wt;
extern bool ShowScript;
extern Eigen::MatrixXd Gain;
extern double omega;
extern double c_mass;
struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 texUV;
};
struct Transforms
{
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 rotation;
	glm::vec3 scale = glm::vec3(1, 1, 1);

	glm::vec3 force = glm::vec3(0, 0, 0);
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	float mass = 10.0f;
	float gravity = 10.0f;
	float raduis = 5000.0f;
	float drift = 0.0f;
	Eigen::MatrixXd Xdot;
};

extern GLFWwindow* Main_Window;

class Glew_Initialization
{
public:

	
	float width = 1920;
	float height = 1080;
	int Init_Glfw()
	{
		if (!glfwInit())
		{
			return EXIT_FAILURE;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		Main_Window = glfwCreateWindow(width, height, "SolarSystem", NULL, NULL);
		
		if (!Main_Window )
		{
			glfwTerminate();
			return EXIT_FAILURE;
		}
		glfwMakeContextCurrent(Main_Window );
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			glfwDestroyWindow(Main_Window );
			glfwTerminate();

			std::cout << "failed to initialize glew";
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

};


#endif
