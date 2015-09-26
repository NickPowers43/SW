#include "MeshConstructor.h"
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <map>
#include <algorithm>
#include <array>
#include <Eigen\Dense>

using namespace Eigen;

namespace SW_Client
{

	MCIndexPair::MCIndexPair(MCIndex a, MCIndex b)
	{
		i[0] = a;
		i[1] = b;
	}

	MCTriangle::MCTriangle(MCIndex a, MCIndex b, MCIndex c)
	{
		i[0] = a;
		i[1] = b;
		i[2] = c;
	}

	MeshConstructor::MeshConstructor()
	{
	}


	MeshConstructor::~MeshConstructor()
	{
	}

	void MeshConstructor::RecomputeNormals()
	{
		if (v.size())
		{
			std::unique_ptr<int[]> normalsPerVertex(new int[v.size()]);

			for (size_t i = 0; i < v.size(); i++)
			{
				normalsPerVertex[i] = 0;
				v[i].normal = glm::vec3(0.0f, 0.0f, 0.0f);
			}

			for (size_t i = 0; i < t.size();i++)
			{
				glm::vec3 tempNormal = glm::normalize(FaceCross(t[i]));

				for (size_t j = 0; j < 3; j++)
				{
					MCIndex index = t[i].i[j];
					v[index].normal += tempNormal;
					normalsPerVertex[index]++;
				}
			}

			for (size_t i = 0; i < v.size(); i++)
			{
				if (normalsPerVertex[i] > 0)
				{
					v[i].normal *= 1.0f / normalsPerVertex[i];
					v[i].normal = glm::normalize(v[i].normal);
				}
			}
		}
	}

	struct ChamferCorner
	{
		MCIndex cornerI;
		std::map<MCIndex, MCIndexPair> cVertices;
		std::vector<MCIndex> indices;
	};

	int CCWAlong(glm::vec3 normal, glm::vec3 origin, glm::vec3 a, glm::vec3 b)
	{
		return glm::dot(glm::cross(a - origin, b - origin), normal) > 0.0f;
	}

	int MeshConstructor::CommonVertex(MCTriangle t0, MCTriangle t1, MCIndex exclude)
	{
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				if (t0.i[i] != exclude && t0.i[i] == t1.i[j])
				{
					return t0.i[i];
				}
			}
		}

		return -1;
	}
	void MeshConstructor::RemoveVertex(int vIndex, std::vector<MCIndex> & indices)
	{
		for (size_t i = 0; i < indices.size(); i++)
		{
			if (indices[i] > vIndex)
			{
				indices[i]--;
			}
		}
	}
	void MeshConstructor::RemoveVertex(int vIndex, std::vector<MCVertex> & vertices)
	{
		if (vertices.size() > vIndex)
		{
			for (size_t i = vIndex + 1; i < vertices.size(); i++)
			{
				vertices[i - 1] = vertices[i];
			}
			vertices.pop_back();
		}
	}
	glm::vec3 MeshConstructor::FaceCross(MCTriangle triangle)
	{
		glm::vec3 a(v[triangle.i[0]].pos);
		glm::vec3 b(v[triangle.i[1]].pos);
		glm::vec3 c(v[triangle.i[2]].pos);

		return glm::cross(b - a, c - a);
	}
	void MeshConstructor::AddQuad(MCIndex a, MCIndex b, MCIndex c, MCIndex d)
	{
		t.push_back(MCTriangle(a, d, b));
		t.push_back(MCTriangle(a, c, d));
	}
}