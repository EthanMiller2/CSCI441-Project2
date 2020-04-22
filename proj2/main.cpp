#include <iostream> 

#include <glm/glm.hpp>

#include <bitmap/bitmap_image.hpp>

#include "camera.h"
#include "hit.h"
#include "intersector.h"
#include "light.h"
#include "ray.h"
#include "renderer.h"
#include "shape.h"
#include "timer.h"







class AABB
{ 
private:
    float minX;
    float maxX;
    float minY;
    float maxY;

public: 
    AABB(float a, float b, float c, float d) { 
        minX = a;
        maxX = b;
        minY = c;
        maxY = d;



    } 

    bool intersect(const Ray &r){




        float tmin = (minX - r.origin.x) / r.direction.x; 
        float tmax = (maxX - r.origin.x) / r.direction.x; 
 
        if (tmin > tmax) std::swap(tmin, tmax); 
 
        float tymin = (minY - r.origin.y) / r.direction.y; 
        float tymax = (maxY - r.origin.y) / r.direction.y; 
 
        if (tymin > tymax) std::swap(tymin, tymax); 
 
        if ((tmin > tymax) || (tymin > tmax)) 
            return false; 
 
        if (tymin > tmin) 
            tmin = tymin; 
 
        if (tymax < tmax) 
            tmax = tymax; 
 
       
 
    return true; 
 

    } 
       
}; 


class BruteForceIntersector : public Intersector {
public:

    Hit find_first_intersection(const World& world, const Ray& ray) {
        Hit hit(ray);
        for (auto surface : world.shapes()) {
            double cur_t = surface->intersect(ray);

            if (cur_t < hit.t()) {
                hit.update(surface, cur_t);
            }
        }
        return hit;
    }
};

std::vector<AABB> boundingBoxes;
std::vector<AABB> boundingBoxes1;
std::vector<AABB> boundingBoxes2;


class MySlickIntersector : public Intersector {
public:
    Hit find_first_intersection(const World& world, const Ray& ray) {
        Hit hit(ray);
        for(auto box: boundingBoxes2){
            if(box.intersect(ray)){
                for (auto surface : world.shapes()) {
                    double cur_t = surface->intersect(ray);

                    if (cur_t < hit.t()) {
                        hit.update(surface, cur_t);
                    }
                }

            }

        }
        return hit;
    }
};



double rand_val() {
    static bool init = true;
    if (init) {
        srand(time(NULL));
        init = false;
    }
    return ((double) rand() / (RAND_MAX));
}

glm::vec3 rand_color() {
    return glm::vec3(rand_val(),rand_val(),rand_val());
}


std::vector<Triangle> random_box() {
    float  x = (rand_val() * 8) - 4;
    float  y = (rand_val() * 8) - 4;
    float  z = rand_val() * 5;
    float scale = rand_val() * 2;

    float minX = x  + scale * -0.5;
    float maxX = x  + scale * 0.5;
    float minY = y  + scale * -0.5;
    float maxY = y  + scale * 0.5;

    AABB box = AABB(minX, maxX, minY, maxY);
    boundingBoxes2.push_back(box);



    return Obj::make_box(glm::vec3(x, y, z), scale, rand_color());
}

float getMax(float a, float b, float c){
    // std::cout << a << ":" << b << ":" << c << std::endl;
    // std::cout << "Max: " << fmax(fmax(a,b),c) << std::endl;
    return(fmax(fmax(a,b),c));

}

float getMin(float a, float b, float c){
    // std::cout << a << ":" << b << ":" << c << std::endl;
    // std::cout << "Min: " << fmin(fmin(a,b),c) << std::endl;
    return(fmin(fmin(a,b),c));

}

 



