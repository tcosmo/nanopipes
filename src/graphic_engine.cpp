#include "graphic_engine.h"

GraphicEngine::GraphicEngine(int screen_w, int screen_h)
{
    window.create(sf::VideoMode(screen_w, screen_h), PROG_NAME);
    camera = window.getDefaultView();
    window.setView(camera);
    move_camera_mode = 0;

    mouse_left = false;
}

GraphicEngine::~GraphicEngine()
{
}

void GraphicEngine::render_grid(int grid_w, int grid_h, int thickness)
{
    sf::RectangleShape carre(sf::Vector2f(CELL_W, CELL_H));
    carre.setFillColor(sf::Color::Black);
    window.draw(carre);
    for( int row = 0 ; row < grid_h ; row += 1 ) {
        sf::RectangleShape line(sf::Vector2f(grid_h*CELL_H, thickness));
        line.setFillColor(GRID_COLOR);
        line.setPosition(0,row*CELL_H);
        window.draw(line);
    }

    for( int col = 0 ; col < grid_w ; col += 1 ) {
        sf::RectangleShape line(sf::Vector2f(grid_w*CELL_W, thickness));
        line.setFillColor(GRID_COLOR);
        line.setPosition(col*CELL_W,0);
        line.rotate(90);
        window.draw(line);
    }
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
    camera.zoom(zoom_factor);
    window.setView(camera);
}

bool control_pressed()
{
    return sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || 
           sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
}

bool GraphicEngine::point_in_camera_view(const sf::Vector2f& point)
{
    /* Warning: that supposes that the view is not rotated */
    sf::Vector2f half_size = {camera.getSize().x/2, camera.getSize().y/2};
    return sf::FloatRect({camera.getCenter()-half_size, 
                          camera.getSize()}).contains(point);
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
                float zoom_factor = 1/DEFAULT_CAM_ZOOM_STEP;
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
                sf::Mouse::setPosition({window.getSize().x/2,window.getSize().y/2}, 
                                        window);
        }
}

void GraphicEngine::run()
{
    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            handle_camera_events(event);

            if(event.type == sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Escape:
                        window.close();
                    break;

                    case sf::Keyboard::A:
                        sf::Vector2f worldPos = window.mapPixelToCoords({0,0});
                        printf("%lf %lf\n", worldPos.x, worldPos.y);
                        worldPos = window.mapPixelToCoords({10,0});
                        printf("%lf %lf\n", worldPos.x, worldPos.y);
                    break;
                }
            }
            if(event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(BACKGROUND_COLOR);
        render_grid(200,200);

        window.display();
    }
}
