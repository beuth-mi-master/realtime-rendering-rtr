#include "Block.hpp"

Block::Block() {
};

Block::Block(ci::vec3 p, ci::vec3 s, rtr::MaterialRef m, int t, rtr::MaterialRef shadowM, rtr::MaterialRef occludedM) {
    size = s;
    position = p;
    material = m;
    shadowMaterial = shadowM;
    occludedMaterial = occludedM;
    textureSize = t;
    geometrie = ci::geom::Cube().subdivisions(textureSize).size(size);
    shape = rtr::Shape::create({geometrie}, material);
    shape->setMaterialForPass("shadow", shadowMaterial);
    shape->setMaterialForPass("scatter", occludedMaterial);
    model = rtr::Model::create({shape});
    transform = translate(transform, position);
    node = rtr::Node::create({model}, transform);
};
