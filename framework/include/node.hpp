#ifndef NODE_HPP
#define NODE_HPP

#include <list>
#include <string>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <memory>


class Node{

    public:
        int getNode();
        
    private:
        std::shared_ptr<Node> parent_;
        std::list<std::shared_ptr<Node>> children_;
        std::string name_;
        std::string path_;
        glm::mat4 localTransform_;
        glm::mat4 worldTransform_;


};

#endif 