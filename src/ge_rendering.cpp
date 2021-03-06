#include "graphic_engine.h"

const sf::Color GraphicEngine::CELLS_COLOR[4] = {COLOR_DARKER_GREEN, sf::Color::Black, sf::Color::Magenta, sf::Color::Blue};

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

void GraphicEngine::render_front_edge()
{
    if(!world.cells_on_edge.empty())
        draw_cell_outline_fill(world.cells_on_edge.front().pos, sf::Color::Magenta, sf::Color::Transparent);
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
    int index = (!inverse_color) ? cell.to_index() : 3-cell.to_index();
    return GraphicEngine::CELLS_COLOR[index];
}

void GraphicEngine::render_cell(const sf::Vector2i& cell_pos, const Cell& cell)
{
    if( cell.status() == UNDEFINED ) return;

    if( cell.status() == DEFINED ) {
        if(!cell_color_mode) 
            draw_cell_outline_fill(cell_pos, sf::Color::Transparent, BACKGROUND_COLOR_DEFINED);
        else 
            draw_cell_outline_fill(cell_pos, color_of_cell(cell), color_of_cell(cell));
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
    if(world.mode == CYCLE_MODE && show_cyclic_symmetries) {
        sf::Vector2i pos = first_visible_cell();
        sf::Vector2i last = last_visible_cell();
        while(pos.y <= last.y) {
            sf::Vector2i current_pos = pos;
            while(cell_visible(current_pos)) {
                sf::Vector2i cyclic_eq = world.cyclic_equivalent_pos(current_pos);
                if( world.cell_exists(cyclic_eq) )
                    render_cell(current_pos,world.cells[cyclic_eq]);
                current_pos += EAST;
            }
            pos += SOUTH;
        }

        return;
    }


    for( const auto& pos_and_cell : world.cells ) {
        const sf::Vector2i& cell_pos = pos_and_cell.first;

        // Be sparse and do not render out of sight cells
        // Particularly useful when coming from very un zoomed to very zoomed
        // SFML seems to struggle when having to render stuff far out of sight.
        
        const Cell& cell = pos_and_cell.second;
        if(cell_visible(cell_pos))
            render_cell(cell_pos, cell);
    }
}

void GraphicEngine::render_colored_selectors()
{
    for( const auto& color_pos: colored_selectors )
        for( const auto& pos: colored_selectors[color_pos.first] )
            draw_cell_outline_fill(pos, color_pos.first, sf::Color::Transparent);
}

void GraphicEngine::set_cell_border(const sf::Vector2i& pos, const sf::Vector2i& which, const sf::Color& color)
{
    if( which == SOUTH ) {
        sf::RectangleShape border(sf::Vector2f({((float)CELL_W), (float)GRID_THICKNESS}));
        border.setPosition(map_world_coords_to_coords(pos+SOUTH));
        border.setFillColor(color);
        window.draw(border);
    }
    if( which == EAST ) {
        sf::RectangleShape border(sf::Vector2f({((float)CELL_H), (float)GRID_THICKNESS}));
        border.setPosition(map_world_coords_to_coords(pos+EAST));
        border.rotate(90);
        border.setFillColor(color);
        window.draw(border);
    }
}

void GraphicEngine::render_colored_border()
{
    for( const auto& color_pos: colored_border )
        for( const auto& pos: colored_border[color_pos.first] ) {
            sf::Vector2i start = pos-world.pv.to_2D_vec();
            for(auto e: world.pv.pv) {
                set_cell_border(start, SOUTH, color_pos.first);
                if(e) set_cell_border(start+SOUTH+WEST, EAST, color_pos.first);
                start += PARITY_MOVES[e];
            }
        }
}