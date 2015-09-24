#pragma once

#include <GLES2/gl2.h>
#include <SW/SW.h>
#include "GraphicsCommon.h"

namespace SW_Client
{
	struct BufferedSubMesh
	{
		BufferedSubMesh();
		BufferedSubMesh(MeshIndex_t offset, MeshIndex_t count);

		MeshIndex_t offset;
		MeshIndex_t count;

		void Draw();
	};

	class BufferedMeshArray
	{
	public:
		BufferedMeshArray(int count);
		~BufferedMeshArray();

		GLuint vbo;
		GLuint ibo;
		BufferedSubMesh* meshes;

		void Bind();
	};
}

