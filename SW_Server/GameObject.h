#pragma once
namespace SW_Server
{
	class GameObject
	{
	public:
		GameObject(glm::vec2 pos, float rot);
		~GameObject();

		glm::vec2 pos;
		float rot;
	};

}
