#ifndef POINT_LIGHT_NODE_HPP
#define POINT_LIGHT_NODE_HPP

#include "node.hpp"

class PointLightNode : public Node {

    public:
        PointLightNode(std::string const& name, std::shared_ptr<Node> parent);
        


    private:
        glm::vec3 lightColor_;
        float lightIntensity_;

};

#endif