#include "geometry_node.hpp"

GeometryNode::GeometryNode():
    Node{"default",nullptr}
{}

GeometryNode::GeometryNode(std::string name) :
    Node{name, nullptr}
    {}

GeometryNode::GeometryNode(std::string name, std::shared_ptr<Node>(parent)):
    Node{name,parent}
    {}

model GeometryNode::getGeometry() const {
    return geometryModel_;
}     

glm::vec3 GeometryNode::getPlanetColor() const {
    return planet_color_;
}

texture_object GeometryNode::getPlanetTexture() const{
    return planet_Texture_;
}

void GeometryNode::setGeometry(model geometryModel) {
    geometryModel_ = geometryModel;
}

void GeometryNode::setPlanetColor(glm::vec3 planetColor){
    planet_color_ = planetColor;
}

void GeometryNode::setPlanetTexture(texture_object const& texture) {
    planet_Texture_ = texture;
}
