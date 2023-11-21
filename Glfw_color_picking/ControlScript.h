#ifndef CONTROL_SCRIPT_H
#define CONTROL_SCRIPT_H
#include<glm/glm.hpp>
#include "Mesh.h"
#include "Sphere.h"
#include <algorithm>
#include <queue>
class ControlScript
{
public:
	std::vector<mesh*> All_Meshs;
	std::vector<std::unique_ptr<mesh>> All_lines;
	std::vector<std::vector<mesh*>> Groups;

	void Int_ControlScript();
	~ControlScript();
	void SetGroups(GLuint& shaderProgram, glm::mat4& CamMatrix);
	void ClohessyWhilshire();
	glm::vec4 nextColorID = glm::vec4(0, 0, 0, 0);
	glm::vec4 ID = glm::vec4(0, 0, 0, 0);
	bool NoGroups = false;
	bool LineAlgo = false;
	Linetype linetype;
	std::vector<float> t_data;
	std::vector<float> y_data;
	std::vector<float> z_data;
	std::vector<float> x_data;
	
private:
	static bool CompareMesh(mesh* a, mesh* b)
	{
		if (std::abs(a->transform.drift - b->transform.drift) < min_drift && glm::distance(a->transform.position , b->transform.position) < min_distance)
		{
			return true;
		}
		else
		{
			return false;  // or true, depending on your desired ordering
		}
	}
	void SingleToLargest(mesh* a, std::vector<mesh*>group,glm::vec3 Color)
	{
		float distance = FLT_MAX;
		int index = 0;
		for (int i = 0; i < group.size(); i++)
		{
			if (glm::distance(a->transform.position, group[i]->transform.position) < distance)
			{
				distance = glm::distance(a->transform.position, group[i]->transform.position);
				index = i;
			}
		}
		std::unique_ptr<mesh> line = std::make_unique<mesh>();
		std::vector<glm::vec3> Line;
		Line.push_back(a->transform.position);
		Line.push_back(group[index]->transform.position);
		line->CreateLine(Line, Color);
		All_lines.push_back(std::move(line));
	}
	void NonSingleToLarget(std::vector<mesh*>a, std::vector<mesh*>b,glm::vec3 Color)
	{
		float distance = FLT_MAX;
		int Aindex = 0;
		int Bindex = 0;
		for (int i = 0; i < a.size(); i++)
		{
			for (int j = 0; j < b.size(); j++)
			{
				if (glm::distance(a[i]->transform.position, b[j]->transform.position) < distance)
				{
					distance = glm::distance(a[i]->transform.position, b[j]->transform.position);
					Aindex = i;
					Bindex = j;
				}
			}
		}
		std::unique_ptr<mesh> line = std::make_unique<mesh>();
		std::vector<glm::vec3> Line;
		Line.push_back(a[Aindex]->transform.position);
		Line.push_back(b[Bindex]->transform.position);
		line->CreateLine(Line, Color);
		All_lines.push_back(std::move(line));
	}
	
	

