#include "graphic_engine.h"

bool GraphicEngine::control_pressed()
{
    return sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || 
           sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
}

void GraphicEngine::handle_insert_events(sf::Event event)
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
}

void GraphicEngine::handle_camera_events(sf::Event event)
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
        mouse_left = true;
    if(event.type == sf::Event::MouseEntered)
        mouse_left = false;

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
            mouse_position = sf::Mouse::getPosition(window); 
        }

    if(event.type == sf::Event::MouseMoved) {
        if(move_camera_mode) {
            auto coordMouseMove = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
            auto coordMouse = window.mapPixelToCoords(mouse_position);
            camera_translate(coordMouse-coordMouseMove);
            mouse_position = sf::Mouse::getPosition(window);              
        }
    }

    if(event.type == sf::Event::MouseButtonReleased)
        if(event.mouseButton.button == sf::Mouse::Middle) {
            move_camera_mode = false;
            
            if(mouse_left)
                sf::Mouse::setPosition({static_cast<int>(window.getSize().x/2),
                                        static_cast<int>(window.getSize().y/2)}, 
                                        window);
        }
}
