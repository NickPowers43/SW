#pragma once
#include "MCVertex.h"
#include <vector>

typedef uint32_t MCIndex;

namespace SW_Client
{
	struct MCIndexPair
	{
		MCIndex i[2];

		MCIndexPair(MCIndex a, MCIndex b);
	};

	struct MCTriangle
	{
		MCIndex i[3];

		MCTriangle(MCIndex a, MCIndex b, MCIndex c);
	};

	class MeshConstructor
	{
	public:
		MeshConstructor();
		~MeshConstructor();

		void RecomputeNormals();
		void SimpleChamferEdges(std::vector<MCIndexPair> edges, float amount);

		int CommonVertex(MCTriangle t0, MCTriangle t1, MCIndex exclude);
		int References(int vIndex);
		static void TrianglesReferencingEdge(MCIndexPair edge, std::vector<MCTriangle> & triangles, int & t0, int & t1);
		static void RemoveVertex(int vIndex, std::vector<MCIndex> & indices);
		static void RemoveVertex(int vIndex, std::vector<MCIndex> & vertices);
		glm::vec3 TriangleNormal(MCTriangle t);
		void AddQuad(MCIndex a, MCIndex b, MCIndex c, MCIndex d);

		std::vector<MCTriangle> triangles;
		std::vector<MCVertex> vertices;
	};
}

