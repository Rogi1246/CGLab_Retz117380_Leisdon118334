#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include "model.hpp"
#include "node.hpp"


class GeometryNode : public Node {

    public:
        GeometryNode(std::string const& name);
        model getGeometry() const;
        void setGeometry(model geometryModel);
        

    private:
        model geometryModel_;


};

#endif