#include "GraphicsCommon.h"
#include "SW_Client.h"
#include <stdio.h>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL/SDL_image.h>
#include <SDL/SDL_input.h>


using namespace std;

const char coloredVertexVShaderSource[] =
"uniform mat4 viewMat;                                 \n"
"attribute vec2 worldPos;                                                  \n"
"void main()                                         \n"
"{                                                   \n"
"   vec4 transPoint = vec4(worldPos.x, worldPos.y, 0.0, 1.0);\n"
"   transPoint = viewMat * transPoint;                                                \n"
"   gl_Position = transPoint;                      \n"
"}                                                   \n";

const char coloredVertexFShaderSource[] =
"precision mediump float;                     \n"
"uniform vec4 color;                                 \n"
"void main()                                  \n"
"{                                            \n"
"  gl_FragColor = color;        \n"
"}                                            \n";

const char shadowVShaderSource[] =
"uniform mat4 viewMat;                                 \n"
"uniform vec2 playerPos;                                 \n"
"                                                   \n"
"attribute vec4 worldPos;                                                  \n"
"attribute vec4 shadowAttrib;                                                  \n"
"                                                   \n"
"varying float alpha;                                 \n"
"                                                   \n"
"void main()                                         \n"
"{                                                   \n"
"   vec2 worldPosV2 = vec2(worldPos.x, worldPos.y);                                                \n"
"   vec2 fworldPos = worldPosV2 + ((normalize(worldPosV2 - playerPos) * 100.0) * shadowAttrib.x);                                                \n"
"   vec4 transPoint = vec4(fworldPos.x, fworldPos.y, 0.0, 1.0);\n"
"                                                   \n"
"   transPoint = viewMat * transPoint;                                                \n"
"                                                   \n"
"   gl_Position = transPoint;                      \n"
"   alpha = shadowAttrib.z;             \n"
"}                                                   \n";


const char shadowFShaderSource[] =
"precision mediump float;                     \n"
"varying float alpha;                                 \n"
"void main()                                  \n"
"{                                            \n"
"  gl_FragColor = vec4(0.0, 0.0, 0.0, alpha);        \n"
"}                                            \n";

const char floorVShaderSource[] =
"uniform mat4 viewMat;                                 \n"
"                                                   \n"
"                                                   \n"
"attribute vec2 floorPos;                                                  \n"
"attribute vec2 floorUV;                                                  \n"
"                                                   \n"
"varying vec2 myUV;                                 \n"
"                                                   \n"
"void main()                                         \n"
"{                                                   \n"
"   vec4 transPoint = vec4(floorPos.x, floorPos.y, 0.0, 1.0);\n"
"                                                   \n"
"   transPoint = viewMat * transPoint;                                                \n"
"                                                   \n"
"   gl_Position = transPoint;\n"
"   myUV = floorUV;             \n"
"}                                                   \n";

