#include "graphic_engine.h"

sf::Vector2f GraphicEngine::get_camera_view_origin()
{
    sf::Vector2f half_size = {camera.getSize().x/2,camera.getSize().y/2};
    return camera.getCenter()-half_size;
}

void GraphicEngine::camera_translate(float dx, float dy)
{
    camera.move(dx,dy);
    window.setView(camera);
}

void GraphicEngine::camera_translate(const sf::Vector2f& vec)
{
    camera_translate(vec.x, vec.y);
}

void GraphicEngine::camera_zoom(float zoom_factor)
{
    camera.zoom(1/zoom_factor);
    window.setView(camera);
}

void GraphicEngine::camera_center(const sf::Vector2f& where)
{
    camera.setCenter(where);
    window.setView(camera);
}

bool GraphicEngine::point_in_camera_view(const sf::Vector2f& point)
{
    /* Warning: that supposes that the view is not rotated */
    return sf::FloatRect({get_camera_view_origin(), 
                          camera.getSize()}).contains(point);
}

sf::Vector2i GraphicEngine::last_visible_cell()
{
    sf::Vector2f bottom_left_coords = window.mapPixelToCoords({0,(int)window.getSize().y});
    sf::Vector2i bottom_left_cell_pos = map_coords_to_world_coords(bottom_left_coords);
    return bottom_left_cell_pos;
}

sf::Vector2i GraphicEngine::first_visible_cell()
{
    sf::Vector2f top_left_coords = window.mapPixelToCoords({0,0});
    sf::Vector2i top_left_cell_pos = map_coords_to_world_coords(top_left_coords);
    return top_left_cell_pos;
}

bool GraphicEngine::cell_visible(const sf::Vector2i& cell_pos)
{
    sf::Vector2i top_left = first_visible_cell();
    sf::Vector2f bottom_right_coords = window.mapPixelToCoords({(int)window.getSize().x,(int)window.getSize().y});
    sf::Vector2i bottom_right_pos = map_coords_to_world_coords(bottom_right_coords);
    return cell_pos.x >= top_left.x && cell_pos.x <= bottom_right_pos.x && cell_pos.y >= top_left.y && cell_pos.y <= bottom_right_pos.y;
}