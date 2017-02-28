#include "Item.hpp"

Item::Item() {
};

Item::Item(ci::vec3 p, float s, rtr::MaterialRef m, int t, int b, rtr::MaterialRef shadow, rtr::MaterialRef occluded) {
    size = s;
    position = p;
    material = m;
    shadowMaterial = shadow;
    occludedMaterial = occluded;
    textureSize = t;
    boost = b;
    geometrie = ci::geom::Torus().radius(size+0.01f, size).subdivisionsAxis(textureSize).subdivisionsHeight(textureSize);
    shape = rtr::Shape::create({geometrie}, material);
    shape->setMaterialForPass("shadow", shadowMaterial);
    shape->setMaterialForPass("scatter", occludedMaterial);
    model = rtr::Model::create({shape});
    transform = translate(transform, position);
    transform = glm::rotate(transform, glm::radians(-90.0f), ci::vec3(1, 0, 0));
    node = rtr::Node::create({model}, transform);
};
