#ifndef _CSCI441_RENDERER_H_
#define _CSCI441_RENDERER_H_

#include "camera.h"
#include "hit.h"
#include "light.h"
#include "ray.h"
#include "intersector.h"

class Renderer {
    Intersector* _intersector;

    int clamp255(float v) {
        return std::max(0.0f, std::min(255.0f, 255.0f*v));
    }


    rgb_t to_color(const glm::vec3 c) {
        return make_colour(clamp255(c.x), clamp255(c.y), clamp255(c.z));
    }


    glm::vec3 phong(const Hit& hit, const Light* light, const glm::vec3& eye) {

        float ambientStrength = 0.2;
        float specularStrength = 0.5;
        float shinyness = 2;

        glm::vec3 pos = hit.position();
        glm::vec3 normal = hit.normal();
        

        glm::vec3 light_dir = glm::normalize(light->direction(pos));

        float ambient = ambientStrength;

        float diffuse = glm::max(glm::dot(normal, light_dir), 0.0f);

        glm::vec3 view_dir = glm::normalize(eye - pos);
        glm::vec3 reflectDir = glm::reflect(-light_dir, normal);
        float specular = specularStrength *
            pow(std::fmax(glm::dot(view_dir, reflectDir), 0.0), shinyness);

        glm::vec3 light_color  =
            (ambient+diffuse+specular)
            * light->attenuation(pos)
            * light->color();

        return light_color*hit.color();
    }


    glm::vec3 shade(const Camera& camera, const Lights& lights, const Hit& hit) {
        glm::vec3 color = camera.background;
        if (hit.is_intersection()) {
            color = glm::vec3(0,0,0);
            for (auto light : lights) {
                color += phong(hit, light, camera.pos);
            }
        }
        return color;
    }

    glm::vec3 render_pixel(
        const Camera& camera,
        const Lights& lights,
        const World& world,
        const Ray& ray
    ) {
        Hit hit = _intersector->find_first_intersection(world, ray);
        return shade(camera, lights, hit);
    }

public:
    int number; 
    bitmap_image& imageG;
    Camera& cameraG;
    Lights& lightsG;
    World& worldG;

    void *render(void){   
        if(number == 1){
            for (int y = 0; y < imageG.height()/4; ++y) {
                for (int x = 0; x < imageG.width(); ++x) {
                    Ray ray = cameraG.make_ray(imageG.width(), imageG.height(), x, y);
                    glm::vec3 c = render_pixel(cameraG, lightsG, worldG, ray);
                    imageG.set_pixel(x, imageG.height()-y-1, to_color(c));
                }
            }
        } else if(number == 2){
            for (int y = imageG.height()/4; y < imageG.height()/4*2; ++y) {
                for (int x = 0; x < imageG.width(); ++x) {
                    Ray ray = cameraG.make_ray(imageG.width(), imageG.height(), x, y);
                    glm::vec3 c = render_pixel(cameraG, lightsG, worldG, ray);
                    imageG.set_pixel(x, imageG.height()-y-1, to_color(c));
                }
            }
        } else if(number == 3){
            for (int y = imageG.height()/4*2; y < imageG.height()/4*3; ++y) {
                for (int x = 0; x < imageG.width(); ++x) {
                     Ray ray = cameraG.make_ray(imageG.width(), imageG.height(), x, y);
                    glm::vec3 c = render_pixel(cameraG, lightsG, worldG, ray);
                    imageG.set_pixel(x, imageG.height()-y-1, to_color(c));
                }
            }
        } else {
            for (int y = imageG.height()/4*3; y < imageG.height(); ++y) {
                for (int x = 0; x < imageG.width(); ++x) {
                     Ray ray = cameraG.make_ray(imageG.width(), imageG.height(), x, y);
                    glm::vec3 c = render_pixel(cameraG, lightsG, worldG, ray);
                    imageG.set_pixel(x, imageG.height()-y-1, to_color(c));
                }
            }
        }
       
        return 0;
    }

    static void *r_helper(void *context){
        return ((Renderer *)context)->render();
    }

    Renderer(
        Intersector* intersector,  
        bitmap_image& image,
        Camera& camera,
        Lights& lights,
        World& world,
        int num) : _intersector(intersector), imageG(image), cameraG(camera), lightsG(lights), worldG(world), number(num) { }
};

#endif
