#pragma once

#include "RTR/RTR.h"

class Block {
    public:
        Block();
        Block(ci::vec3 position, ci::vec3 size, rtr::MaterialRef m, int textureSize, rtr::MaterialRef shadow, rtr::MaterialRef occluded);
        rtr::NodeRef node;

    private:
        ci::vec3 size = ci::vec3(0.1f);
        ci::vec3 position = ci::vec3(0);
        rtr::MaterialRef material;
        rtr::MaterialRef shadowMaterial;
        rtr::MaterialRef occludedMaterial;
        int textureSize = 512;
        ci::geom::Cube geometrie;
        rtr::ShapeRef shape;
        rtr::ModelRef model;
        glm::mat4 transform;
};
