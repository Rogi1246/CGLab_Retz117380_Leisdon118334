#include "node.hpp"

//Constructors
Node::Node():
    name_{"planet"},
    parent_{nullptr}
 {}

Node::Node(std::string name, std::shared_ptr<Node> parent):
    name_{name},
    parent_{parent},
    worldTransform_{glm::mat4(1.0)}
 {}

//Getter
std::shared_ptr<Node> Node::getParent() const {
    return parent_;
}

std::shared_ptr<Node> Node::getChildren(std::string childName) const {
    auto i = std::find_if(children_.begin(), children_.end(), 
        [&childName]( std::shared_ptr<Node> node) {
        return node->getName() == childName; });
    if(i != children_.end()) {
        return *i;
    } else {
        return nullptr;
    }
}

std::list<std::shared_ptr<Node>> Node::getChildList() {
    return children_;
}

std::string Node::getName() const {
    return name_;
}

std::string Node::getPath() const {
    return path_;
}

int Node::getDepth() const {
    return depth_;
}

glm::mat4 Node::getLocalTransform() const {
    return localTransform_;
}

glm::mat4 Node::getWorldTransform() const {
    return worldTransform_;
}
        

float Node::getRotationSpeed() const {
    return rotation_sp_;
}

float Node::getDistance() const {
    return distance_;
}

float Node::getSize() const {
    return size_;
}
bool Node::getIsMoon() const{
    return is_moon_;
}

int Node::getIsSun() const{
    return isSun_;
}

//Setter
void Node::setParent(std::shared_ptr<Node> parent) {
    parent_ = parent;
}

void Node::setLocalTransform(glm::mat4 localTrans) {
    localTransform_ = localTrans;
}

void Node::setWorldTransform(glm::mat4 worldTrans) {
    worldTransform_ = worldTrans;
}

void Node::setRotationSpeed(float rotation_sp) {
    rotation_sp_ = rotation_sp;
}

void Node::setDistance(float distance) {
    distance_ = distance;
}

void Node::setSize(float size) {
    size_ = size;
}
void Node::setIsMoon(bool is_moon) {
    is_moon_ = is_moon;
}
void Node::setIsSUn(int is_sun){
    isSun_ = is_sun;
}

//Adding and removing children
void Node::addChild(std::shared_ptr<Node> child) {
    children_.push_back(child);
}

std::shared_ptr<Node> Node::removeChild(std::string childName) {
    auto i = std::find_if(children_.begin(), children_.end(), [&childName](std::shared_ptr<Node> foundName) {
        return (foundName->getName() == childName);});
    if(i != children_.end()) {
        children_.remove(*i);
        return *i;
    } else {
        return nullptr;
    }
}


    
        

        


