#include "camera_node.hpp"
#include "node.hpp"

CameraNode::CameraNode(std::string const& name, std::shared_ptr<Node> parent) {
    Node{name, parent};
}

bool CameraNode::getPerspective(){
    return isPerspective_;
}
        
bool CameraNode::getEnabled(){
    return isEnabled_;
}
        
void CameraNode::setEnabled(bool isEnabled){
    isEnabled_ = isEnabled;
}

glm::mat4 CameraNode::getProjectionMatrix(){
    return projectionMatrix_;
}

void CameraNode::setProjectionMatrix(glm::mat4 projectionMatrix){
    projectionMatrix_ = projectionMatrix;
}