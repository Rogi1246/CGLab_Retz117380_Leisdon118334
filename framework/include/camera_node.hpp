#ifndef CAMERA_NODE_HPP
#define CAMERA_NODE_HPP

#include "node.hpp"

class CameraNode {

    public:
        CameraNode(std::string const& name, std::shared_ptr<Node> parent);
        bool getPerspective();
        bool getEnabled();
        void setEnabled(bool isEnabled);
        glm::mat4 getProjectionMatrix();
        void setProjectionMatrix(glm::mat4 projectionMatrix);


    private:
        bool isPerspective_;
        bool isEnabled_;
        glm::mat4 projectionMatrix_;


};

#endif