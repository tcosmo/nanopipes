#include "world.h"

World::World() 
{
    mode = LINE_MODE;//FREE;
    nb_macro_iterations = 0;
}

World::~World() 
{
}

void World::set_mode(WorldMode p_mode) 
{
    mode = p_mode;
}

void World::set_initial_cells(const std::string& initial_cells_input)
{
    record_initial_cells_input = initial_cells_input;
    if(mode == LINE_MODE) {
        sf::Vector2i pos = {-1,0};

        for( int i = initial_cells_input.size()-1 ; i >= 0 ; i-= 1 ) {
            char c = initial_cells_input[i];
            if( c != '0' && c != '1') {
                printf("Line mode excepts binary input. Example: ./nanopipes -l 0010110. Abort.\n");
                exit(1);
            }
            cells[pos].bit = (int)(c - '0');
            pos.x -= 1;
        }
    }
}

void World::bootstrap_Collatz_line() 
{
    sf::Vector2i pos = {-1,0};
    cells_on_edge.clear();
    bool non_zero_found = false;
    while( cells.find(pos) != cells.end() ) {
        if( cells[pos].bit == 0 && !non_zero_found)
            cells[pos].carry = 0;
        if( cells[pos].bit == 1 && !non_zero_found ) {
            non_zero_found = true;
            cells[{pos.x+1,pos.y}] = {0,1,true};
        }
        if(non_zero_found)
            cells_on_edge.push_back({pos,0});
        pos.x -= 1;
    }
}

void World::next_micro()
{
    if( mode == LINE_MODE )
        next_micro_line();
}

bool World::cell_exists(const sf::Vector2i& pos)
{
    return cells.find(pos) != cells.end();
}

void World::assert_cell_exists(const sf::Vector2i& pos)
{
    assert(cell_exists(pos));
}

int deduce_south_bit(Cell me, Cell east)
{
    assert(me.bit != UNDEFINED && east.bit != UNDEFINED && east.carry != UNDEFINED);
    return (me.bit + east.bit + east.carry)%2;
}

int deduce_me_carry(Cell me, Cell east)
{
    assert(me.bit != UNDEFINED && east.bit != UNDEFINED && east.carry != UNDEFINED);
    return (int)((me.bit + east.bit + east.carry)>1);
}

void World::next_micro_line()
{
    if( cells_on_edge.empty() ) return;
    MicroIteration first_cell = cells_on_edge.front();

    sf::Vector2i pos_me = first_cell.pos;
    assert_cell_exists(pos_me);

    sf::Vector2i pos_east = first_cell.pos+EAST;
    assert_cell_exists(pos_east);

    sf::Vector2i pos_south = first_cell.pos+SOUTH;
    cells[pos_south].bit = deduce_south_bit(cells[first_cell.pos],cells[pos_east]);
    cells[pos_me].carry = deduce_me_carry(cells[first_cell.pos],cells[pos_east]);

    MicroIteration last_cell = cells_on_edge.back();
    bool non_zero_found = false;
    
    if( last_cell.macro_iteration != first_cell.macro_iteration )
        non_zero_found = true;

    if( cells[pos_south].bit == 1 && !non_zero_found) {
        non_zero_found = true;
        cells[pos_south+EAST] = {0,1,true};
        sf::Vector2i south_neig = pos_south+EAST+EAST;
        while(cell_exists(south_neig)) {
            cells[south_neig].carry = 0;
            south_neig += EAST;
        }
    }

    cells_on_edge.pop_front();

    if( non_zero_found ) {
        cells_on_edge.push_back({pos_south,first_cell.macro_iteration+1});
        nb_macro_iterations = first_cell.macro_iteration+1;
    }

    if( cells_on_edge.front().macro_iteration != first_cell.macro_iteration )
        // Edge case where last element of the current line is \bar 1
        // We need to add a \bar 0 to the west of it and 0 to the west of that
        if( cells[pos_me].bit == 1 && cells[pos_me].carry == 1 ) {
            cells[pos_me+WEST] = {0,1};
            cells[pos_me+WEST+WEST] = {0,0};
            cells_on_edge.push_front({pos_me+WEST+WEST, first_cell.macro_iteration});
            cells_on_edge.push_front({pos_me+WEST, first_cell.macro_iteration});
        }
        // if 1 no carry we need to add a zero to west
        else if( cells[pos_me].bit == 1 && cells[pos_me].carry == 0 ) {
            cells[pos_me+WEST] = {0,0};
            cells_on_edge.push_front({pos_me+WEST, first_cell.macro_iteration});
        }
}

void World::reset() 
{
    cells.clear();
    cells_on_edge.clear();
    nb_macro_iterations = 0;
    if( record_initial_cells_input.size() != 0 ) {
        set_initial_cells(record_initial_cells_input);
        bootstrap_Collatz();
    }
}

void World::bootstrap_Collatz() 
{
    if(mode == LINE_MODE)
        bootstrap_Collatz_line();
    return;
}