#include "scenegraph.hpp"


Scenegraph::Scenegraph():
    name_{},
    root_{}{}

Scenegraph::Scenegraph(std::string name, std::shared_ptr<Node> root):
    name_{name},
    root_{root}{}

std::string Scenegraph::getName() const{
    return name_;
}

std::shared_ptr<Node> Scenegraph::getRoot() const{
    return root_;
}

void Scenegraph::setName(std::string name){
    name_ = name;
}

void Scenegraph::setRoot(std::shared_ptr<Node> root) {
    root_ = root;
}
