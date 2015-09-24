#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace SW_Client
{
	Camera::Camera()
	{
		zoom = 1.0f / 4.0f;
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		dim = glm::vec2(1.0f, 1.0f);
		fov = glm::pi<float>() * 0.3333f;
		rot = glm::vec2(0.0f, 0.0f);
		nearZ = 0.001f;
		farZ = 1000.0f;
	}
	Camera::~Camera()
	{

	}

	void Camera::GenerateView(glm::mat4 & viewMat)
	{
		//viewMat = glm::rotate(viewMat, glm::pi<float>() * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
		viewMat = glm::rotate(viewMat, -rot.y, glm::vec3(1.0f, 0.0f, 0.0f));
		viewMat = glm::rotate(viewMat, -rot.x, glm::vec3(0.0f, 1.0f, 0.0f));
		viewMat = glm::translate(viewMat, -pos);
	}
	void Camera::GenerateProjection(glm::mat4 & projMat)
	{
		projMat = glm::perspectiveFovLH(fov, dim.x, dim.y, nearZ, farZ);
	}
}
