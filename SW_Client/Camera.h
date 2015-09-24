#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace SW_Client
{
	class Camera
	{
	public:
		float zoom;
		glm::vec2 dim;
		glm::vec2 rot;
		glm::vec3 pos;
		float fov;
		float farZ;
		float nearZ;

		Camera();
		~Camera();

		void GenerateView(glm::mat4 & viewMat);
		void GenerateProjection(glm::mat4 & projMat);
	};
}

