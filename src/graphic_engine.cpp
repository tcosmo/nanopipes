#include "graphic_engine.h"

GraphicEngine::GraphicEngine(World& world, int screen_w, int screen_h) : world(world)
{
    window.create(sf::VideoMode(screen_w, screen_h), PROG_NAME);
    camera = window.getDefaultView();
    window.setView(camera);
    move_camera_mode = false;
    show_grid = true;
    mouse_left = false;
    insert_mode = world.cells_on_edge.empty();
    cursor_position = {0,0};
    cursor_color = DEFAULT_CURSOR_COLOR;
    assert(default_font.loadFromFile("arial.ttf"));
    cell_color_mode = false;
}

GraphicEngine::~GraphicEngine()
{
}

void GraphicEngine::move_cursor(sf::Vector2i pos_delta)
{
    cursor_position += pos_delta;
}

sf::Vector2f GraphicEngine::map_world_coords_to_coords(const sf::Vector2i& world_coords)
{
    return sf::Vector2f({static_cast<float>(world_coords.x*CELL_W),
                         static_cast<float>(world_coords.y*CELL_H)});
}

sf::Vector2i GraphicEngine::map_coords_to_world_coords(const sf::Vector2f& coords)
{
    return sf::Vector2i({static_cast<int>(coords.x/CELL_W),
                         static_cast<int>(coords.y/CELL_H)});
}

void GraphicEngine::run()
{
    camera_zoom(3);
    camera_center({-5*CELL_W,0});

    // world.cells[{0,0}] = {0,0};
    // world.cells[{0,1}] = {0,1};
    // world.cells[{1,0}] = {1,0};
    // world.cells[{1,1}] = {1,1};
    // world.cells[{-1,0}] = {1};
    // world.cells[{-2,0}] = {UNDEFINED,1};

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            handle_camera_events(event);

            if(insert_mode)
                handle_insert_events(event);

            if(event.type == sf::Event::KeyPressed) {
                switch(event.key.code) {
                    case sf::Keyboard::Escape:
                        window.close();
                    break;

                    case sf::Keyboard::C:
                        if(!insert_mode)
                            camera_center({0,0});
                        else
                            camera_center(map_world_coords_to_coords(cursor_position));
                    break;

                    case sf::Keyboard::G:
                        show_grid = !show_grid;
                    break;

                    case sf::Keyboard::K:
                        cell_color_mode = !cell_color_mode;
                    break;

                    case sf::Keyboard::B:
                        world.bootstrap_Collatz();
                        if(insert_mode)
                            insert_mode = !insert_mode;
                    break;

                    case sf::Keyboard::N:
                        world.next_micro();
                    break;

                    case sf::Keyboard::M:
                        if(world.mode == LINE_MODE)
                            while(world.nb_macro_iterations < last_visible_cell().y)
                                world.next_micro();
                    break;

                    case sf::Keyboard::R:
                        world.reset();
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

        sf::RectangleShape carre(sf::Vector2f(CELL_W, CELL_H));
        carre.setFillColor(sf::Color::Black);
        window.draw(carre);

        if( show_grid )
            render_grid();

        if( insert_mode )
            render_cursor();

        render_world();

        window.display();
    }
}
