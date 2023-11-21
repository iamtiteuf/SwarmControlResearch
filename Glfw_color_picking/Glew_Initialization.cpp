#include "Glew_Initialization.h"
GLFWwindow* Main_Window = nullptr;
glm::vec3 Camera_Position = glm::vec3(0, 0, 30.0f);
int SwarmSize = 2;
float min_drift = 1.0f;
float  min_distance = 1.0f;
glm::vec4 Background_Color = glm::vec4(0.2, 0.2, 0.2, 0.2);
float G_const = 1.0f;
float dt = 0.06f;
float avg_raduis =0.3f;
double wt = 0.0;
bool ShowScript = false;
double omega = 0;
double c_mass = 1;
Eigen::MatrixXd Gain = (Eigen::MatrixXd(3, 6) << 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0).finished();
enum Linetype
{
	Original = 0,
	Dijkstra = 1,
	kruskals = 2
	
};