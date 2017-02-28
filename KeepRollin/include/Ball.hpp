#pragma once

#include "RTR/RTR.h"

class Ball {
    public:
        Ball();
        Ball(float size, ci::vec3 position, ci::vec3 distanceToCamera, ci::vec3 cameraTargetPoint, rtr::MaterialRef m, int t, rtr::MaterialRef occluded);
        void update();
        rtr::NodeRef getNode();
        ci::vec3 getPosition();
        void setPosition(ci::vec3 p);

        ci::vec3 getDistanceToCamera();
        ci::vec3 getCameraTarget();
        float getSize();
    
        rtr::NodeRef node;
        ci::geom::Sphere geometrie;

    private:
        float size = 0.1f;
        ci::vec3 position;
        ci::vec3 distanceToCamera;
        ci::vec3 cameraTargetPoint;
        int textureSize = 512;
        rtr::MaterialRef material;
        rtr::MaterialRef occludedMaterial;
        rtr::ShapeRef shape;
        rtr::ModelRef model;
        glm::mat4 transform;
        float rotation;
};
