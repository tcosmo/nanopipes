#include "world.h"

void World::bootstrap_Collatz_border()
{
    sf::Vector2i pos = BORDER_ORIGIN;
    for(bool e: pv.pv) {
        cells[pos] = {0,e,e};
        if(e && cells_on_edge.empty()) cells_on_edge.push_back({pos+WEST,0});
        pos += PARITY_MOVES[e];
    }
}

void World::next_micro_border()
{
    // Macro it are micro it
    sf::Vector2i front_pos = cells_on_edge.front().pos;
    int front_macro_it = cells_on_edge.front().macro_iteration;

    assert(cell_exists(front_pos+EAST));
    assert(cell_exists(front_pos+SOUTH));

    cells[front_pos] = deduce_backward(cells[front_pos+SOUTH], 
                                       cells[front_pos+EAST]);

    if(front_pos.y != 0) cells_on_edge.push_back({front_pos+NORTH,front_macro_it+1});
    else if(front_pos.x != -1*pv.norm()) {
        sf::Vector2i pos = front_pos+WEST;
        while(!cell_exists(pos+SOUTH)) { pos += SOUTH; }
        cells_on_edge.push_back({pos,front_macro_it+1});
    }
    cells_on_edge.pop_front();
}