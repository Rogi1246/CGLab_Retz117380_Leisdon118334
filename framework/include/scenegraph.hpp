#include "node.hpp"
#include <iostream>


class Scenegraph{
    public:
    Scenegraph();
    Scenegraph(std::string name, std::shared_ptr<Node> const& root);
    
    std::string getName() const;
    std::shared_ptr<Node> getRoot() const;
    
    void setName(std::string name);
    void setRoot(std::shared_ptr<Node>);



    private:
        std::string name_;
        std::shared_ptr<Node> root_;

};