const char floorFShaderSource[] =
"precision mediump float;                     \n"
"uniform sampler2D floorTexture;                                                   \n"
"varying vec2 myUV;                          \n"
"void main()                                  \n"
"{                                            \n"
"  gl_FragColor = texture2D(floorTexture, myUV);        \n"
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
		PosUVMesh::vCount = vCount;
		if (iCount)
		{
			PosUVMesh::v = new PosUVPair[vCount];
			for (size_t j = 0; j < vCount; j++)
			{
				PosUVMesh::v[j] = v[j];
			}
		}
		PosUVMesh::iCount = iCount;
		if (vCount)
		{
			PosUVMesh::i = new MeshIndex_t[iCount];
			for (size_t j = 0; j < iCount; j++)
			{
				PosUVMesh::i[j] = i[j];
			}
		}
	}

	Camera::Camera()
	{
		zoom = 1.0f / 4.0f;
		position = glm::vec2(0.0f, 0.0f);
		dim = glm::vec2(1.0f, 1.0f);
	}
	Camera::~Camera()
	{

	}

	void Camera::GenerateView(glm::mat4 & viewMat)
	{
		float InvAspectRatio = dim.y / dim.x;
		viewMat = glm::scale(viewMat, glm::vec3(zoom * InvAspectRatio, zoom, 1.0f));
		viewMat = glm::translate(viewMat, glm::vec3(-position.x, -position.y, 0.0f));
	}

	void InitializeClient()
	{
		SW::Initialize();

		keyStates = SDL_GetKeyboardState(NULL);

		glEnable(GL_TEXTURE_2D);

		SDL_Surface* surface = IMG_Load("data/textures/floors.png");
		GLenum texture_format;
		int nOfColors = surface->format->BytesPerPixel;
		if (nOfColors == 4)     // contains an alpha channel
		{
			texture_format = GL_RGBA;
		}
		else if (nOfColors == 3)     // no alpha channel
		{
			texture_format = GL_RGB;
		}
		else
		{
			PrintMessage((int)"Unknown color format for texture");
		}

		if (!surface)
		{
			std::ostringstream print0;
			print0 << "IMG_Load: " << IMG_GetError();
			PrintMessage((int)print0.str().c_str());
		}
		else
		{
			std::ostringstream print0;
			print0 << "IMG_Load: {width: " << surface->w << ", height: " << surface->h << "}";
			PrintMessage((int)print0.str().c_str());
			//glActiveTexture(GL_TEXTURE0);
			glGenTextures(1, &floorProgram.texture);
			glBindTexture(GL_TEXTURE_2D, floorProgram.texture);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, texture_format, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			//glFinish();
			SDL_FreeSurface(surface);
		}

		float floorUVScale = 1.0f / (float)FLOOR_TEXTURE_RES;

		floorUVRects = new SpriteRect[3];
		floorUVRects[1] = SpriteRect(glm::vec2(0.0f, 0.0f) * floorUVScale, glm::vec2(32.0f, 32.0f) * floorUVScale);
		floorUVRects[2] = SpriteRect(glm::vec2(32.0f, 0.0f) * floorUVScale, glm::vec2(32.0f, 32.0f) * floorUVScale);

		GenerateFloorMeshes();

		GLuint floorVShader = loadShader(GL_VERTEX_SHADER, floorVShaderSource);
		GLuint floorFShader = loadShader(GL_FRAGMENT_SHADER, floorFShaderSource);
		floorProgram.program = buildProgram(floorVShader, floorFShader);

		//check if the program linked successfully
		GLint linked;
		glGetProgramiv(floorProgram.program, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			printf("floorShaderProgram link error");
			glDeleteProgram(floorProgram.program);
		}
		else
		{
			floorProgram.viewMat = glGetUniformLocation(floorProgram.program, "viewMat");
			floorProgram.posAttrib = glGetAttribLocation(floorProgram.program, "floorPos");
			floorProgram.uvAttrib = glGetAttribLocation(floorProgram.program, "floorUV");
			floorProgram.textureLoc = glGetUniformLocation(floorProgram.program, "floorTexture");
		}

		GLuint shadowVShader = loadShader(GL_VERTEX_SHADER, shadowVShaderSource);
		GLuint shadowFShader = loadShader(GL_FRAGMENT_SHADER, shadowFShaderSource);
		shadowProgram.program = buildProgram(shadowVShader, shadowFShader);

		//check if the program linked successfully
		glGetProgramiv(shadowProgram.program, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			printf("shadowShaderProgram link error");
			glDeleteProgram(shadowProgram.program);
		}
		else
		{
			shadowProgram.viewMat = glGetUniformLocation(shadowProgram.program, "viewMat");
			shadowProgram.playerPos = glGetUniformLocation(shadowProgram.program, "playerPos");
			shadowProgram.worldPosAttrib = glGetAttribLocation(shadowProgram.program, "worldPos");
			shadowProgram.shadowAttrib = glGetAttribLocation(shadowProgram.program, "shadowAttrib");
		}

		GLuint coloredVertexVShader = loadShader(GL_VERTEX_SHADER, coloredVertexVShaderSource);
		GLuint coloredVertexFShader = loadShader(GL_FRAGMENT_SHADER, coloredVertexFShaderSource);
		coloredVertexProgram.program = buildProgram(coloredVertexVShader, coloredVertexFShader);

		//check if the program linked successfully
		glGetProgramiv(coloredVertexProgram.program, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			printf("coloredVertexShaderProgram link error");
			glDeleteProgram(coloredVertexProgram.program);
		}
		else
		{
			coloredVertexProgram.color = glGetUniformLocation(coloredVertexProgram.program, "color");
			coloredVertexProgram.viewMat = glGetUniformLocation(coloredVertexProgram.program, "viewMat");
			coloredVertexProgram.worldPosAttrib = glGetAttribLocation(coloredVertexProgram.program, "worldPos");
		}
	}

	void GenerateBaseVertices(SpriteRect rect, PosUVPair* output)
	{
		//PosUVPair* output = new PosUVPair[8];

		//0
		output[0] = PosUVPair(
			glm::vec2(0.0, 0.0),
			glm::vec2(rect.x, rect.y));
		//1
		output[1] = PosUVPair(
			glm::vec2(0.0, 1.0),
			glm::vec2(rect.x, rect.y + rect.height));
		//2
		output[2] = PosUVPair(
			glm::vec2(1.0, 0.0),
			glm::vec2(rect.x + rect.width, rect.y));
		//3
		output[3] = PosUVPair(
			glm::vec2(1.0, 1.0),
			glm::vec2(rect.x + rect.width, rect.y + rect.height));

		//4
		output[4] = PosUVPair(
			glm::vec2(0.0, 0.5),
			glm::vec2(rect.x, rect.y + (rect.height * 0.5)));
		//5
		output[5] = PosUVPair(
			glm::vec2(0.5, 1.0),
			glm::vec2(rect.x + (rect.width * 0.5), rect.y + rect.height));
		//6
		output[6] = PosUVPair(
			glm::vec2(0.5, 0.0),
			glm::vec2(rect.x + (rect.width * 0.5), rect.y));
		//7
		output[7] = PosUVPair(
			glm::vec2(1.0, 0.5),
			glm::vec2(rect.x + rect.width, rect.y + (rect.height * 0.5)));

		//return output;
		for (size_t i = 0; i < 8; i++)
		{
			output[i].uv.y = 1.0f - output[i].uv.y;
		}
	}

	void AppendQuadIndices(MeshIndex_t* indices, int vCount, int & start)
	{
		int start_ = vCount - 4;
		indices[start++] = start_ + 0;
		indices[start++] = start_ + 3;
		indices[start++] = start_ + 1;
		indices[start++] = start_ + 0;
		indices[start++] = start_ + 2;
		indices[start++] = start_ + 3;
	}

	void AppendTriangleIndices(MeshIndex_t* indices, int vCount, int & start)
	{
		int start_ = vCount - 3;
		indices[start++] = start_ + 0;
		indices[start++] = start_ + 2;
		indices[start++] = start_ + 1;
	}

	//globals
	uint8_t* keyStates;

	Camera camera = Camera();
	FloorProgram floorProgram;
	ShadowProgram shadowProgram;
	ColoredVertexProgram coloredVertexProgram;;

	SpriteRect* floorUVRects;
	PosUVMesh* floorMeshes = new PosUVMesh[FloorCount * FloorCount * WallTypeCount * 2];
	//


	PosUVPair* v0TempBuffer8 = new PosUVPair[8];
	PosUVPair* v1TempBuffer8 = new PosUVPair[8];
	MeshIndex_t* iTemp = new MeshIndex_t[9];
	PosUVPair* vTemp = new PosUVPair[8];

	void GenerateMeshes(PosUVMesh* floorMesh, WallType_t type, SpriteRect rightFloor, int none0, SpriteRect leftFloor, int none1)
	{
		if (none0 && none1) {
			return;
		}

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
				AppendQuadIndices(iTemp, vTempCount, iTempCount);
			}
			floorMesh[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			break;
		case WallType::TwoByOne:
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[0];
				vTemp[vTempCount++] = v0TempBuffer8[7];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				AppendTriangleIndices(iTemp, vTempCount, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[7];
				vTemp[vTempCount++] = v1TempBuffer8[3];
				AppendQuadIndices(iTemp, vTempCount, iTempCount);
			}
			floorMesh[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			vTempCount = 0;
			iTempCount = 0;
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[0];
				vTemp[vTempCount++] = v0TempBuffer8[4];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendQuadIndices(iTemp, vTempCount, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[4];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[3];
				AppendTriangleIndices(iTemp, vTempCount, iTempCount);
			}
			floorMesh[1] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			break;
		case WallType::OneByOne:
			//wall type None (simple quad)
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[0];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				AppendTriangleIndices(iTemp, vTempCount, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[3];
				AppendTriangleIndices(iTemp, vTempCount, iTempCount);
			}
			floorMesh[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			break;
		case WallType::OneByTwo:
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[0];
				vTemp[vTempCount++] = v0TempBuffer8[5];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendQuadIndices(iTemp, vTempCount, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[5];
				AppendTriangleIndices(iTemp, vTempCount, iTempCount);
			}
			floorMesh[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			vTempCount = 0;
			iTempCount = 0;
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[6];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				AppendTriangleIndices(iTemp, vTempCount, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[6];
				vTemp[vTempCount++] = v1TempBuffer8[3];
				AppendQuadIndices(iTemp, vTempCount, iTempCount);
			}
			floorMesh[1] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			break;
		case WallType::OneByTwoFlipped:
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[5];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendTriangleIndices(iTemp, vTempCount, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[2];
				vTemp[vTempCount++] = v1TempBuffer8[5];
				AppendQuadIndices(iTemp, vTempCount, iTempCount);
			}
			floorMesh[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			vTempCount = 0;
			iTempCount = 0;
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[6];
				vTemp[vTempCount++] = v0TempBuffer8[1];
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendQuadIndices(iTemp, vTempCount, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[6];
				AppendTriangleIndices(iTemp, vTempCount, iTempCount);
			}
			floorMesh[1] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			break;
		case WallType::OneByOneFlipped:
			//wall type None (simple quad)
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[1];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendTriangleIndices(iTemp, vTempCount, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				vTemp[vTempCount++] = v1TempBuffer8[2];
				AppendTriangleIndices(iTemp, vTempCount, iTempCount);
			}
			floorMesh[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			break;
		case WallType::TwoByOneFlipped:
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[2];
				vTemp[vTempCount++] = v0TempBuffer8[4];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				vTemp[vTempCount++] = v0TempBuffer8[1];
				AppendQuadIndices(iTemp, vTempCount, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[2];
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[4];
				AppendTriangleIndices(iTemp, vTempCount, iTempCount);
			}
			floorMesh[0] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			vTempCount = 0;
			iTempCount = 0;
			if (!none0) {
				vTemp[vTempCount++] = v0TempBuffer8[7];
				vTemp[vTempCount++] = v0TempBuffer8[1];
				vTemp[vTempCount++] = v0TempBuffer8[3];
				AppendTriangleIndices(iTemp, vTempCount, iTempCount);
			}
			if (!none1) {
				vTemp[vTempCount++] = v1TempBuffer8[2];
				vTemp[vTempCount++] = v1TempBuffer8[0];
				vTemp[vTempCount++] = v1TempBuffer8[7];
				vTemp[vTempCount++] = v1TempBuffer8[1];
				AppendQuadIndices(iTemp, vTempCount, iTempCount);
			}
			floorMesh[1] = PosUVMesh(iTemp, iTempCount, vTemp, vTempCount);
			break;
		default:
			return;
		}

	}

	PosUVMesh* GetFloorMesh(FloorType_t floor0, FloorType_t floor1, WallType_t wall, int offset)
	{
		return &floorMeshes[((FloorCount * WallTypeCount * 2) * floor0) + (WallTypeCount * 2 * floor1) + (wall * 2) + offset];

		/*if (!output)
		{
			std::ostringstream print;
			print << "GetFloorMesh: Null floorMesh{ floor0: " << (int)floor0 << ", floor1: " << (int)floor1 << ", wall: " << (int)wall << "}";
			PrintMessage((int)print.str().c_str());
		}

		return output;*/
	}
	void SetFloorMesh(FloorType_t floor0, FloorType_t floor1, WallType_t wall, int offset, PosUVMesh val)
	{
		/*std::ostringstream print;
		print << "SetFloorMesh: { floor0: " << (int)floor0 << ", floor1: " << (int)floor1 << ", wall: " << (int)wall << ", val: " << (int)val << "}";
		PrintMessage((int)print.str().c_str());*/
		floorMeshes[((FloorCount * WallTypeCount * 2) * floor0) + (WallTypeCount * 2 * floor1) + (wall * 2) + offset] = val;
	}

	void GenerateFloorMeshes()
	{
		/*for (int i = 0; i < (FloorCount * FloorCount * WallTypeCount * 2); i++) {
			floorMeshes[i] = NULL;
		}*/

		for (int i = 0; i < FloorCount; i++) {
			for (int j = 0; j < FloorCount; j++) {
				for (int k = 0; k < WallTypeCount; k++) {
					GenerateMeshes(
						GetFloorMesh(i, j, k, 0),
						(WallType_t)k,
						floorUVRects[i],
						i == 0,
						floorUVRects[j],
						j == 0);
				}
			}
		}
	}

	void AppendMeshData(PosUVMesh* floorMesh, std::vector<float> & vertices, std::vector<MeshIndex_t> & indices, glm::vec2 offset)
	{
		if (!floorMesh)
		{
			PrintMessage((int)"AppendMeshData: Null floorMesh");
			return;
		}

		/*std::ostringstream print;
		print << "appending ";
		print << floorMesh->iCount;
		print << " indices";
		PrintMessage((int)print.str().c_str());*/

		MeshIndex_t startI = vertices.size() / 4;
		for (size_t i = 0; i < floorMesh->iCount; i++) {
			indices.push_back(floorMesh->i[i] + startI);
		}


		/*std::ostringstream print0;
		print0 << "appending ";
		print0 << floorMesh->vCount;
		print0 << " floats";
		PrintMessage((int)print0.str().c_str());*/

		for (size_t i = 0; i < floorMesh->vCount; i++) {
			glm::vec2 temp = floorMesh->v[i].pos + offset;
			vertices.push_back(temp.x);
			vertices.push_back(temp.y);
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
			PrintMessage((int)"Error creating shader");
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
			char* logBuffer = new char[1 << 14];
			for (size_t i = 0; i < 1 << 14; i++)
			{
				logBuffer[i] = 0;
			}
			GLsizei ingoLogLength = 0;
			glGetShaderInfoLog(shader, 1 << 14, &ingoLogLength, logBuffer);
			PrintMessage((int)"Shader compilation error");
			PrintMessage((int)logBuffer);
			glDeleteShader(shader);
			return 0;
		}

		return shader;
	}

	GLuint buildProgram(GLuint vertexShader, GLuint fragmentShader)
	{
		//create a GL program and link it
		GLuint programObject = glCreateProgram();
		if (programObject == 0)
		{
			PrintMessage((int)"Program creation error");
			return 0;
		}
		else
		{
			glAttachShader(programObject, vertexShader);
			glAttachShader(programObject, fragmentShader);
			glLinkProgram(programObject);
			return programObject;
		}
	}
}


