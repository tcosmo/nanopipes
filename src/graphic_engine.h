#ifndef GRAPHIC_ENGINE_H
#define GRAPHIC_ENGINE_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "global.h"

#define CELL_W 20
#define CELL_H 20

#define DEFAULT_CAM_TRANSLATION 20

#define BACKGROUND_COLOR sf::Color(0.3*255, 0.3*255, 0.3*255)
#define GRID_COLOR sf::Color(0.5*255, 0.5*255, 0.5*255)

class GraphicEngine {

public:

GraphicEngine(int screen_w, int screen_h);
~GraphicEngine();

void run();

private:

sf::RenderWindow window;
sf::View camera;

void render_grid(int grid_w, int grid_h, int thickness = 2);
void translate_camera(float dx, float dy);

};

#endif