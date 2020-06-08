#include "world.h"

void World::set_initial_cells_line_mode(const std::string& initial_cells_input)
{
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
        pos += WEST;
    }
    line_mode_macro_iteration_one_found = std::make_pair(0,false);
}

void World::next_micro_line()
{      
    assert(!cells_on_edge.empty());// This is checked before, see next_micro();, prevents bad calls
    sf::Vector2i front_pos = cells_on_edge.front().pos;
    int front_macro_it = cells_on_edge.front().macro_iteration;
    nb_macro_iterations = front_macro_it;

    //printf("%d %d %d\n", front_pos.x, front_pos.y, front_macro_it);

    assert_cell_exists(front_pos);

    if( front_macro_it != line_mode_macro_iteration_one_found.first ) {
        assert(cells[front_pos].bit == 1 && cell_exists(front_pos+EAST) && (cells[front_pos+EAST].bit == 0));
        cells[front_pos+EAST].carry = 1;
        cells[front_pos+EAST].special_carry = true;
        sf::Vector2i front_pos_east = front_pos+EAST+EAST;
        while(cell_exists(front_pos_east)) {
            cells[front_pos_east].carry = 0;
            front_pos_east += EAST;
        }
        line_mode_macro_iteration_one_found = std::make_pair(front_macro_it,false);
    }

    assert_cell_defined(front_pos+EAST);

    cells[front_pos].carry = deduce_me_carry(cells[front_pos],cells[front_pos+EAST]);
    cells[front_pos+SOUTH].bit = deduce_south_bit(cells[front_pos],cells[front_pos+EAST]);

    if( cells[front_pos+SOUTH].bit == 1 )  line_mode_macro_iteration_one_found.second = true;

    if( line_mode_macro_iteration_one_found.second ) {
        //printf("PUSH BACK %d %d %d\n", (front_pos+SOUTH).x, (front_pos+SOUTH).y, front_macro_it+1);
        cells_on_edge.push_back({front_pos+SOUTH,front_macro_it+1});
    }

    //printf("POP FRONT\n");
    cells_on_edge.pop_front();

        // Extending the space if current macro iteration ends with \bar 0, 1 or \bar 1
    if( !cell_exists(front_pos + WEST) && (cells[front_pos].bit + cells[front_pos].carry) >= 1 ) {
        cells[front_pos + WEST] = {0,0};
        cells_on_edge.push_front({front_pos+WEST,front_macro_it});
        //printf("PUSH FRONT\n");
    }
}

