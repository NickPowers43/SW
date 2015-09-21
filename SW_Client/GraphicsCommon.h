#pragma once
#include <GLES2/gl2.h>
#include <SW/SW.h>
#include <vector>

#define FLOOR_TEXTURE_RES 256

#define FLOOR_SHADER_VIEW_MAT_LOC 0

#define SHADOW_VERTEX_F_COUNT 8
#define WALL_VERTEX_F_COUNT 2

typedef uint32_t MeshIndex_t;

namespace SW_Client
{
	struct SpriteRect
	{
		float x;
		float y;
		float width;
		float height;

		SpriteRect();
		SpriteRect(glm::vec2 bl, glm::vec2 dim);
	};

	struct PosUVPair
	{
		glm::vec2 pos;
		glm::vec2 uv;

		PosUVPair();
		PosUVPair(glm::vec2 pos, glm::vec2 uv);
	};

	struct PosUVMesh
	{
		int vCount;
		PosUVPair* v;
		int iCount;
		MeshIndex_t* i;

		PosUVMesh();
		PosUVMesh(MeshIndex_t* i, int iCount, PosUVPair* v, int vCount);
	};

	struct FloorProgram
	{
		GLuint program;

		int viewMat;
		GLuint texture;
		GLuint textureLoc;

		GLuint posAttrib;
		GLuint uvAttrib;
	};

	struct ShadowProgram
	{
		GLuint program;

		int viewMat;
		int playerPos;

		GLuint worldPosAttrib;
		GLuint shadowAttrib;
	};

	struct ColoredVertexProgram
	{
		GLuint program;

		int viewMat;
		int color;

		GLuint worldPosAttrib;
	};

	class Camera
	{
	public: 
		float zoom;
		glm::vec2 position;
		glm::vec2 dim;

		Camera();
		~Camera();

		void GenerateView(glm::mat4 & viewMat);
	};

	extern uint8_t* keyStates;

	extern Camera camera;
	extern FloorProgram floorProgram;
	extern ShadowProgram shadowProgram;
	extern ColoredVertexProgram coloredVertexProgram;


	extern SpriteRect* floorUVRects;
	extern PosUVMesh* floorMeshes;

	glm::vec2 WallCorner(WallType_t wall0, WallType_t wall1);

	void InitializeClient();

	void GenerateBaseVertices(SpriteRect rect, PosUVPair* output);
	void GenerateMeshes(WallType_t type, SpriteRect rightFloor, int none0, SpriteRect leftFloor, int none1);

	PosUVMesh* GetFloorMesh(FloorType_t floor0, FloorType_t floor1, WallType_t wall, int offset);
	void SetFloorMesh(FloorType_t floor0, FloorType_t floor1, WallType_t wall, int offset, PosUVMesh** val);

	void GenerateFloorMeshes();
	void AppendMeshData(PosUVMesh* floorMesh, std::vector<float> & vertices, std::vector<MeshIndex_t> & indices, glm::vec2 offset);

	GLuint loadShader(GLenum type, const char *source);
	GLuint buildProgram(GLuint vertexShader, GLuint fragmentShader);
}


