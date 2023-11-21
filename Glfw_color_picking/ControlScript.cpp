#include "ControlScript.h"
glm::vec3 ControlScript::RandomPosition()
{
	float x = ((float)rand() / (float)RAND_MAX) * 10.0f - 5.0f;
	float y = ((float)rand() / (float)RAND_MAX) * 10.0f - 5.0f;
	float z = ((float)rand() / (float)RAND_MAX) * 10.0f - 5.0f;

	return glm::vec3(x, y, z);
}
void ControlScript::Int_ControlScript()
{
	sphere.generateSphere(avg_raduis, 10, 10, glm::vec3(1.0f, 0.1f, 1.0f), Sphere_vert, Sphere_ind);

	for (int i = 0; i < SwarmSize; i++)
	{
		mesh* obj = new mesh;
		obj->CreateMesh(Sphere_vert, Sphere_ind);
		obj->transform.position = RandomPosition();
		obj->transform.velocity = RandomPosition() / 20.0f;
		obj->ColorID = uniqueID();
		if (obj->ColorID == Background_Color)
		{
			obj->ColorID = uniqueID();
		}
		All_Meshs.push_back(obj);

	}
}
ControlScript::~ControlScript()
{
	for (auto obj : All_Meshs)
	{
		delete obj;
	}

}
void ControlScript::SetGroups(GLuint& shaderProgram, glm::mat4& CamMatrix)
{
	
	std::vector<int>temp;
	for (int i = 0; i < All_Meshs.size(); i++)
	{
		temp.push_back(i);
	}

	int grpNum = -1;
	for (auto obj : All_Meshs)
	{
		obj->GroupID = -1;
	}
	for (int i = 0; i < All_lines.size(); i++)
	{
		All_lines[i]->ClearMesh();
	}
	Groups.clear();
	All_lines.clear();
	while (temp.size() > 0)
	{
		++grpNum;
		if (Groups.size() < grpNum + 1)
		{
			Groups.resize(grpNum + 1);
		}
		for (int i = temp.size() - 1; i > -1; --i)
		{
			if (i == 0)
			{
				Groups[grpNum].push_back(All_Meshs[temp[i]]);
				All_Meshs[temp[i]]->GroupID = grpNum;
				temp.erase(temp.begin() + i);
			}
			else if (CompareMesh(All_Meshs[temp[0]], All_Meshs[temp[i]]))
			{
				All_Meshs[temp[i]]->GroupID = grpNum;
				Groups[grpNum].push_back(All_Meshs[temp[i]]);
				temp.erase(temp.begin() + i);
			}

		}

	}

	for (int i = 0; i < Groups.size(); i++)
	{
		for (int j = 0; j < Groups[i].size(); j++)
		{
			Groups[i][j]->GroupID = i;
		}
	}
	int MaxGrp = -1;
	int MaxSize = 0;
	for (int i = 0; i < Groups.size(); i++)
	{
		if (Groups[i].size() > 1)
		{
			if (Groups[i].size() > MaxSize)
			{
				MaxSize = Groups[i].size();
				MaxGrp = i;
			}
		}
	}


	for (int i = 0; i < Groups.size(); i++)
	{
		for (int j = 0; j < Groups[i].size(); j++)
		{
			if (MaxGrp == i)
			{
				switch (linetype)
				{
				case 0:
					Original(Groups[i], glm::vec3(0, 1, 0));
					break;
				case 1:
					Dijkstra(Groups[i], glm::vec3(0, 1, 0));
					break;
				case 2:
					kruskals(Groups[i], glm::vec3(0, 1, 0));
					break;
				}
				
				
			}
			else if (Groups[i].size() > 1)
			{

				switch (linetype)
				{
				case 0:
					Original(Groups[i], glm::vec3(0, 0, 1));
					break;
				case 1:
					Dijkstra(Groups[i], glm::vec3(0, 0, 1));
					break;
				case 2:
					kruskals(Groups[i], glm::vec3(0, 0, 1));
					break;
				}

				if (MaxGrp != -1)
				{
					NonSingleToLarget(Groups[i], Groups[MaxGrp], glm::vec3(1, 1, 1));
				}
			}
			else
			{
				if (MaxGrp != -1)
				{
					SingleToLargest(Groups[i][0], Groups[MaxGrp], glm::vec3(1, 0, 0));
				}
				
			}
		}
	}
	if (All_lines.size() < 1)
	{
		NoGroups = true;
	}
	else
	{
		NoGroups = false;
	}
	ClohessyWhilshire();
	for (int i = 0; i < All_lines.size(); i++)
	{
		All_lines[i]->DrawLine(shaderProgram, CamMatrix);
	}

}
void ControlScript::ClohessyWhilshire()
{
	for (int i = 0; i < All_Meshs.size(); i++)
	{
		All_Meshs[i]->transform.drift = All_Meshs[i]->transform.velocity.y + 2.0f * All_Meshs[i]->transform.position.x;
	}

	
}