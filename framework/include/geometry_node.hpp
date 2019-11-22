#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include "model.hpp"
#include "node.hpp"


class GeometryNode : public Node {

    public:
        GeometryNode(std::string name);
        GeometryNode(std::string name, int depth, float diameter, float rotation_sp, float dist, float rotation_self );
        model getGeometry() const;
        void setGeometry(model geometryModel);
        

    private:
        model geometryModel_;
        std::string name_;
        int depth_;
        float diameter_;
        float rotation_sp_;
        float distance_;
        float rotation_self_;


};

#endif