#include "world.h"

void World::set_initial_cells_col_mode(const std::string& initial_cells_input)
{
    std::vector<int> base3p = base3_to_3p(initial_cells_input);
    sf::Vector2i pos = {0,-1};
    for( int i = base3p.size()-1 ; i >= 0 ; i -= 1) {
        cells[pos] = {base3p[i]/2, base3p[i]%2};
        //Bootstraping can be done
        cells_on_edge.push_front({pos,0});
        pos += NORTH;
    }
    col_mode_last_macro_it_added = 0;
}

void World::next_micro_col()
{
    assert(!cells_on_edge.empty());// This is checked before, see next_micro();, prevents bad calls
    
    sf::Vector2i front_pos = cells_on_edge.front().pos;
    int front_macro_it = cells_on_edge.front().macro_iteration;
    nb_macro_iterations = front_macro_it;
    
    assert(!cell_exists(front_pos+WEST+SOUTH));

    cells[front_pos+WEST+SOUTH] = {0};

    if(!cell_exists(front_pos+WEST))
        cells[front_pos+WEST] = {0,0};

    cells[front_pos+WEST+SOUTH].bit = deduce_south_bit(cells[front_pos+WEST],cells[front_pos]);
    cells[front_pos+WEST].carry = deduce_me_carry(cells[front_pos+WEST],cells[front_pos]);

    if( !(col_mode_last_macro_it_added != front_macro_it+1 && cells[front_pos+WEST].to_index() == 0)) {
        cells_on_edge.push_back({front_pos+WEST,front_macro_it+1});
        col_mode_last_macro_it_added = front_macro_it+1;
    }

    if(!cell_exists(front_pos+SOUTH) || cells[front_pos+SOUTH].status() != DEFINED ) {
        if(cells[front_pos+WEST+SOUTH].bit == 1) {
            cells[front_pos+SOUTH] = {0,1,true};
            sf::Vector2i pos_correct = front_pos+SOUTH+EAST;
            while(cell_exists(pos_correct)) {
                cells[pos_correct].carry = 0;
                pos_correct += EAST;
            }

            cells[front_pos+WEST+SOUTH].carry = 1;
            cells[front_pos+WEST+SOUTH+SOUTH] = {0}; // To have the right amount of 0s
            cells_on_edge.push_back({front_pos+WEST+SOUTH,front_macro_it+1});
        } 
    }

    cells_on_edge.pop_front();
}