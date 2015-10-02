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
#include <iostream>
#include <fstream>
#include "BufferedMeshArray.h"
#include "MeshConstructor.h"
#include "MCPlane.h"

std::string readFile(const char *filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

using namespace std;

namespace SW_Client
{
	void AppendVertex(std::vector<float> & vertices, glm::vec3 pos, glm::vec3 normal, glm::vec4 color)
	{
		vertices.push_back(pos.x);
		vertices.push_back(pos.y);
		vertices.push_back(pos.z);
		vertices.push_back(1.0f);
		vertices.push_back(normal.x);
		vertices.push_back(normal.y);
		vertices.push_back(normal.z);
		vertices.push_back(0.0f);
		vertices.push_back(color.x);
		vertices.push_back(color.y);
		vertices.push_back(color.z);
		vertices.push_back(1.0f);
	}

	void GenerateWallMeshes()
	{

		std::vector<float> vertices;
		std::vector<MeshIndex_t> indices;

		glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

		const float WALL_CORNER_OFF = 0.0f;
		const float WALL_HEIGHT = 1.7f;
		const float WALL_BACK = 0.01f;
		const float WALL_BEVEL_X = 0.06f;
		const float WALL_BEVEL_Y = 0.1f;


		for (size_t i = 0; i < 3; i++)
		{
			MeshConstructor mc;

			float mag = SW::wallMagnitudes[WallType::OneByZero];
			if (i == 1)
			{
				mag = SW::wallMagnitudes[WallType::OneByZero];
			}
			else if (i == 2)
			{
				mag = SW::wallMagnitudes[WallType::OneByOne];
			}
			else if (i == 0)
			{
				mag = SW::wallMagnitudes[WallType::TwoByOne];
			}

			const float KEY_BOT = mag;
			const float KEY_TOP = mag;

			vector<glm::vec3> keyPoints;
			keyPoints.push_back(glm::vec3(WALL_CORNER_OFF, 0.0f, -WALL_BEVEL_X - WALL_BACK));
			keyPoints.push_back(glm::vec3(mag - WALL_CORNER_OFF, 0.0f, -WALL_BEVEL_X - WALL_BACK));
			keyPoints.push_back(glm::vec3(WALL_CORNER_OFF, WALL_BEVEL_Y, -WALL_BACK));
			keyPoints.push_back(glm::vec3(mag - WALL_CORNER_OFF, WALL_BEVEL_Y, -WALL_BACK));
			keyPoints.push_back(glm::vec3(WALL_CORNER_OFF, WALL_HEIGHT - WALL_BEVEL_Y, -WALL_BACK));
			keyPoints.push_back(glm::vec3(mag - WALL_CORNER_OFF, WALL_HEIGHT - WALL_BEVEL_Y, -WALL_BACK));
			keyPoints.push_back(glm::vec3(WALL_CORNER_OFF, WALL_HEIGHT, -WALL_BEVEL_X - WALL_BACK));
			keyPoints.push_back(glm::vec3(mag - WALL_CORNER_OFF, WALL_HEIGHT, -WALL_BEVEL_X - WALL_BACK));

			glm::vec3 zero(0.0f, 0.0f, 0.0f);
			glm::vec3 up(0.0f, CEILING_HEIGHT, 0.0f);
			glm::vec3 right(mag, 0.0f, 0.0f);

			for (size_t j = 0; j < keyPoints.size(); j++)
			{
				mc.v.push_back(keyPoints[j]);
			}

			/*mc.v.push_back(MCVertex(zero));
			mc.v.push_back(MCVertex(up));
			mc.v.push_back(MCVertex(right));
			mc.v.push_back(MCVertex(up + right));*/

			mc.AddQuad(0, 2, 1, 3);
			mc.AddQuad(2, 4, 3, 5);
			mc.AddQuad(4, 6, 5, 7);

			mc.DuplicateVertices();
			mc.RecomputeNormals();

			for (size_t i = 0; i < mc.v.size(); i++)
			{
				AppendVertex(vertices, mc.v[i].pos, mc.v[i].normal, color);
			}

			MeshIndex_t start;

			wallMeshes->meshes[(i * 2)].offset = indices.size();
			wallMeshes->meshes[(i * 2)].count = mc.t.size() * 3;
			start = (vertices.size() / 12);
			for (size_t i = 0; i < mc.t.size(); i++)
			{
				indices.push_back(start + mc.t[i].i[0]);
				indices.push_back(start + mc.t[i].i[1]);
				indices.push_back(start + mc.t[i].i[2]);
			}
			wallMeshes->meshes[(i * 2) + 1].offset = indices.size();
			wallMeshes->meshes[(i * 2) + 1].count = mc.t.size() * 3;
			start = (vertices.size() / 12);
			for (size_t i = 0; i < mc.t.size(); i++)
			{
				indices.push_back(start + mc.t[i].i[2]);
				indices.push_back(start + mc.t[i].i[1]);
				indices.push_back(start + mc.t[i].i[0]);
			}
		}


		wallMeshes->Bind();
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), (GLvoid*)&vertices[0], GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshIndex_t) * indices.size(), (GLvoid*)&indices[0], GL_STATIC_DRAW);
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


	void InitializeClient()
	{
		SW::Initialize();

		wallMeshes = new BufferedMeshArray(6);
		cornerFloorMeshes = new BufferedMeshArray(4);
		GenerateWallMeshes();

		keyStates = SDL_GetKeyboardState(NULL);

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

		glFinish();

		floorProgram.program = LoadProgram("data/shaders/floor.vs", "data/shaders/floor.fs");
		floorProgram.viewMat = glGetUniformLocation(floorProgram.program, "viewMat");
		floorProgram.projMat = glGetUniformLocation(floorProgram.program, "projMat");
		floorProgram.posAttrib = glGetAttribLocation(floorProgram.program, "floorPos");
		floorProgram.uvAttrib = glGetAttribLocation(floorProgram.program, "floorUV");
		floorProgram.textureLoc = glGetUniformLocation(floorProgram.program, "floorTexture");

		shadowProgram.program = LoadProgram("data/shaders/shadow.vs", "data/shaders/shadow.fs");
		shadowProgram.viewMat = glGetUniformLocation(shadowProgram.program, "viewMat");
		shadowProgram.playerPos = glGetUniformLocation(shadowProgram.program, "playerPos");
		shadowProgram.worldPosAttrib = glGetAttribLocation(shadowProgram.program, "worldPos");
		shadowProgram.shadowAttrib = glGetAttribLocation(shadowProgram.program, "shadowAttrib");

		coloredVertexProgram.program = LoadProgram("data/shaders/colored_vertex.vs", "data/shaders/colored_vertex.fs");
		coloredVertexProgram.viewMat = glGetUniformLocation(coloredVertexProgram.program, "viewMat");
		coloredVertexProgram.projMat = glGetUniformLocation(coloredVertexProgram.program, "projMat");
		coloredVertexProgram.objMat = glGetUniformLocation(coloredVertexProgram.program, "objMat");
		coloredVertexProgram.posAttrib = glGetAttribLocation(coloredVertexProgram.program, "position");
		coloredVertexProgram.normalAttrib = glGetAttribLocation(coloredVertexProgram.program, "normal");
		coloredVertexProgram.colorAttrib = glGetAttribLocation(coloredVertexProgram.program, "color");

		litColoredVertexProgram.program = LoadProgram("data/shaders/lit_colored_vertex.vs", "data/shaders/lit_colored_vertex.fs");
		litColoredVertexProgram.lightIntensity = glGetUniformLocation(litColoredVertexProgram.program, "lightIntensity");
		litColoredVertexProgram.lightPosition = glGetUniformLocation(litColoredVertexProgram.program, "lightPosition");
		litColoredVertexProgram.viewMat = glGetUniformLocation(litColoredVertexProgram.program, "viewMat");
		litColoredVertexProgram.projMat = glGetUniformLocation(litColoredVertexProgram.program, "projMat");
		litColoredVertexProgram.objMat = glGetUniformLocation(litColoredVertexProgram.program, "objMat");
		litColoredVertexProgram.posAttrib = glGetAttribLocation(litColoredVertexProgram.program, "position");
		litColoredVertexProgram.normalAttrib = glGetAttribLocation(litColoredVertexProgram.program, "normal");
		litColoredVertexProgram.colorAttrib = glGetAttribLocation(litColoredVertexProgram.program, "color");

		moduleProgram.program = LoadProgram("data/shaders/module.vs", "data/shaders/module.fs");
		moduleProgram.viewMat = glGetUniformLocation(moduleProgram.program, "viewMat");
		moduleProgram.projMat = glGetUniformLocation(moduleProgram.program, "projMat");
		moduleProgram.objMat = glGetUniformLocation(moduleProgram.program, "objMat");
		moduleProgram.posAttrib = glGetAttribLocation(moduleProgram.program, "position");
		moduleProgram.colorAttrib = glGetAttribLocation(moduleProgram.program, "color");
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
	ColoredVertexProgram coloredVertexProgram;
	LitColoredVertexProgram litColoredVertexProgram;
	ModuleProgram moduleProgram;

	BufferedMeshArray* wallMeshes = NULL;
	BufferedMeshArray* cornerFloorMeshes = NULL;
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

	GLuint LoadProgram(const char *vs, const char *fs)
	{
		std::string shaderSource;

		shaderSource = readFile(vs);
		GLuint vShader = LoadShader(GL_VERTEX_SHADER, shaderSource.c_str());
		shaderSource = readFile(fs);
		GLuint fShader = LoadShader(GL_FRAGMENT_SHADER, shaderSource.c_str());

		glFinish();
		GLuint output = BuildProgram(vShader, fShader);

		GLint linked;
		glGetProgramiv(output, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			std::ostringstream print;
			print << "Program Link error: \n";
			print << "VS: " << vs << "\n";
			print << "FS: " << fs << "\n";
			PrintMessage((int)print.str().c_str());
			glDeleteProgram(output);
			return 0;
		}
		else
		{
			return output;
		}
	}
	GLuint LoadShader(GLenum type, const char *source)
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

	GLuint BuildProgram(GLuint vertexShader, GLuint fragmentShader)
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


