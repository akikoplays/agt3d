#include "agt_node.h"
#include "agt_stdafx.h"

namespace agt3d
{

Node::Node() {}

Node::Node(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
    : localPos(pos), localRot(rot), localScale(scale)
{
}

Node::Node(glm::vec3&& pos, glm::quat&& rot, glm::vec3&& scale)
    : localPos(pos), localRot(rot), localScale(scale)
{
}

void Node::setLocalPosition(const glm::vec3& pos) { localPos = pos; }

void Node::setLocalRotation(const glm::quat& rot) { localRot = rot; }

void Node::setLocalScale(const glm::vec3& scale) { localScale = scale; }

}  // namespace agt3d
