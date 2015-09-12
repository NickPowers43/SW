#include "GraphicsCommon.h"
#include <stdio.h>

using namespace std;

const char floorVShaderSource[] =
"layout(location = 0) uniform mat3 viewMat;                                 \n"
"                                                   \n"
"uniform sampler2D floorTexture;                                                   \n"
"                                                   \n"
"layout(location = 0) in vec2 floorPos;                                                  \n"
"layout(location = 1) in vec2 floorUV;                                                  \n"
"                                                   \n"
"varying vec4 color;                                 \n"
"                                                   \n"
"void main()                                         \n"
"{                                                   \n"
"   vec3 transPoint = vec3(floorPos.x, floorPos.y, 1.0);\n"
"                                                   \n"
"   transPoint = viewMat * transPoint;                                                \n"
"                                                   \n"
"   gl_Position = vec4(transPoint, 1.0);\n"
"   color = texture(floorTexture, floorUV);             \n"
"}                                                   \n";

const char floorFShaderSource[] =
"precision mediump float;                     \n"
"varying vec4 color;                          \n"
"void main()                                  \n"
"{                                            \n"
"  gl_FragColor = color;        \n"
"}                                            \n";

namespace SW_Client
{
	SpriteRect::SpriteRect()
	{
		x = 0.0f;
		y = 0.0f;
		width = 0.0f;
		height = 0.0f;
	}
	SpriteRect::SpriteRect(glm::vec2 bl, glm::vec2 dim)
	{
		x = bl.x;
		y = bl.y;
		width = dim.x;
		height = dim.y;
	}

	PosUVPair::PosUVPair()
	{
		pos.x = 0.0f;
		pos.y = 0.0f;
		uv.x = 0.0f;
		uv.y = 0.0f;
	}
	PosUVPair::PosUVPair(glm::vec2 pos, glm::vec2 uv)
	{
		PosUVPair::pos = pos;
		PosUVPair::uv = uv;
	}

	PosUVMesh::PosUVMesh()
	{
		v = NULL;
		vCount = 0;
		i = NULL;
		iCount = 0;
	}
	PosUVMesh::PosUVMesh(MeshIndex_t* i, int iCount, PosUVPair* v, int vCount)
	{
		PosUVMesh::v = new PosUVPair[vCount];
		PosUVMesh::vCount = vCount;
		for (size_t j = 0; j < vCount; j++)
		{
			PosUVMesh::v[j] = v[j];
		}
		PosUVMesh::i = new MeshIndex_t[vCount];
		PosUVMesh::iCount = iCount;
		for (size_t j = 0; j < iCount; j++)
		{
			PosUVMesh::i[j] = i[j];
		}
	}

	void InitializeClient()
	{
		float floorUVScale = 1.0f / (float)FLOOR_TEXTURE_RES;

		floorUVRects = new SpriteRect[3];
		floorUVRects[1] = SpriteRect(glm::vec2(0.0f, 0.0f) * floorUVScale, glm::vec2(32.0f, 32.0f) * floorUVScale);
		floorUVRects[2] = SpriteRect(glm::vec2(32.0f, 0.0f) * floorUVScale, glm::vec2(32.0f, 32.0f) * floorUVScale);

		GenerateFloorMeshes();

		GLuint floorVShader = loadShader(GL_VERTEX_SHADER, floorVShaderSource);
		GLuint floorFShader = loadShader(GL_FRAGMENT_SHADER, floorFShaderSource);
		floorShaderProgram = buildProgram(floorVShader, floorFShader);

		//check if the program linked successfully
		GLint linked;
		glGetProgramiv(floorShaderProgram, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			printf("floorShaderProgram link error");
			glDeleteProgram(floorShaderProgram);
		}

		glBindAttribLocation(floorShaderProgram, 0, "floorPos");
		glBindAttribLocation(floorShaderProgram, 1, "floorUV");
	}

