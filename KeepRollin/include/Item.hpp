#pragma once

#include "RTR/RTR.h"

class Item {
    public:
        Item();
        Item(ci::vec3 position, float size, rtr::MaterialRef m, int textureSize, int boost, rtr::MaterialRef shadow, rtr::MaterialRef occluded);
        rtr::NodeRef node;
        int boost = 0;
        float size = 0;
        ci::vec3 position = ci::vec3(0);
    
    private:
        rtr::MaterialRef material;
        rtr::MaterialRef shadowMaterial;
        rtr::MaterialRef occludedMaterial;
        int textureSize = 1024;
        ci::geom::Torus geometrie;
        rtr::ShapeRef shape;
        rtr::ModelRef model;
        glm::mat4 transform;
};
