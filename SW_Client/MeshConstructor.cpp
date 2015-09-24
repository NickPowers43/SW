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
		if (vertices.size())
		{
			std::unique_ptr<int[]> normalsPerVertex(new int[vertices.size()]);

			for (size_t i = 0; i < vertices.size(); i++)
			{
				vertices[i].normal = glm::vec3(0.0f, 0.0f, 0.0f);
			}

			for (size_t i = 0; i < triangles.size();i++)
			{
				glm::vec3 tempNormal = glm::normalize(TriangleNormal(triangles[i]));

				for (size_t j = 0; j < 3; j++)
				{
					MCIndex index = triangles[i].i[j];
					vertices[index].normal += tempNormal;
					normalsPerVertex[index]++;
				}
			}

			for (size_t i = 0; i < vertices.size(); i++)
			{
				vertices[i].normal *= 1.0f / normalsPerVertex[i];
				vertices[i].normal = glm::normalize(vertices[i].normal);
			}
		}
	}

	struct ChamferCorner
	{
		std::vector<MCIndex> indices;
		std::vector<MCIndex> chamferEnds;
	};

	struct Plane
	{
		glm::vec3 o;
		glm::vec3 n;
	};

	glm::vec3 Intersection(Plane & a, Plane & b, Plane & c)
	{
		Matrix3f A;
		Vector3f B;
		A << a.n.x, a.n.y, a.n.z, b.n.x, b.n.y, b.n.z, c.n.x, c.n.y, c.n.z;
		B << glm::dot(a.n, a.o), glm::dot(b.n, b.o), glm::dot(c.n, c.o);
		Vector3f x = A.colPivHouseholderQr().solve(B);
		return glm::vec3(x.x, x.y, x.z);
	}

	void MeshConstructor::SimpleChamferEdges(std::vector<MCIndexPair> edges, float amount)
	{
		std::vector<Plane> cPlanes;
		std::map<MCIndex, ChamferCorner> corners;

		cPlanes.reserve(edges.size());
		std::for_each(edges.begin(), edges.end(), [&](MCIndexPair edge){
			Plane plane;
			plane.n = glm::normalize((vertices[edge.i[0]].normal + vertices[edge.i[1]].normal) * 0.5f);
			plane.o = glm::normalize(vertices[edge.i[0]].pos + (plane.n * -amount));
			cPlanes.push_back(plane);

			for (size_t j = 0; j < 2; j++)
			{
				if (!corners.count(edge.i[j]))
				{
					corners[edge.i[j]] = ChamferCorner();
				}
			}

			corners[edge.i[0]].chamferEnds.push_back(edge.i[1]);
			corners[edge.i[1]].chamferEnds.push_back(edge.i[0]);
		});

		std::for_each(corners.begin(), corners.end(), [&](MCIndex index){
		
		});



		glm::vec3 cornerPosition;
		glm::vec3 cP0Normal;
		glm::vec3 cP1Normal;
		glm::vec3 surfaceNormal;

		Plane sP;
		sP.o = cornerPosition;
		sP.n = surfaceNormal;
		Plane cP0;
		cP0.o = cornerPosition + (cP0Normal * -amount);
		cP0.n = cP0Normal;
		Plane cP1;
		cP1.o = cornerPosition + (cP1Normal * -amount);
		cP1.n = cP1Normal;

		

		//create a chamfer corner for each vertex effected by the operation

		//find edges containing references to vertices referenced in edgeindices
		////these will be used to reconstruct the affected triangles later

		//append new vertices created by the chamfer

		//modify triangles to use the newly created vertices

		//connect necessary chamfer corners

		for (size_t i = 0; i < edgeIndices.size(); i++)
		{
			
		}
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
	int MeshConstructor::References(int vIndex)
	{
		size_t output = 0;
		for (size_t i = 0; i < triangles.size(); i++)
		{
			if (triangles[i].i[0] == vIndex ||  triangles[i].i[1] == vIndex ||  triangles[i].i[2] == vIndex)
			{
				output++;
			}
		}
		return output;
	}
	void MeshConstructor::TrianglesReferencingEdge(MCIndexPair edge, std::vector<MCTriangle> & triangles, int & t0, int & t1)
	{
		t0 = -1;
		t1 = -1;

		bool t0Found = false;
		for (size_t i = 0; i < triangles.size();)
		{
			int matches;
			if (triangles[i].i[0] == edge.i[0]|| triangles[i].i[0] == edge.i[1]) { matches++; }
			if (triangles[i].i[1] == edge.i[0]|| triangles[i].i[1] == edge.i[1]) { matches++; }
			if (triangles[i].i[2] == edge.i[0]|| triangles[i].i[2] == edge.i[1]) { matches++; }

			if (matches > 1)
			{
				if (!t0Found)
				{
					t0 = i - 3;
					t0Found = true;
				}
				else
				{
					t1 = i - 3;
					return;
				}
			}
		}
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
	void MeshConstructor::RemoveVertex(int vIndex, std::vector<MCIndex> & vertices)
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
	glm::vec3 MeshConstructor::TriangleNormal(MCTriangle t)
	{
		glm::vec3 a(vertices[t.i[0]].pos);
		glm::vec3 b(vertices[t.i[1]].pos);
		glm::vec3 c(vertices[t.i[2]].pos);

		return glm::cross(b - a, c - a);
	}
	void MeshConstructor::AddQuad(MCIndex a, MCIndex b, MCIndex c, MCIndex d)
	{
		triangles.push_back(MCTriangle(a, d, b));
		triangles.push_back(MCTriangle(a, c, d));
	}
}