int main(int argc, char** argv) {
    

    // set the number of boxes
    int NUM_BOXES = 4;

    // create an image 640 pixels wide by 480 pixels tall
    bitmap_image image(640, 480);

    // setup the camera
    float dist_to_origin = 5;
    Camera camera(
            glm::vec3(0, 0, -dist_to_origin),   // eye
            glm::vec3(0, 0, 0),                 // target
            glm::vec3(0, 1, 0),                 // up
            glm::vec2(-5, -5),                  // viewport min
            glm::vec2(5, 5),                    // viewport max
            dist_to_origin,                     // distance from eye to view plane
            glm::vec3(.3, .6, .8)               // background color
    );

    // setup lights
    // see http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    // for good attenuation value.
    // I found the values at 7 to be nice looking
    PointLight l1(glm::vec3(1, 1, 1), glm::vec3(3, -3, 0), 1.0, .7, 0.18);
    DirectionalLight l2(glm::vec3(.5, .5, .5), glm::vec3(-5, 4, -1));
    Lights lights = { &l1, &l2 };

    // setup world
    World world;

    // add the light
    world.append(Sphere(l1.position(), .25, glm::vec3(1,1,1)));




    //Bounding boxes around spheres for triangle hierarchical bounding boxes and the spheres
    // world.append(Sphere(glm::vec3(1, 1, 1), 1, rand_color()));
    // AABB sphere1 = AABB((1 - 1), (1+1), (1-1), (1+1));
    // boundingBoxes.push_back(sphere1);
    // world.append(Sphere(glm::vec3(2, 2, 4), 2, rand_color()));
    // AABB sphere2 = AABB((2 - 2), (2+2), (2-2), (2+2));
    // boundingBoxes.push_back(sphere2);
    // world.append(Sphere(glm::vec3(3, 3, 6), 3, rand_color()));
    // AABB sphere3 = AABB((3 - 3), (3+3), (3-3), (3+3));
    // boundingBoxes.push_back(sphere3);



    //Bounding boxes around spheres for full box hierarchical bounding boxes
    world.append(Sphere(glm::vec3(1, 1, 1), 1, rand_color()));
    AABB sphere1 = AABB((1 - 1), (1+1), (1-1), (1+1));
    boundingBoxes2.push_back(sphere1);
    world.append(Sphere(glm::vec3(2, 2, 4), 2, rand_color()));
    AABB sphere2 = AABB((2 - 2), (2+2), (2-2), (2+2));
    boundingBoxes2.push_back(sphere2);
    world.append(Sphere(glm::vec3(3, 3, 6), 3, rand_color()));
    AABB sphere3 = AABB((3 - 3), (3+3), (3-3), (3+3));
    boundingBoxes2.push_back(sphere3);


    //Bounding boxes for uniform space partinioning
    AABB hardCode1 = AABB(0, 160, 0, 480);
    boundingBoxes1.push_back(hardCode1);

    AABB hardCode2 = AABB(160, 320, 0, 480);
    boundingBoxes1.push_back(hardCode2);

    AABB hardCode3 = AABB(320, 480, 0, 480);
    boundingBoxes1.push_back(hardCode3);

    AABB hardCode4 = AABB(480, 640, 0, 480);
    boundingBoxes1.push_back(hardCode4);

    // and add some boxes and prep world for rendering
    for (int i = 0 ; i < NUM_BOXES ; ++i) {
        std::vector<Triangle> cube = random_box();
        for(auto t : cube){
            float minX = getMin(t.getA().x, t.getB().x, t.getC().x);
            float maxX = getMax(t.getA().x, t.getB().x, t.getC().x);
            float minY = getMin(t.getA().y, t.getB().y, t.getC().y);
            float maxY = getMax(t.getA().y, t.getB().y, t.getC().y);
            AABB box = AABB(minX, maxX, minY, maxY);
            boundingBoxes.push_back(box);
        }

        world.append(cube);
    }
    world.lock();

    // create the intersector
    BruteForceIntersector intersector;

    MySlickIntersector intersector1;

    // and setup the renderer
    Renderer renderer(&intersector1);

    // render
    Timer timer;
    timer.start();
    renderer.render(image, camera, lights, world);
    timer.stop();

    image.save_image("ray-traced.bmp");
    std::cout << "Rendered in " <<  timer.total() << " milliseconds" << std::endl;
}


