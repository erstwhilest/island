#pragma once

#include "glm\glm.hpp"

class ModelData
{
public:
	glm::vec3* vertices{};
	glm::vec3* normals{};
	glm::vec3* offsets{};
	glm::vec3 position{};
	int vertexCount{6};

	int instanceCountX{5};
	int instanceCountY{5};
	int instanceCountZ{1};
	int instanceCount{};

	unsigned int VAO;
	unsigned int VBO;
	unsigned int instanceVBO;
	unsigned int normalVBO;

	glm::mat4 projectionTransform;
	glm::mat4 viewTransform;
	glm::mat4 modelTransform;


	Shader shader;

	ModelData(glm::vec3 pos)
		: position{pos}
		, instanceCount{instanceCountX*instanceCountY*instanceCountZ}
	{

		vertices=new glm::vec3[vertexCount]
		{
			glm::vec3(-0.5f, +0.5f, +0.0f),
			glm::vec3(-0.5f, -0.5f, +0.0f),
			glm::vec3(+0.5f, +0.5f, +0.0f),

			glm::vec3(+0.5f, -0.5f, +0.0f),
			glm::vec3(+0.5f, +0.5f, +0.0f),
			glm::vec3(-0.5f, -0.5f, +0.0f),
		};

		offsets=new glm::vec3[instanceCount];
		generateOffsets(1.1f);
	}

	void init()
	{
		shader = Shader("src/vertexshader.glsl", "src/fragmentshader.glsl");

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &instanceVBO);
		glGenBuffers(1, &normalVBO);

		// position attribute
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, getVertexBufferSize(), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);
	
		// offset attribute
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*instanceCount, offsets, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glVertexAttribDivisor(1, 1);
		glEnableVertexAttribArray(1);
	}

	int getVertexBufferSize()
	{
		return vertexCount * sizeof(glm::vec3);
	}

	void generateOffsets(float spacing)
	{
		for (int i{}; i < instanceCountX; i++)
		{
			for (int j{}; j < instanceCountY; j++)
			{
				for (int k{}; k < instanceCountZ; k++)
				{
					offsets[i*instanceCountY*instanceCountZ+j*instanceCountZ+k]=glm::vec3{(float)i*spacing,(float)j*spacing,0.0f};
				}
			}
		}
	}

	void render()
	{
		shader.use();
		shader.setMat4("projection", projectionTransform);
		shader.setMat4("view", viewTransform);
		shader.setMat4("model", modelTransform);

		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, instanceCount);
	}

	~ModelData()
	{
		delete[] vertices;
		delete[] normals;
		delete[] offsets;
	}
};