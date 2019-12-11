#ifndef NODE_HPP
#define NODE_HPP

#include <list>
#include <string>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <algorithm> //for find_if
#include <iostream>
#include <memory>


class Node{

    public:
        //Constructors
        Node();
        Node(std::string name, std::shared_ptr<Node> parent);
        
        //Getter
        std::shared_ptr<Node> getParent() const;
        std::shared_ptr<Node> getChildren(std::string childName) const;
        std::list<std::shared_ptr<Node>> getChildList();
        std::string getName() const;
        std::string getPath() const;
        int getDepth() const;
        glm::mat4 getLocalTransform() const;
        glm::mat4 getWorldTransform() const;
        float getRotationSpeed() const;
        float getDistance() const;
        float getSize() const;


        //Setter
        void setParent(std::shared_ptr<Node> parent);
        void setLocalTransform(glm::mat4 localTrans);
        void setWorldTransform(glm::mat4 worldTrans);
        void setRotationSpeed(float rotation_sp);
        void setDistance(float distance);
        void setSize(float size);


        //Adding and removing children
        void addChild(std::shared_ptr<Node> child);
        std::shared_ptr<Node> removeChild(std::string childName);
        
    private:
        std::shared_ptr<Node> parent_;
        std::list<std::shared_ptr<Node>> children_;
        std::list<std::shared_ptr<Node>> childrenList_;
        std::string name_;
        std::string path_;
        int depth_;
        glm::mat4 localTransform_;
        glm::mat4 worldTransform_;
        float rotation_sp_;
        float distance_;
        float size_;

};
#endif 