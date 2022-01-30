#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include "Transform.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class SceneGraphNode {
public:
	Model* model;
	Shader* shader;
	Transform transform;
	SceneGraphNode* parent = NULL;
	vector<SceneGraphNode*> children;
	vector<SceneGraphNode*> instChildren;
	int size = 0;
	bool dirty = false;

	SceneGraphNode() {}
	SceneGraphNode(Model* model, Shader* shader) 
	{
		this->model = model;
		this->shader = shader;
	}

	void setParent(SceneGraphNode* parent) 
	{
		this->parent = parent;
	}

	void addChild(SceneGraphNode* child) 
	{
		this->children.push_back(child);
	}

	void addChildInst(SceneGraphNode* child)
	{
		this->instChildren.push_back(child);
	}

	void DrawInstance(Model* model, Shader* shader, int size) 
	{
		shader->use();
		shader->setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model->textures_loaded[0].id);
		for (unsigned int i = 0; i < model->meshes.size(); i++)
		{
			glBindVertexArray(model->meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, model->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, size);
			glBindVertexArray(0);
		}
	}

	void Draw() 
	{
		transform.TransformMat = glm::translate(glm::mat4(1.f), transform.position);
		transform.Local_mat = glm::translate(glm::mat4(1.f), transform.position);
		transform.Local_mat = glm::scale(this->transform.Local_mat, this->transform.scale);
		transform.Local_mat = glm::rotate(this->transform.Local_mat, this->transform.a, this->transform.rotation);
		transform.RotateMat = glm::eulerAngleXZY(transform.e_x, transform.e_y, transform.e_z);

		transform.Local_mat = transform.TransformMat * transform.RotateMat;

		if (parent != NULL) 
		{
			transform.World_mat = parent->transform.World_mat * transform.Local_mat;
		}
		else 
		{
			transform.World_mat = transform.Local_mat;
		}

		for (int i = 0; i < children.size(); i++) 
		{
			children[i]->Draw();
		}

		transform.World_mat = glm::scale(transform.World_mat, transform.scale);
		shader->use();
		shader->setMat4("model", transform.World_mat);
		shader->setFloat("DFP", transform.Dist_f_Parent[1]);
		shader->setVec3("color", transform.color);

		model->Draw(*shader);
	}

	void DrawI() 
	{
		for (int i = 0; i < instChildren.size(); i++) 
		{
			DrawInstance(instChildren[i]->model, instChildren[i]->shader, instChildren[i]->size);
		}
	}

	void Update_World()
	{
		if (this->dirty) 
		{
			this->transform.Local_mat = glm::translate(glm::mat4(1.f), this->transform.position);
			this->transform.Local_mat = glm::scale(this->transform.Local_mat, this->transform.scale);
			this->transform.Local_mat = glm::rotate(this->transform.Local_mat, this->transform.a, this->transform.rotation);
			transform.RotateMat = glm::eulerAngleXZY(transform.e_x, transform.e_z, transform.e_y);
			this->transform.Local_mat = this->transform.Local_mat * transform.RotateMat;
			if (parent != NULL) 
			{
				this->transform.World_mat = parent->transform.World_mat * transform.Local_mat;
			}
			else 
			{
				this->transform.World_mat = transform.Local_mat;
			}

			for (int i = 0; i < children.size(); i++)
			{
				children[i]->dirty = true;
				children[i]->Update_World();
			}
			dirty = false;
		}
	}
};