#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include "model.hpp"
#include "node.hpp"


class GeometryNode : public Node {

    public:
        GeometryNode();
        GeometryNode(std::string name);
        GeometryNode(std::string name, std::shared_ptr<Node>(parent));
        model getGeometry() const;
        glm::vec3 getPlanetColor() const;
        void setGeometry(model geometryModel);
        void setPlanetColor(glm::vec3 planet_color);

    private:
        model geometryModel_;
        std::string name_;
        //int depth_;
        float diameter_;
        float rotation_self_;
        glm::vec3 planet_color_;


};

#endif