	void GenerateBaseVertices(SpriteRect rect, PosUVPair* output)
	{
		//PosUVPair* output = new PosUVPair[8];

		//0
		output[0] = PosUVPair(
			glm::vec2(0.0, 0.0),
			glm::vec2(rect.x, rect.y));
		//1
		output[0] = PosUVPair(
			glm::vec2(0.0, 1.0),
			glm::vec2(rect.x, rect.y + rect.height));
		//2
		output[0] = PosUVPair(
			glm::vec2(1.0, 0.0),
			glm::vec2(rect.x + rect.width, rect.y));
		//3
		output[0] = PosUVPair(
			glm::vec2(1.0, 1.0),
			glm::vec2(rect.x + rect.width, rect.y + rect.height));

		//4
		output[0] = PosUVPair(
			glm::vec2(0.0, 0.5),
			glm::vec2(rect.x, rect.y + (rect.height * 0.5)));
		//5
		output[0] = PosUVPair(
			glm::vec2(0.5, 1.0),
			glm::vec2(rect.x + (rect.width * 0.5), rect.y + rect.height));
		//6
		output[0] = PosUVPair(
			glm::vec2(0.5, 0.0),
			glm::vec2(rect.x + (rect.width * 0.5), rect.y));
		//7
		output[0] = PosUVPair(
			glm::vec2(1.0, 0.5),
			glm::vec2(rect.x + rect.width, rect.y + (rect.height * 0.5)));

		//return output;
	}

	void AppendQuadIndices(MeshIndex_t* indices, int & start)
	{
		int start_ = start;
		indices[start++] = start_ + 0;
		indices[start++] = start_ + 1;
		indices[start++] = start_ + 2;
		indices[start++] = start_ + 1;
		indices[start++] = start_ + 3;
		indices[start++] = start_ + 2;
	}

	void AppendTriangleIndices(MeshIndex_t* indices, int & start)
	{
		int start_ = start;
		indices[start++] = start_ + 0;
		indices[start++] = start_ + 1;
		indices[start++] = start_ + 2;
	}

	//globals
	GLuint floorShaderProgram;

	SpriteRect* floorUVRects;
	PosUVMesh** floorMeshes;
	//

	PosUVPair* v0TempBuffer8 = new PosUVPair[8];
	PosUVPair* v1TempBuffer8 = new PosUVPair[8];
	MeshIndex_t* iTemp = new MeshIndex_t[8];
	PosUVPair* vTemp = new PosUVPair[8];

