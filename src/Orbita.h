#pragma once
#include<vector>
#include <glm/gtx/euler_angles.hpp>
using namespace std;
class Orbita :public SceneGraphNode 
{
public:
	SceneGraphNode* object;

	Orbita(SceneGraphNode* object) 
	{
		this->object = object;
	}

	void Count_Position(float alpha) 
	{
		object->transform.position = glm::vec3(cos(alpha * object->transform.fast) * object->transform.Dist_f_Parent[0], 
										200, sin(alpha * object->transform.fast) * object->transform.Dist_f_Parent[0]);
	}

};

