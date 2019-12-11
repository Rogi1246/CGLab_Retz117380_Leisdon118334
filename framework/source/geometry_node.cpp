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

void GeometryNode::setGeometry(model geometryModel) {
    geometryModel_ = geometryModel;
}