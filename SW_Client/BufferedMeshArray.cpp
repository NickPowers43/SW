#include "BufferedMeshArray.h"


namespace SW_Client
{

	BufferedSubMesh::BufferedSubMesh()
	{
		BufferedSubMesh::offset = 0;
		BufferedSubMesh::count = 0;
	}
	BufferedSubMesh::BufferedSubMesh(MeshIndex_t offset, MeshIndex_t count)
	{
		BufferedSubMesh::offset = offset;
		BufferedSubMesh::count = count;
	}

	void BufferedSubMesh::Draw()
	{
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (GLvoid*)offset);
	}

	BufferedMeshArray::BufferedMeshArray(int count)
	{
		meshes = new BufferedSubMesh[count];

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);
	}


	BufferedMeshArray::~BufferedMeshArray()
	{
	}

	void BufferedMeshArray::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	}
}
