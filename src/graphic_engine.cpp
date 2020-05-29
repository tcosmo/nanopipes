#include "graphic_engine.h"

const sf::Color CELLS_COLOR[4] = {sf::Color::Green, sf::Color::Black, sf::Color::Magenta, sf::Color::Blue};

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

sf::Vector2f GraphicEngine::get_camera_view_origin()
{
    sf::Vector2f half_size = {camera.getSize().x/2,camera.getSize().y/2};
    return camera.getCenter()-half_size;
}

void GraphicEngine::render_grid()
{    
    auto view_origin = get_camera_view_origin();
    int col_start = view_origin.x/CELL_W;
    int col_end = (view_origin.x+camera.getSize().x)/CELL_W;
    int line_start = view_origin.y/CELL_H;
    int line_end = (view_origin.y+camera.getSize().y)/CELL_H;

    sf::RectangleShape col(sf::Vector2f(camera.getSize().y, GRID_THICKNESS));
    col.rotate(90);
    col.setFillColor(GRID_COLOR);

    for( int i_col = col_start ; i_col <= col_end ; i_col += 1 ) {
        col.setPosition({static_cast<float>(CELL_W*i_col), 
                         static_cast<float>(view_origin.y)});
        window.draw(col);
    }

    sf::RectangleShape line(sf::Vector2f(camera.getSize().x, GRID_THICKNESS));
    line.setFillColor(GRID_COLOR);
    for( int i_line = line_start ; i_line <= line_end ; i_line += 1 ) {
        line.setPosition({static_cast<float>(view_origin.x),
                          static_cast<float>(CELL_H*i_line)});
        window.draw(line);
    }
}

sf::Color GraphicEngine::color_of_cell(const Cell& cell)
{
    assert(cell.status() == DEFINED);
    return CELLS_COLOR[cell.to_index()];
}

void GraphicEngine::render_cell(const sf::Vector2i& cell_pos, const Cell& cell)
{
    if( cell.status() == UNDEFINED ) return;

    if( cell.status() == DEFINED ) {
        if(!cell_color_mode) draw_cell_outline_fill(cell_pos, sf::Color::Transparent, BACKGROUND_COLOR_DEFINED);
        else draw_cell_outline_fill(cell_pos, color_of_cell(cell), color_of_cell(cell));
    }
    else
        draw_cell_outline_fill(cell_pos, sf::Color::Transparent, BACKGROUND_COLOR_HALF_DEFINED);
    
    sf::Vector2f cell_ge_pos = map_world_coords_to_coords(cell_pos);
    if( cell.bit != UNDEFINED ) {
        sf::Text text;
        text.setFont(default_font);
        text.setString(std::string({(char)(cell.bit + '0')}));
        text.setCharacterSize(15);

        text.setFillColor(sf::Color::White);
        
        float text_x = (CELL_W-text.getLocalBounds().width)/2;
        float text_y = (CELL_H-text.getLocalBounds().height)/2;

        text.setPosition(cell_ge_pos + sf::Vector2f({text_x-(float)1, text_y-1}));//Tweaking text pos
        window.draw(text);
    }

    if( cell.carry == 1 ) {
        sf::RectangleShape col(sf::Vector2f({((float)CELL_W)/2.2, (float)GRID_THICKNESS}));
        col.setPosition( cell_ge_pos + sf::Vector2f({(CELL_W-col.getSize().x)/2-(float)0.8, 2*GRID_THICKNESS}) );//Tweaking carry pos
        if(!cell.special_carry) col.setFillColor(sf::Color::White);
        else col.setFillColor(COLOR_SPECIAL_CARRY);
        window.draw(col);
    }

}

void GraphicEngine::render_world()
{
    for( const auto& pos_and_cell : world.cells ) {
        const sf::Vector2i& cell_pos = pos_and_cell.first;
        const Cell& cell = pos_and_cell.second;
        render_cell(cell_pos, cell);
    }
}

void GraphicEngine::draw_cell_outline_fill(const sf::Vector2i& cell_position, 
                                           const sf::Color& outline, const sf::Color& fill)
{
    sf::RectangleShape cursor(sf::Vector2f(CELL_W-GRID_THICKNESS, CELL_H-GRID_THICKNESS));
    cursor.setOutlineColor(outline);
    cursor.setOutlineThickness(GRID_THICKNESS);
    cursor.setPosition(map_world_coords_to_coords(cell_position)+
                        sf::Vector2f({0.0, static_cast<float>(GRID_THICKNESS)}));
    cursor.setFillColor(fill);
    window.draw(cursor);
}

void GraphicEngine::render_cursor()
{
    draw_cell_outline_fill(cursor_position, cursor_color, sf::Color::Transparent);
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

bool control_pressed()
{
    return sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || 
           sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
}

bool GraphicEngine::point_in_camera_view(const sf::Vector2f& point)
{
    /* Warning: that supposes that the view is not rotated */
    return sf::FloatRect({get_camera_view_origin(), 
                          camera.getSize()}).contains(point);
}

void GraphicEngine::move_cursor(sf::Vector2i pos_delta)
{
    cursor_position += pos_delta;
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

sf::Vector2i GraphicEngine::last_visible_cell()
{
    sf::Vector2f bottom_left_coords = window.mapPixelToCoords({0,(int)window.getSize().y});
    sf::Vector2i bottom_left_cell_pos = map_coords_to_world_coords(bottom_left_coords);
    return bottom_left_cell_pos;
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
