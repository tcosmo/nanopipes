#ifndef GRAPHIC_ENGINE_H
#define GRAPHIC_ENGINE_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "global.h"
#include "routines.h"

#define CELL_W 20
#define CELL_H 20

#define DEFAULT_CAM_TRANSLATION 20
#define DEFAULT_CAM_ZOOM_STEP 1.5

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

int move_camera_mode;
sf::Vector2i mouse_position;

bool mouse_left;

void handle_camera_events(sf::Event event);
void render_grid(int grid_w, int grid_h, int thickness = 2);
void camera_translate(float dx, float dy);
void camera_translate(const sf::Vector2f& vec);
void camera_zoom(float zoom_factor);

bool point_in_camera_view(const sf::Vector2f& point);

};

#endif