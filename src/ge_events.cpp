#include "graphic_engine.h"

bool GraphicEngine::control_pressed()
{
    return sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || 
           sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
}

bool GraphicEngine::shift_pressed()
{
    return sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || 
           sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
}

void GraphicEngine::handle_insert_events(const sf::Event& event)
{
    if( world.mode == LINE_MODE )
        if(event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
                case sf::Keyboard::Right:
                    move_cursor(EAST);
                break;

                case sf::Keyboard::Left:
                               
                    move_cursor(WEST);
                break;

                case sf::Keyboard::Numpad0:
                    world.cells[cursor_position] = {0};
                break;

                case sf::Keyboard::Numpad1:
                    world.cells[cursor_position] = {1};
                break;
            }
        }

    if( world.mode == COL_MODE )
        if(event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
                case sf::Keyboard::Up:
                    move_cursor(NORTH);
                break;

                case sf::Keyboard::Down:
                    move_cursor(SOUTH);
                break;
            }
        }
}

void GraphicEngine::handle_camera_events(const sf::Event& event)
{
    if(event.type == sf::Event::KeyPressed) {
        switch(event.key.code) {
            case sf::Keyboard::Up:
                if(event.key.control)
                    camera_translate(0,-DEFAULT_CAM_TRANSLATION*CELL_H);
            break;

            case sf::Keyboard::Down:
                if(event.key.control)
                    camera_translate(0,DEFAULT_CAM_TRANSLATION*CELL_H);
            break;

            case sf::Keyboard::Right:
                if(event.key.control)
                    camera_translate(DEFAULT_CAM_TRANSLATION*CELL_W,0);
            break;

            case sf::Keyboard::Left:
                if(event.key.control)
                    camera_translate(-DEFAULT_CAM_TRANSLATION*CELL_W,0);
            break;
        }
    }

    if(event.type == sf::Event::MouseLeft)
        camera_mouse_left = true;
    if(event.type == sf::Event::MouseEntered)
        camera_mouse_left = false;

    if (event.type == sf::Event::MouseWheelScrolled)
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            if( control_pressed() ) {
                float zoom_factor = DEFAULT_CAM_ZOOM_STEP;
                if( event.mouseWheelScroll.delta < 0 )
                    zoom_factor = 1/zoom_factor;
                camera_zoom(zoom_factor);
            }
        }

    if(event.type == sf::Event::MouseButtonPressed)
        if( (event.mouseButton.button == sf::Mouse::Middle) ) {
            move_camera_mode = true;
            camera_mouse_position = sf::Mouse::getPosition(window); 
        }

    if(event.type == sf::Event::MouseMoved) {
        if(move_camera_mode) {
            auto coordMouseMove = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
            auto coordMouse = window.mapPixelToCoords(camera_mouse_position);
            camera_translate(coordMouse-coordMouseMove);
            camera_mouse_position = sf::Mouse::getPosition(window);              
        }
    }

    if(event.type == sf::Event::MouseButtonReleased)
        if(event.mouseButton.button == sf::Mouse::Middle) {
            move_camera_mode = false;
            
            if(camera_mouse_left)
                sf::Mouse::setPosition({static_cast<int>(window.getSize().x/2),
                                        static_cast<int>(window.getSize().y/2)}, 
                                        window);
        }
}

void GraphicEngine::handle_selectors_events(const sf::Event& event)
{
    if(event.type == sf::Event::MouseButtonPressed) {
        if( (event.mouseButton.button == sf::Mouse::Left) ) {
            sf::Vector2i cliked_cell_pos = map_coords_to_world_coords(
                        window.mapPixelToCoords(sf::Mouse::getPosition(window)));
            colored_selectors_toggle(cliked_cell_pos);
        }
        if( (event.mouseButton.button == sf::Mouse::Right) ) {
            sf::Vector2i cliked_cell_pos = map_coords_to_world_coords(
                        window.mapPixelToCoords(sf::Mouse::getPosition(window)));
            colored_selectors_clear(cliked_cell_pos);
        }
    }

    if(event.type == sf::Event::KeyPressed) {
        switch(event.key.code) {
            case sf::Keyboard::Right:
                current_selector_color += 1;
                current_selector_color %= COLORED_SELECTORS_WHEEL_SIZE;
            break;

            case sf::Keyboard::Left:
                current_selector_color -= 1;
                current_selector_color %= COLORED_SELECTORS_WHEEL_SIZE;
            break;
        }
    }
}