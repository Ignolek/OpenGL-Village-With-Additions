#pragma once

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <time.h>

using namespace std;
class Transform
{
public:

	glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);

	glm::vec3 color = glm::vec3
	((static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1))),
		(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1))),
		(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1))));
	float e_x = 0, e_y = 0, e_z = 0;

	
	glm::vec3 rotation = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);
	glm::vec2 Dist_f_Parent = glm::vec2(0, 0);

	glm::mat4 This_Pos = glm::mat4(1.0f);
	glm::mat4 Local_mat = glm::mat4(1.0f);
	glm::mat4 World_mat = glm::mat4(1.0f);
	glm::mat4 TransformMat = glm::mat4(1.0f);
	glm::mat4 ScaleMat = glm::mat4((static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1))));
	glm::mat4 RotateMat = glm::mat4(1.0f);
	float a = 0;

	float fast = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 6))) - 3;

	Transform() {}
};
