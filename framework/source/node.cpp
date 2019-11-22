#include "node.hpp"

//Constructors
Node::Node() {}

Node::Node(std::string name, std::shared_ptr<Node> parent) {
    name = name_;
    parent = parent_;
    worldTransform_ = glm::mat4(1.0);
}

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
        
//Setter
void Node::setParent(std::shared_ptr<Node> parent) {
    parent = parent_;
}

void Node::setLocalTransform(glm::mat4 localTrans) {
    localTrans = localTransform_;
}

void Node::setWorldTransform(glm::mat4 worldTrans) {
    worldTrans = worldTransform_;
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


    
        

        


