#include "geometry_node.hpp"

GeometryNode::GeometryNode(std::string name) :
    name_{name} {}

GeometryNode::GeometryNode(std::string name, float diameter, float rotation_self ) {
    name_ = name;
    //depth_ = depth;
    diameter_ = diameter;
    rotation_self_ = rotation_self;
}

model GeometryNode::getGeometry() const {
    return geometryModel_;
}     

void GeometryNode::setGeometry(model geometryModel) {
    geometryModel_ = geometryModel;
}