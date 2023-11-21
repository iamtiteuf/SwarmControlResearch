
#include "Glew_Initialization.h"
#include "shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "ColorPicking.h"
#include "Sphere.h"
#include "PhysicsHandler.h"
#include "implot.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "ControlScript.h"
#include "ScriptingArea.h"

bool start_Simulation = false;
bool IsWidget;

Shaders shader; // main shader program
Shaders pickingshader; // picking color shader program

GenSphere c_Sphere;//generate sphre vertices

Color_Picking colorPicking; 

Glew_Initialization sys;

std::vector<Vertex> cube_vertex =
{
	// positions          // colors           // normals         // texUV
 { glm::vec3(-0.3,-0.3,-0.3), glm::vec3(1.0,0.0,0.5), glm::vec3(-1,-1,-1), glm::vec2(0, 0) },
 { glm::vec3(-0.3,-0.3,0.3), glm::vec3(1.0,0.0,0.5), glm::vec3(-1,-1, 1), glm::vec2(1, 0) },
 { glm::vec3(-0.3,0.3,-0.3),  glm::vec3(1.0,0.0,0.5), glm::vec3(-1, 1,-1), glm::vec2(0, 1) },
 { glm::vec3(-0.3,0.3,0.3),  glm::vec3(1.0,0.0,0.5), glm::vec3(-1, 1, 1), glm::vec2(1, 1) },
 { glm::vec3(0.3,-0.3,-0.3),  glm::vec3(1.0,0.0,0.5), glm::vec3(1,-1,-1),  glm::vec2(1, 0) },
 { glm::vec3(0.3,-0.3,0.3),   glm::vec3(1.0,0.0,0.5), glm::vec3(1,-1, 1),  glm::vec2(0, 0) },
 { glm::vec3(0.3,0.3,-0.3),  glm::vec3(1.0,0.0,0.5) , glm::vec3(1, 1,-1),  glm::vec2(1, 1) },
 { glm::vec3(0.3,0.3,0.3),    glm::vec3(1.0,0.0,0.5), glm::vec3(1, 1, 1),  glm::vec2(0, 1) }
};
std::vector<unsigned int> indices =
{
   0, 1, 2, 1, 2, 3, 4, 5, 6, 5, 6, 7, // front 
   1, 5, 3, 5, 3, 7, 0, 4, 2, 4, 2, 6, // back 
   0, 1, 4, 1, 4, 5, 2, 3, 6, 3, 6, 7  // sides
};


std::vector<std::unique_ptr<mesh>> All_lines;

Eigen::MatrixXd Kk;

glm::mat4 CamMatrix; //camera Matrix

void ManageWidget();
void Render_Scene();
void Render_Picking_Scene();
int  Get_Object(glm::vec4 ColorID);
bool Almost_Equal(glm::vec4 a, glm::vec4 b);
void InitiatePicking();
void ClearUp();
void CheckIfWidget(ImGuiIO& io)
{
	if (io.WantCaptureMouse || ImGui::IsAnyItemHovered())
	{
		IsWidget = true;
	}
	else
	{
		IsWidget = false;
	}
}

bool leftclicked = false;
int Cube_ID = 0;
int Cube2_ID = 0;
Script_Editor pyIntergrate;

