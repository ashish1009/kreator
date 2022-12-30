#include "scene.hpp"

void Scene::Clear() {
    objects.clear();
    objects.shrink_to_fit();
}

void Scene::Add(std::shared_ptr<Shape> object) {
    objects.push_back(object);
}

bool Scene::Intersect(const Ray_ &r, SurfaceInteraction &interaction) const {
    
    SurfaceInteraction temp;
    bool any_hit = false;
    float t_near = r.t_max;
    
    for (const auto& object : objects) {
        if (object->Intersect(r, temp)) {
            any_hit = true;
            t_near = temp.t;
            interaction = temp;
        }
    }
    return any_hit;
}
