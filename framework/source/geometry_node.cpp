#include "geometry_node.hpp"

GeometryNode::GeometryNode(std::string name) {
    name_ = name;
}

GeometryNode::GeometryNode(std::string name, int depth, float diameter, float rotation_sp, float dist, float rotation_self ) {
    name_ = name;
    depth_ = depth;
    diameter_ = diameter;
    rotation_sp_ = rotation_sp;
    distance_ = dist;
    rotation_self_ = rotation_self;
}

model GeometryNode::getGeometry() const {
    return geometryModel_;
}     

void GeometryNode::setGeometry(model geometryModel) {
    geometryModel_ = geometryModel;
}