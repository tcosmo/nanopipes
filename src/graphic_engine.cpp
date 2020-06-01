#include "graphic_engine.h"

const int GraphicEngine::COLORED_SELECTORS_WHEEL_SIZE = 5;
const sf::Color GraphicEngine::COLORED_SELECTORS_WHEEL[GraphicEngine::COLORED_SELECTORS_WHEEL_SIZE] = {
    sf::Color::Magenta,
    sf::Color(0x845FC2),
    sf::Color(0x2C73D2),
    sf::Color(0xFF9671),
    sf::Color(0xC34A36)
};

GraphicEngine::GraphicEngine(World& world, int screen_w, int screen_h) : world(world)
{
    window.create(sf::VideoMode(screen_w, screen_h), PROG_NAME);
    assert(default_font.loadFromFile("arial.ttf"));

    camera = window.getDefaultView();
    camera_mouse_left = false;
    window.setView(camera);

    cursor_position = {0,0};
    cursor_color = DEFAULT_CURSOR_COLOR;

    move_camera_mode = false;
    insert_mode = world.cells_on_edge.empty() || (world.mode == FREE);
    
    show_grid = true;
    cell_color_mode = false;

    current_selector_color = 0;
    for( int i_color = 0 ; i_color < COLORED_SELECTORS_WHEEL_SIZE ; i_color += 1)
        colored_selectors[COLORED_SELECTORS_WHEEL[i_color]] = Poset({});
}

GraphicEngine::~GraphicEngine()
{
}

void GraphicEngine::move_cursor(const sf::Vector2i& pos_delta)
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
    int sign_x = (coords.x < 0) ? -1*CELL_W : 0;
    int sign_y = (coords.y < 0) ? -1*CELL_H : 0;
    return sf::Vector2i({static_cast<int>((coords.x+sign_x)/CELL_W),
                         static_cast<int>((coords.y+sign_y)/CELL_H)});
}

void GraphicEngine::colored_selectors_toggle(const sf::Vector2i& world_coord)
{
    const sf::Color& current_color = COLORED_SELECTORS_WHEEL[current_selector_color];
    auto find_iterator = colored_selectors[current_color].find(world_coord);
    if( find_iterator  == colored_selectors[current_color].end() )
        colored_selectors[current_color].insert(world_coord);
    else
        colored_selectors[current_color].erase(find_iterator);
}

void GraphicEngine::colored_selectors_clear(const sf::Vector2i& world_coord)
{
    for(auto& color_poset: colored_selectors )
        if( color_poset.second.find(world_coord) != color_poset.second.end() )
            color_poset.second.clear();
}

void GraphicEngine::colored_selectors_clear_all()
{
    for(auto& color_poset: colored_selectors )
        color_poset.second.clear();
}

void GraphicEngine::run()
{
    camera_zoom(3);
    camera_center({-5*CELL_W,0});

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            handle_camera_events(event);

            if(insert_mode)
                handle_insert_events(event);

            if(shift_pressed())
                handle_selectors_events(event);

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
                        if(world.mode == COL_MODE)
                            while(world.nb_macro_iterations < abs(last_visible_cell().x))
                                world.next_micro();
                    break;

                    case sf::Keyboard::R:
                        world.reset();
                        insert_mode = world.cells_on_edge.empty();
                        colored_selectors_clear_all();
                    break;

                    case sf::Keyboard::A:
                        printf("Macro it: %d\n", world.nb_macro_iterations);
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
        render_colored_selectors();

        window.display();
    }
}
