#pragma once
namespace agt3d {

	class Node
	{
	public:
		Node();
		Node(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale);
		Node(glm::vec3&& pos, glm::quat&& rot, glm::vec3&& scale);
		void setLocalPosition(const glm::vec3& pos);
		void setLocalRotation(const glm::quat& rot);
		void setLocalScale(const glm::vec3& scale);

	public:
		glm::vec3 localPos = { 0,0,0 };
		glm::quat localRot = { 1,0,0,0 };
		glm::vec3 localScale = { 1,1,1 };
	};
}

