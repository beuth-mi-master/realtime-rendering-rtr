#include "Ball.hpp"

Ball::Ball(float s, ci::vec3 p, ci::vec3 d, ci::vec3 ct, rtr::MaterialRef m, int t, rtr::MaterialRef occluded) {
    size = s;
    position = p;
    distanceToCamera = d;
    textureSize = t;
    material = m;
    cameraTargetPoint = ct;
    occludedMaterial = occluded;

    geometrie = ci::geom::Sphere().subdivisions(textureSize).radius(size);
    shape = rtr::Shape::create({geometrie}, material);
    shape->setMaterialForPass("scatter", occludedMaterial);
    model = rtr::Model::create({shape});
    transform = translate(transform, position);
    node = rtr::Node::create({model}, transform);
}

Ball::Ball() {
}

void Ball::update() {
    
}

float Ball::getSize() {
    return size;
}

void Ball::setPosition(ci::vec3 p) {
    ci::mat4 t;
    t = translate(t, p);
    node->transform = t;
    position = p;
}

ci::vec3 Ball::getPosition() {
    return position;
}

ci::vec3 Ball::getDistanceToCamera() {
    return distanceToCamera;
}

ci::vec3 Ball::getCameraTarget(){
    return cameraTargetPoint;
}
