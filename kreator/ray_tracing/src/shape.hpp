#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "math.hpp"

class Shape {
public:
    virtual bool intersect(const Ray_& r, SurfaceInteraction& interaction) const = 0;
};

#endif
