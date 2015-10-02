#include "RigidBody.h"

namespace SW
{
	RigidBody::RigidBody(glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 com, glm::vec3 rot) : GameObject(pos, rot)
	{
		RigidBody::angular_vel = 0.0f;
		RigidBody::vel = vel;
		RigidBody::m = m;
		RigidBody::com = com;
	}
	RigidBody::~RigidBody()
	{

	}

	float RigidBody::mass()
	{
		return m;
	}
	float RigidBody::massInv()
	{
		return 1.0f / m;
	}
	glm::vec3 RigidBody::velocity()
	{
		return vel;
	}
	void RigidBody::ApplyForceXZ(glm::vec2 f)
	{
		float scale = (elapsedTime / m);
		vel += glm::vec3(f.x * scale, 0.0f, f.y * scale);
	}
	void RigidBody::ApplyForceXZ(glm::vec2 local, glm::vec2 f)
	{
		float scale = (elapsedTime / m);
		angular_vel += glm::cross(glm::vec3(local.x, 0.0f, local.y), glm::vec3(f.x, 0.0f, f.y)).y * scale;
		vel += glm::vec3(f.x * scale, 0.0f, f.y * scale);
	}
	void RigidBody::StepXZ()
	{
		pos.x += vel.x * elapsedTime;
		pos.z += vel.z * elapsedTime;
		rot.x += angular_vel * elapsedTime;
	}

}

