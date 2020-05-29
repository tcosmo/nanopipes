#ifndef GRAPHIC_ENGINE_H
#define GRAPHIC_ENGINE_H

#include <string>
#include <math.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "global.h"
#include "routines.h"
#include "world.h"

#define CELL_W 20
#define CELL_H 20

#define DEFAULT_CAM_TRANSLATION 20
#define DEFAULT_CAM_ZOOM_STEP 1.5

#define BACKGROUND_COLOR sf::Color(0.3*255, 0.3*255, 0.3*255)
#define GRID_COLOR sf::Color(0.5*255, 0.5*255, 0.5*255)
#define DEFAULT_CURSOR_COLOR sf::Color(34,220,34)
#define BACKGROUND_COLOR_HALF_DEFINED sf::Color(0.4*255, 0.4*255, 0.4*255)
#define BACKGROUND_COLOR_DEFINED sf::Color(0.6*255, 0.6*255, 0.6*255)
#define COLOR_SPECIAL_CARRY sf::Color(0.4*255, 0.1*255, 0.1*255)

#define GRID_THICKNESS 2

class GraphicEngine {

public:

GraphicEngine(World& world, int screen_w, int screen_h);
~GraphicEngine();

void run();

private:

World& world;

sf::RenderWindow window;
sf::View camera;

int move_camera_mode;
sf::Vector2i mouse_position;

sf::Font default_font;

bool mouse_left;
bool show_grid; 

sf::Vector2i cursor_position;//expressed in world positions
sf::Color cursor_color;
bool insert_mode;

bool cell_color_mode;

void handle_camera_events(sf::Event event);
void handle_insert_events(sf::Event event);

void camera_translate(float dx, float dy);
void camera_translate(const sf::Vector2f& vec);
void camera_zoom(float zoom_factor);
void camera_center(const sf::Vector2f& where);
sf::Vector2f get_camera_view_origin();
bool point_in_camera_view(const sf::Vector2f& point);

void render_grid();
void render_cursor();
void render_world();
void render_cell(const sf::Vector2i& cell_pos, const Cell& cell);

// World coords are cells position
sf::Vector2f map_world_coords_to_coords(const sf::Vector2i& world_coords);
sf::Vector2i map_coords_to_world_coords(const sf::Vector2f& coords);

void move_cursor(sf::Vector2i pos_delta);

void draw_cell_outline_fill(const sf::Vector2i& cell_position, 
                            const sf::Color& outline, const sf::Color& fill);

sf::Vector2i last_visible_cell(); // World coordinates of the left rightmost bottom cell

sf::Color color_of_cell(const Cell& cell);

};

#endif