	PosUVMesh* GenerateMeshes(WallType_t type, SpriteRect rightFloor, int none0, SpriteRect leftFloor, int none1)
	{
		if (none0 && none1) {
			return NULL;
		}

		PosUVMesh* output = new PosUVMesh[2];


		GenerateBaseVertices(rightFloor, v0TempBuffer8);
		GenerateBaseVertices(leftFloor, v1TempBuffer8);

		int iTempCount = 0;
		int vTempCount = 0;

		switch (type) {
		case WallType::None:
			//wall type None (simple quad)
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[0];
				vTemp[vTempCount++] = v0TempBuffer8[1];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendQuadIndices(iTemp, iTempCount);
			}
			output[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
		case WallType::TwoByOne:
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[0];
				vTemp[vTempCount++] = v0TempBuffer8[7];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				AppendTriangleIndices(iTemp, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[7];
				vTemp[vTempCount++] = v1TempBuffer8[3];
				AppendQuadIndices(iTemp, iTempCount);
			}
			output[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			vTempCount = 0;
			iTempCount = 0;
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[0];
				vTemp[vTempCount++] = v0TempBuffer8[4];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendQuadIndices(iTemp, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[4];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[3];
				AppendTriangleIndices(iTemp, iTempCount);
			}
			output[1] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
		case WallType::OneByOne:
			//wall type None (simple quad)
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[0];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				AppendTriangleIndices(iTemp, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[3];
				AppendTriangleIndices(iTemp, iTempCount);
			}
			output[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
		case WallType::OneByTwo:
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[0];
				vTemp[vTempCount++] = v0TempBuffer8[5];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendQuadIndices(iTemp, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[5];
				AppendTriangleIndices(iTemp, iTempCount);
			}
			output[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			vTempCount = 0;
			iTempCount = 0;
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[6];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				AppendTriangleIndices(iTemp, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[6];
				vTemp[vTempCount++] = v1TempBuffer8[3];
				AppendQuadIndices(iTemp, iTempCount);
			}
			output[1] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
		case WallType::OneByTwoFlipped:
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[5];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendTriangleIndices(iTemp, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[2];
				vTemp[vTempCount++] = v1TempBuffer8[5];
				AppendQuadIndices(iTemp, iTempCount);
			}
			output[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			vTempCount = 0;
			iTempCount = 0;
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[6];
				vTemp[vTempCount++] = v0TempBuffer8[1];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendQuadIndices(iTemp, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[6];
				AppendTriangleIndices(iTemp, iTempCount);
			}
			output[1] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
		case WallType::OneByOneFlipped:
			//wall type None (simple quad)
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[1];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendTriangleIndices(iTemp, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[2];
				AppendTriangleIndices(iTemp, iTempCount);
			}
			output[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
		case WallType::TwoByOneFlipped:
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[4];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				vTemp[vTempCount++] = v0TempBuffer8[1];
				AppendQuadIndices(iTemp, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[2];
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[4];
				AppendTriangleIndices(iTemp, iTempCount);
			}
			output[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			vTempCount = 0;
			iTempCount = 0;
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[7];
				vTemp[vTempCount++] = v0TempBuffer8[1];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendTriangleIndices(iTemp, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[2];
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[7];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				AppendQuadIndices(iTemp, iTempCount);
			}
			output[1] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
		default:
			return NULL;
		}

		return output;
	}

	PosUVMesh* GetFloorMesh(FloorType_t floor0, FloorType_t floor1, WallType_t wall)
	{
		return floorMeshes[((FloorCount * FloorCount * WallTypeCount) * floor0) + (FloorCount * floor1) + wall];
	}
	void SetFloorMesh(FloorType_t floor0, FloorType_t floor1, WallType_t wall, PosUVMesh* val)
	{
		floorMeshes[((FloorCount * FloorCount * WallTypeCount) * floor0) + (FloorCount * floor1) + wall] = val;
	}

	void GenerateFloorMeshes()
	{
		floorMeshes = new PosUVMesh*[FloorCount * FloorCount * WallTypeCount];
		for (int i = 0; i < (FloorCount * FloorCount * WallTypeCount); i++) {
			floorMeshes[i] = NULL;
		}

		for (int i = 0; i < FloorCount; i++) {
			for (int j = 0; j < FloorCount; j++) {
				for (int k = 0; k < 9; k++) {

					PosUVMesh* temp = GenerateMeshes(
						(WallType_t)k,
						floorUVRects[i],
						!i,
						floorUVRects[j],
						!j);

					SetFloorMesh(i, j, k, temp);
				}
			}
		}
	}

	void AppendMeshData(PosUVMesh* floorMesh, std::vector<float> vertices, std::vector<MeshIndex_t> indices, glm::vec2 offset)
	{
		MeshIndex_t startI = vertices.size();
		for (size_t i = 0; i < floorMesh->iCount; i++) {
			indices.push_back(floorMesh->i[i] + startI);
		}


		for (size_t i = 0; i < floorMesh->vCount; i++) {
			glm::vec2 temp = floorMesh->v[i].pos;
			vertices.push_back(temp.x + offset.x);
			vertices.push_back(temp.y + offset.y);
			temp = floorMesh->v[i].uv;
			vertices.push_back(temp.x);
			vertices.push_back(temp.y);
		}
	}

	GLuint loadShader(GLenum type, const char *source)
	{
		//create a shader
		GLuint shader = glCreateShader(type);
		if (shader == 0)
		{
			printf("Error creating shader");
			return 0;
		}

		//load the shader source to the shader object and compile it
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);

		//check if the shader compiled successfully
		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			printf("Shader compilation error");
			glDeleteShader(shader);
			return 0;
		}

		return shader;
	}

	GLuint buildProgram(GLuint vertexShader, GLuint fragmentShader)
	{

		//create a GL program and link it
		GLuint programObject = glCreateProgram();
		glAttachShader(programObject, vertexShader);
		glAttachShader(programObject, fragmentShader);
		glLinkProgram(programObject);
		return programObject;
	}
}


