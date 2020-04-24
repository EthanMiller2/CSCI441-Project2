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

    return Obj::make_box(glm::vec3(x, y, z), scale, rand_color());
}

int main(int argc, char** argv) {
    // create an image imageWidth pixels wide by imageHeight pixels tall
    // int imageWidth = 320;
    // int imageHeight = 240;
    int imageWidth = 640;
    int imageHeight = 480;

    // set up the images
    bitmap_image image(imageWidth*4, imageHeight*4);
    bitmap_image image2(imageWidth, imageHeight);
    bitmap_image image3(imageWidth, imageHeight);

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
    PointLight l1(glm::vec3(1, 1, 1), glm::vec3(3, -3, 0), 1.0, .7, 0.18);
    DirectionalLight l2(glm::vec3(.5, .5, .5), glm::vec3(-5, 4, -1));
    Lights lights = { &l1, &l2 };

    // setup world
    World world;

    // add the light to the world
    world.append(Sphere(l1.position(), .25, glm::vec3(1,1,1)));

    // create spheres to be added to the world
    world.append(Sphere(glm::vec3(1, 1, 1), 1, rand_color()));
    world.append(Sphere(glm::vec3(2, 2, 4), 2, rand_color()));
    world.append(Sphere(glm::vec3(3, 3, 6), 3, rand_color()));
    world.append(Sphere(glm::vec3(2, 1, 1), 1, rand_color()));
    world.append(Sphere(glm::vec3(5, 6, 4), 2, rand_color()));
    world.append(Sphere(glm::vec3(7, 3, 6), 3, rand_color()));
    world.append(Sphere(glm::vec3(-3, 6, 1), 1, rand_color()));
    world.append(Sphere(glm::vec3(-5, -6, 4), 2, rand_color()));
 
    // create cubes to be added to the world
    std::vector<Triangle> cube1 = Obj::make_box(glm::vec3(3, 3, 3), 3, rand_color());
    std::vector<Triangle> cube2 = Obj::make_box(glm::vec3(-3, 3, 3), 3, rand_color());
    world.append(cube1);
    world.append(cube2);

    // lock the world as no other objects will be added to it
    world.lock();

    // create the intersectors
    BruteForceIntersector intersector1;
    BruteForceIntersector intersector2;
    BruteForceIntersector intersector3;
    BruteForceIntersector intersector4;
    BruteForceIntersector intersector5;
    BruteForceIntersector intersector6;
    BruteForceIntersector intersector7;
    BruteForceIntersector intersector8;

    // Start timer 
    Timer timer;
    timer.start();

    image.save_image("ray-traced.bmp");
    image3.save_image("non-aliased.bmp");
    // Setup the renderers
    Renderer renderer1(&intersector1, image3, camera, lights, world, 1);
    Renderer renderer2(&intersector2, image3, camera, lights, world, 2);
    Renderer renderer3(&intersector3, image3, camera, lights, world, 3);
    Renderer renderer4(&intersector4, image3, camera, lights, world, 4);
    Renderer renderer5(&intersector5, image, camera, lights, world, 1);
    Renderer renderer6(&intersector6, image, camera, lights, world, 2);
    Renderer renderer7(&intersector7, image, camera, lights, world, 3);
    Renderer renderer8(&intersector8, image, camera, lights, world, 4);
    

    // create threads and render images
    pthread_t t1;
    pthread_t t2;
    pthread_t t3;
    pthread_t t4;
    pthread_t t5;
    pthread_t t6;
    pthread_t t7;
    pthread_t t8;
    
    pthread_create(&t1, NULL, &Renderer::r_helper, &renderer1);
    pthread_create(&t2, NULL, &Renderer::r_helper, &renderer2);
    pthread_create(&t3, NULL, &Renderer::r_helper, &renderer3);
    pthread_create(&t4, NULL, &Renderer::r_helper, &renderer4); 
    pthread_create(&t5, NULL, &Renderer::r_helper, &renderer5);
    pthread_create(&t6, NULL, &Renderer::r_helper, &renderer6);
    pthread_create(&t7, NULL, &Renderer::r_helper, &renderer7);
    pthread_create(&t8, NULL, &Renderer::r_helper, &renderer8);

    // ensure that threads have completed the render before 
    // starting anti-aliasing process
    (void) pthread_join(t1, NULL);
    (void) pthread_join(t2, NULL);
    (void) pthread_join(t3, NULL);
    (void) pthread_join(t4, NULL);
    (void) pthread_join(t5, NULL);
    (void) pthread_join(t6, NULL);
    (void) pthread_join(t7, NULL);
    (void) pthread_join(t8, NULL);

    // Anti-aliasing process!
    int image2i = 0;
    int image2j = 0;
    const unsigned int height = image.height();
    const unsigned int width = image.width();

    for(std::size_t i = 0; i < height - 2; i+=2){
        image2j = 0;
        for(std::size_t j = 0; j < width - 2; j+=2){
            rgb_t colour1;
            rgb_t colour2;
            rgb_t colour3;
            rgb_t colour4;

            image.get_pixel(j, i,colour1);
            int red1 = colour1.red;
            int green1 = colour1.green;
            int blue1 = colour1.blue;

            image.get_pixel(j + 1, i,colour2);
            int red2 = colour2.red;
            int green2 = colour2.green;
            int blue2 = colour2.blue;

            image.get_pixel(j, i + 1,colour3);
            int red3 = colour3.red;
            int green3 = colour3.green;
            int blue3 = colour3.blue;

            image.get_pixel(j + 1, i + 1,colour4);
            int red4 = colour4.red;
            int green4 = colour4.green;
            int blue4 = colour4.blue;

            int newRed = (red1 + red2 + red3 + red4) / 4;
            int newGreen = (green1 + green2 + green3 + green4) / 4;
            int newBlue = (blue1 + blue2 + blue3 + blue4) / 4;

            rgb_t newColour = make_colour(newRed, newGreen, newBlue);

            image2.set_pixel(image2j / 2, image2i / 2, newColour);
            image2j +=1;
        }
        image2i +=1;
    }

    // End timer
    timer.stop();

    // save temp image used for AA if you like

    image2.save_image("anti-alias-image.bmp");
    std::cout << "Rendered in " <<  timer.total() << " milliseconds" << std::endl;
}


