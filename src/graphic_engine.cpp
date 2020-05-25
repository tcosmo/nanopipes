#include "graphic_engine.h"

GraphicEngine::GraphicEngine(int screen_w, int screen_h)
{
    window.create(sf::VideoMode(screen_w, screen_h), PROG_NAME);
    camera = window.getDefaultView();
    window.setView(camera);
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

void GraphicEngine::translate_camera(float dx, float dy)
{
    camera.move(dx,dy);
    window.setView(camera);
}

void GraphicEngine::run()
{
    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Escape:
                        window.close();
                    break;

                    case sf::Keyboard::Up:
                        if(event.key.control)
                            translate_camera(0,-DEFAULT_CAM_TRANSLATION*CELL_H);
                    break;

                    case sf::Keyboard::Down:
                        if(event.key.control)
                            translate_camera(0,DEFAULT_CAM_TRANSLATION*CELL_H);
                    break;

                    case sf::Keyboard::Right:
                        if(event.key.control)
                            translate_camera(DEFAULT_CAM_TRANSLATION*CELL_W,0);
                    break;

                    case sf::Keyboard::Left:
                        if(event.key.control)
                            translate_camera(-DEFAULT_CAM_TRANSLATION*CELL_W,0);
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