ControlScript Control;
int main()
{
//#if defined(WIN32) || defined(_WIN32)
//	_putenv_s("PYTHONHOME", "C:/Users/ndene/AppData/Local/Programs/Python/Python39");
//#else
//	setenv("PYTHONHOME", "C:/Users/ndene/AppData/Local/Programs/Python/Python39", 1);
//#endif
	//PhysicsHandler::LQR_Solver(Kk, 1, 1,- 10000);
	//std::cout << "kk" << Kk;
	sys.Init_Glfw();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(Main_Window , true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Users/ndene/Documents/Visual Studio 2022/Projects/Thesis/ExternalLibs/freedom-font/VTCSundaykomix.ttf", 18.5f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Users/User/Documents/Visual Studio 2022/Projects/Hakujitsu _v2/Hakujitsu/ExternalLibs/freedom-font/VIPRoman-Regular.ttf", 18.5f);
	if (font)
	{
		std::cout << "Font loaded" << "\n";
	}
	else
	{
		std::cout << "Font failed to load" << "\n";
	}
	io.FontDefault = font;
	Camera camera;

	shader.Create_Normal_Shaders();
	pickingshader.Create_Picking_Shaders();

	// create custome framebuffer
	colorPicking.Init(sys.width, sys.height, glm::vec3(0, 0, 0));
	std::vector<Vertex> Sphere_vert;
	std::vector<unsigned int> Sphere_ind;

	
	//create cubes and assign unique ColorIDs
	c_Sphere.generateSphere(0.5f, 10, 10, glm::vec3(1, 0, 0), Sphere_vert, Sphere_ind);




	
	
	
	
	
	glClearColor(Background_Color[0], Background_Color[1], Background_Color[2], Background_Color[3]);
	glEnable(GL_DEPTH_TEST);
	
	while (!glfwWindowShouldClose(Main_Window ))
	{
		/*
		Predic_Transforms.clear();
		for (auto obj : Control.All_Meshs)
		{
			Predic_Transforms.push_back(obj->transform);
		}*/

		
		CheckIfWidget(io);
		glfwPollEvents();
		CamMatrix = camera.Camera_Matrix();
		camera.Camera_Controls(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		Control.SetGroups(shader.shaderprogram, CamMatrix);
		Render_Scene();
		
		if (Control.All_Meshs.size() > 0)
		{
			InitiatePicking();
		}
		
		

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ui implementation
		
		ManageWidget();
		if (ShowScript)
		{
			pyIntergrate.OpenWindow();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(Main_Window );
		

	}
	
	ClearUp();
	glfwDestroyWindow(Main_Window );
	glfwTerminate();
	
}


void Render_Scene()
{
	if (start_Simulation)
	{
		PhysicsHandler::Calc_Omega(Control.All_Meshs);
		pyIntergrate.RunScript(omega);
		PhysicsHandler::Forces(Control.All_Meshs,Control.t_data, Control.x_data, Control.y_data, Control.z_data);
	}

	for (int i = 0; i < Control.All_Meshs.size(); i++)
	{
		if (start_Simulation)
		{
			PhysicsHandler::Step(Control.All_Meshs[i]->transform);
		}

	}
	for (auto obj : Control.All_Meshs)
	{
		obj->RenderMesh(shader.shaderprogram, CamMatrix);
		
	}
	
}
void Render_Picking_Scene()
{
	for (int i = 0; i < Control.All_Meshs.size(); i++)
	{
		Control.All_Meshs[i]->RenderMesh(pickingshader.shaderprogram, CamMatrix);
	}
}
void InitiatePicking()
{
	if (glfwGetMouseButton(Main_Window , GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !IsWidget)
	{
		if (!leftclicked)
		{
			Render_Scene();
			Render_Picking_Scene();
			//draw cubes in the custom frame buffer
			colorPicking.EnableWriting();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			Render_Picking_Scene();
			colorPicking.DisableWriting();
			//read color values from custom frame buffer
			Cube_ID = Get_Object(colorPicking.Read_Pixels());

			Control.All_Meshs[Cube_ID]->UpdateColor(glm::vec3(0.9f,0.9f,0.9f));

			for (int i = 0; i < Control.All_Meshs.size(); i++)
			{
				if (i != Cube_ID)
				{
					Control.All_Meshs[i]->UpdateColor(Control.All_Meshs[i]->Color);
				}
			}
			leftclicked = true;
		}

	}
	else if (glfwGetMouseButton(Main_Window , GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		if (leftclicked)
		{
			leftclicked = false;
		}

	}
}
int Get_Object(glm::vec4 ColorID)
{
	int e = 0;
	for (int i = 0; i < Control.All_Meshs.size(); i++)
	{
		if (Almost_Equal(Control.All_Meshs[i]->ColorID, ColorID))
		{
			e = i;
			break;

		}
	}
	if (ColorID == glm::vec4(0.2f, 0.3f, 0.3f, 1.0f))
	{
		return Cube_ID;
	}
	else
	{
		return e;
	}


}
bool Almost_Equal(glm::vec4 a, glm::vec4 b)
{
	double epsilon = 0.0001;
	bool check_diff = true;
	for (int i = 0; i < 4; i++)
	{
		if (std::abs(a[i] - b[i]) > epsilon)
		{
			check_diff = false;
		}
	}
	return check_diff;
}
void ClearUp()
{
	for (auto obj : Control.All_Meshs)
	{
		obj->ClearMesh();
	}
	ImPlot::DestroyContext();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}
void ManageWidget()
{
	ImGui::Begin("Settings");
	if (Control.All_Meshs.size() > 0)
	{
		if (ImGui::Button("Reset"))
		{
			for (auto obj : Control.All_Meshs)
			{
				obj->ClearMesh();
			}
			Control.All_Meshs.clear();
			start_Simulation = false;
			leftclicked = false;
			Cube_ID = 0;
			Cube2_ID = 0;
			Control.nextColorID = glm::vec4(0, 0, 0, 0);
			Control.ID = glm::vec4(0, 0, 0, 0);

		}
		
	}
	else
	{
		if (ImGui::Button("spawn"))
		{
			Control.Int_ControlScript();
		}
	}
	

	ImGui::InputInt("SwarmSize", &SwarmSize);
	ImGui::InputFloat("Sphere raduis", &avg_raduis);
	ImGui::InputFloat("Min Drift", &min_drift);
	ImGui::InputFloat("Min Distance", &min_distance);
	if (ImGui::Combo("Line Type", (int*)&Control.linetype, "Original\0kruskals\0Dijkstra\0")) {
		// The selected item is stored in the variable line
	}

	if (Control.All_Meshs.size() > 0)
	{
		if (Control.NoGroups)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red color
			ImGui::Text("No Groups Found");;
			ImGui::PopStyleColor();	
		}
		ImGui::Text("Object ID");
		ImGui::SameLine();
		ImGui::Text("%0.1d", Cube_ID);
		ImGui::InputFloat("drift", &Control.All_Meshs[Cube_ID]->transform.drift);

		ImGui::InputInt("Group Number", &Control.All_Meshs[Cube_ID]->GroupID);
		ImGui::InputInt("ID", &Cube_ID);
		ImGui::InputFloat("gravitaional Constant", &G_const);
		ImGui::InputFloat("Time Step", &dt);
		ImGui::Checkbox("Start Simulation", &start_Simulation);
		

		if (ImGui::CollapsingHeader("Object Transfrom"))
		{
			float pos3[3] = { Control.All_Meshs[Cube_ID]->transform.position.x, Control.All_Meshs[Cube_ID]->transform.position.y, Control.All_Meshs[Cube_ID]->transform.position.z };

			if (ImGui::InputFloat3("position", pos3))
			{
				Control.All_Meshs[Cube_ID]->transform.position.x = pos3[0];
				Control.All_Meshs[Cube_ID]->transform.position.y = pos3[1];
				Control.All_Meshs[Cube_ID]->transform.position.z = pos3[2];

			}
			float sca3[3] = { Control.All_Meshs[Cube_ID]->transform.scale.x, Control.All_Meshs[Cube_ID]->transform.scale.y, Control.All_Meshs[Cube_ID]->transform.scale.z };

			if (ImGui::InputFloat3("scale", sca3))
			{
				Control.All_Meshs[Cube_ID]->transform.scale.x = sca3[0];
				Control.All_Meshs[Cube_ID]->transform.scale.y = sca3[1];
				Control.All_Meshs[Cube_ID]->transform.scale.z = sca3[2];
			}
			float vec3[3] = { Control.All_Meshs[Cube_ID]->transform.velocity.x, Control.All_Meshs[Cube_ID]->transform.velocity.y, Control.All_Meshs[Cube_ID]->transform.velocity.z };

			if (ImGui::InputFloat3("velocity", vec3))
			{
				Control.All_Meshs[Cube_ID]->transform.velocity.x = vec3[0];
				Control.All_Meshs[Cube_ID]->transform.velocity.y = vec3[1];
				Control.All_Meshs[Cube_ID]->transform.velocity.z = vec3[2];

			}
			if (ImGui::InputFloat3("force", vec3))
			{
				Control.All_Meshs[Cube_ID]->transform.force.x = vec3[0];
				Control.All_Meshs[Cube_ID]->transform.force.y = vec3[1];
				Control.All_Meshs[Cube_ID]->transform.force.z = vec3[2];

			}


			if (ImGui::InputFloat("mass", &Control.All_Meshs[Cube_ID]->transform.mass))
			{

			}

		}
	}
	if (ImGui::Checkbox("Show Script",&ShowScript))
	{
		
	}
	if (ImGui::CollapsingHeader("Gain : "))
	{
		if (ImGui::Begin("Group Table"))
		{
			// Check if the matrix is initialized
			if (!Gain.rows() || !Gain.cols()) {
				ImGui::Text("Matrix not initialized");
				ImGui::End();
				return;
			}

			// Create a new table
			if (ImGui::BeginTable("Gain Matrix ", Gain.cols(), ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) // +1 for row numbers
			{
				// Draw new row at the top
				
				for (int j = 0; j < Gain.cols(); ++j)
				{
					/*ImGui::TableNextColumn();
					ImGui::Text("%d", j );*/
				}

				// Draw existing rows
				for (int i = 0; i < Gain.rows(); ++i)
				{
					/*ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("%d", i);*/

					// Draw row cells
					for (int j = 0; j < Gain.cols(); ++j)
					{
						// Check if the indices are within the valid range
						if (i < 0 || i >= Gain.rows() || j < 0 || j >= Gain.cols()) {
							ImGui::TableNextColumn();
							ImGui::Text("Out of bounds");
						}
						else {
							ImGui::TableNextColumn();
							ImGui::Text("%.17e", Gain(i, j)); // Use %.2f to display as float with 2 decimal places
						}
					}
				}
				ImGui::EndTable();
			}
		}
		ImGui::End();
	}

	if (ImGui::CollapsingHeader("Show graph : "))
	{
		/*float min_x = *std::min_element(Control.x_data.begin(), Control.x_data.end());
		float max_x = *std::max_element(Control.x_data.begin(), Control.x_data.end());
		float min_y = *std::min_element(Control.y_data.begin(), Control.y_data.end());
		float max_y = *std::max_element(Control.y_data.begin(), Control.y_data.end());*/
		/*ImPlot::SetNextAxisLimits(ImAxis_X1, min_x, max_x);
		ImPlot::SetNextAxisLimits(ImAxis_Y1, min_y, max_y);*/
		ImPlot::SetNextAxisToFit(ImAxis_X1);
		ImPlot::SetNextAxisToFit(ImAxis_Y1);
		
		ImGui::Begin("plot");
		if (ImPlot::BeginPlot("x Plot")) {
			
			ImPlot::PlotLine("x Plot :", Control.t_data.data(), Control.x_data.data(), Control.t_data.size());
			
			
			ImPlot::EndPlot();
		}
		ImPlot::SetNextAxisToFit(ImAxis_X1);
		ImPlot::SetNextAxisToFit(ImAxis_Y1);
		if (ImPlot::BeginPlot("y Plot")) {

			
			ImPlot::PlotLine("y Plot :", Control.t_data.data(), Control.y_data.data(), Control.t_data.size());
			

			ImPlot::EndPlot();
		}
		ImPlot::SetNextAxisToFit(ImAxis_X1);
		ImPlot::SetNextAxisToFit(ImAxis_Y1);
		if (ImPlot::BeginPlot("z Plot")) {

			
			ImPlot::PlotLine("z Plot :", Control.t_data.data(), Control.z_data.data(), Control.t_data.size());

			ImPlot::EndPlot();
		}
		ImGui::End();
	}
	
	ImGui::End();
}