	void kruskals(std::vector<mesh*> a, glm::vec3 Color) {
		std::vector<Edge> edges;
		for (int i = 0; i < a.size(); i++) {
			for (int j = i + 1; j < a.size(); j++) {
				float distance = glm::distance(a[i]->transform.position, a[j]->transform.position);
				edges.push_back(Edge(i, j, distance));
			}
		}

		std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
			return a.distance < b.distance;
			});

		std::vector<int> parent(a.size());
		for (int i = 0; i < a.size(); i++) {
			parent[i] = i;
		}

		std::vector<Edge> mst;
		for (const Edge& edge : edges) {
			int parent1 = find(parent, edge.vertex1);
			int parent2 = find(parent, edge.vertex2);

			if (parent1 != parent2) {
				mst.push_back(edge);
				union_(parent, parent1, parent2);
			}
		}

		for (const Edge& edge : mst) {
			std::unique_ptr<mesh> line = std::make_unique<mesh>();
			std::vector<glm::vec3> Line;
			Line.push_back(a[edge.vertex1]->transform.position);
			Line.push_back(a[edge.vertex2]->transform.position);
			line->CreateLine(Line, Color);
			All_lines.push_back(std::move(line));
		}
	}

	void Dijkstra(std::vector<mesh*> a, glm::vec3 Color) 
	{
		std::vector<std::pair<mesh*, mesh*>> Pairs;
		float MaxLeft = FLT_MAX;
		int start = -1;
		std::vector<mesh*>temp = a;
		for (int i = 0; i < a.size(); i++)
		{
			if (a[i]->transform.position.x < MaxLeft)
			{
				MaxLeft = a[i]->transform.position.x;
				start = i;
			}
			
		}
		for (int i = 0; i < a.size(); i++)
		{
			if (i!=start)
			{
				temp.push_back(a[i]);
			}

		}
		temp.push_back(a[start]);
		while (temp.size() > 1)
		{
			float distance = FLT_MAX;
			int index = -1;
			for (int i = 0; i < temp.size()-1; i++)
			{
				float dist = glm::distance(temp[i]->transform.position, temp.back()->transform.position);
				if (dist < distance)
				{
					distance = dist;
					index = i;
				}
			}
			if (index > -1)
			{
				Pairs.push_back({ temp.back(), temp[index] });
			}
			temp.erase(temp.begin() + index);
			temp.pop_back();
			temp.push_back(Pairs.back().second);
		}
		// Draw lines
		for (int i = 0; i < Pairs.size(); i++) {
			std::unique_ptr<mesh> line = std::make_unique<mesh>();
			std::vector<glm::vec3> Line;
			Line.push_back(Pairs[i].first->transform.position);
			Line.push_back(Pairs[i].second->transform.position);
			line->CreateLine(Line, Color);
			All_lines.push_back(std::move(line));
			
		}

	}

	void Original(std::vector<mesh*> a, glm::vec3 Color)
	{
		for (int i = 0; i < a.size()-1; i++)
		{
			mesh* x = a[i];
			mesh* y = nullptr;
			double minDistance = std::numeric_limits<double>::max();

			for (int j = i+1; j < a.size(); j++)
			{
				if (i != j)
				{
					double distance = glm::distance(x->transform.position, a[j]->transform.position);
					if (distance < minDistance) {
						minDistance = distance;
						y = a[j];
					}
				}
			}

			if (y != nullptr)
			{
				std::unique_ptr<mesh> bline = std::make_unique<mesh>();
				std::vector<glm::vec3> Line;
				Line.push_back(x->transform.position);
				Line.push_back(y->transform.position);
				bline->CreateLine(Line, Color); //draw white line
				All_lines.push_back(std::move(bline)); // Add the unique_ptr to AllLines
			}
		}
	}



	glm::vec4 uniqueID()
	{
		if (ID[3] < 1)
		{
			ID[3] = ID[3] + 0.1f;
			nextColorID = ID;
			return nextColorID;
		}
		else if (nextColorID[2] < 1)
		{
			ID[2] = ID[2] + 0.1f;
			nextColorID = ID;
			return nextColorID;
		}
		else if (ID[1] < 1)
		{
			ID[1] = ID[1] + 0.1f;
			nextColorID = ID;
			return nextColorID;
		}
		else if (ID[0] < 1)
		{
			ID[0] = ID[0] + 0.1f;
			nextColorID = ID;
			return nextColorID;
		}
		else
		{
			std::cout << "Out of picking color";
			return glm::vec4(0,0,0,0);
		}
	}

	
	std::vector<Vertex> Sphere_vert;
	std::vector<unsigned int> Sphere_ind;
	GenSphere sphere;
	glm::vec3 RandomPosition();
	struct Edge {
		int vertex1;
		int vertex2;
		float distance;

		Edge(int v1, int v2, float dist) : vertex1(v1), vertex2(v2), distance(dist) {}
		bool operator<(const Edge& other) const {
			return distance < other.distance;
		}
	};
	

	int find(std::vector<int>& parent, int vertex) {
		if (parent[vertex] != vertex) {
			parent[vertex] = find(parent, parent[vertex]);
		}
		return parent[vertex];
	}

	void union_(std::vector<int>& parent, int vertex1, int vertex2) {
		parent[find(parent, vertex1)] = find(parent, vertex2);
	}
};
#endif
