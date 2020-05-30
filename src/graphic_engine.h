#ifndef GRAPHIC_ENGINE_H
#define GRAPHIC_ENGINE_H

#include <map>
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
#define COLOR_DARKER_GREEN sf::Color(0.2*255, 0.9*255, 0.2*255)

#define GRID_THICKNESS 2

class GraphicEngine {

public:

GraphicEngine(World& world, int screen_w, int screen_h);
~GraphicEngine();

void run();

private:

World& world;

sf::RenderWindow window;
sf::Font default_font;

// Camera
sf::View camera;
sf::Vector2i camera_mouse_position;
bool camera_mouse_left;

// Cursor (deprecated?)
sf::Vector2i cursor_position;//expressed in world positions
sf::Color cursor_color;

// Interaction modes
bool insert_mode;
bool move_camera_mode;

// Rendering modes
bool show_grid; 
bool cell_color_mode;
static const sf::Color CELLS_COLOR[4];

// Colored selectors
int current_selector_color;
std::map<sf::Color, Poset, compare_colors > colored_selectors;
static const int COLORED_SELECTORS_WHEEL_SIZE;
static const sf::Color COLORED_SELECTORS_WHEEL[];

// Events
void handle_camera_events(const sf::Event& event);
void handle_insert_events(const sf::Event& event);
void handle_selectors_events(const sf::Event& event);
bool control_pressed();
bool shift_pressed();

// Camera
void camera_translate(float dx, float dy);
void camera_translate(const sf::Vector2f& vec);
void camera_zoom(float zoom_factor);
void camera_center(const sf::Vector2f& where);
sf::Vector2f get_camera_view_origin();
bool point_in_camera_view(const sf::Vector2f& point);
sf::Vector2i last_visible_cell(); // World coordinates of the left rightmost bottom cell

// Rendering
void render_grid();
void render_cursor();
void render_world();
void render_cell(const sf::Vector2i& cell_pos, const Cell& cell);
void render_colored_selectors();
void draw_cell_outline_fill(const sf::Vector2i& cell_position, 
                            const sf::Color& outline, const sf::Color& fill);
sf::Color color_of_cell(const Cell& cell);

// Coordinates conversion
sf::Vector2f map_world_coords_to_coords(const sf::Vector2i& world_coords); // World coords are cells position
sf::Vector2i map_coords_to_world_coords(const sf::Vector2f& coords);

// Cursor (deprecated?)
void move_cursor(const sf::Vector2i& pos_delta);

// Colored selectors
void colored_selectors_toggle(const sf::Vector2i& world_coord);
void colored_selectors_clear(const sf::Vector2i& world_coord);

};

#endif