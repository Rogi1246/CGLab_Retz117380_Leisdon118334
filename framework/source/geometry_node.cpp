#include "geometry_node.hpp"

GeometryNode::GeometryNode(std::string const& name) {

}

model GeometryNode::getGeometry() const {
    return geometryModel_;
}     

void GeometryNode::setGeometry(model geometryModel) {
    geometryModel_ = geometryModel;
}