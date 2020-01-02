#include "point_light_node.hpp"

PointLightNode::PointLightNode(){}

PointLightNode::PointLightNode(std::string const& name, std::shared_ptr<Node>(parent), glm::vec3 lightColor, float lightInt):
    Node{name,parent},
    lightColor_{lightColor},
    lightIntensity_{lightInt}
    {}

glm::vec3 PointLightNode::getLightCol() {
    return lightColor_;
}

float PointLightNode::getLightInt() {
    return lightIntensity_;
}