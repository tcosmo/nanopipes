#include "world.h"

void World::bootstrap_Collatz_cycle()
{
    sf::Vector2i pos = CYCLIC_ORIGIN;
    for(bool e: pv.pv) {
        cells[pos] = {0,int(e),int(e)};
        if(e) cells_on_edge.push_back({pos,0});
        pos += PARITY_MOVES[e];
    }
    cells[pos] = cells[CYCLIC_ORIGIN];
    cycle_mode_last_macro_it_added = 0;
    cycle_detected = std::make_pair(-1,-1);
}

sf::Vector2i World::cyclic_equivalent_pos(const sf::Vector2i& pos)
{
    assert(mode == CYCLE_MODE);

    if(pos.x <= -1 && pos.x >= (-1)*pv.norm())
        return pos;

    bool to_the_right = pos.x > -1;
    if( to_the_right) {
        int cycle_rep = pos.x/pv.norm()+1;
        return {pos.x%pv.norm()-1*pv.norm(), pos.y+cycle_rep*pv.span()};
    } else {
        int cycle_rep = (pos.x+1)/pv.norm();

        return {-1*((abs(pos.x)-1)%pv.norm()+1), pos.y+cycle_rep*pv.span()};
    }
    return pos;
}

std::string World::cyclic_macro_it_to_string(int macro_it)
{
    assert(mode == CYCLE_MODE);
    sf::Vector2i start = {CYCLIC_ORIGIN.x,-1*macro_it};
    std::string to_return = "";
    for( auto e: pv.pv ) {
        assert(cell_defined(start));
        to_return.push_back(cells[start].to_index()+'0');
        start += PARITY_MOVES[e];
    }
    
    return to_return;
}

std::pair<std::string,std::string> World::get_cycle_expansion()
{
    //TODO: 2-adic
    
    if(cycle_detected.first == -1) return std::make_pair("","");

    std::string initial_seg = "";
    std::string period = "";

    sf::Vector2i pos = CYCLIC_ORIGIN+NORTH;
    int i = 0;
    for( ; i < cycle_detected.first ; i += 1 ) {
        initial_seg.push_back(cells[pos].to_trit());
        pos += NORTH;
    }

    for( ; i < cycle_detected.second ; i += 1 ) {
        period.push_back(cells[pos].to_trit());
        pos += NORTH;
    }

    return std::make_pair(initial_seg,period);
}

void World::next_micro_cycle()
{
    assert(!cells_on_edge.empty());
 
    sf::Vector2i front_pos = cells_on_edge.front().pos;
    int front_macro_it = cells_on_edge.front().macro_iteration;
    
    if( abs(front_pos.x) == pv.find_first_1()+1 ) {
        //this way of tracking the current macro iteration depends on having
        //the origin at {-1,0}. Not great I know.
        assert(CYCLIC_ORIGIN == sf::Vector2i({-1,0}));
        
        std::string last_level_string = cyclic_macro_it_to_string(nb_macro_iterations);
        
        //Cycle detected!
        if(cycle_period_detection_map.find(last_level_string)!=cycle_period_detection_map.end())
            cycle_detected = std::make_pair(cycle_period_detection_map[last_level_string], nb_macro_iterations);
        else
            cycle_period_detection_map[last_level_string] = nb_macro_iterations;


        printf("%s\n", cyclic_macro_it_to_string(nb_macro_iterations).c_str());
        nb_macro_iterations += 1;

    }

    assert_cell_defined(front_pos);

    if(cell_exists(front_pos+WEST+SOUTH)) {
        //printf("%d %d %d\n", front_pos.x, front_pos.y, nb_macro_iterations);
        assert(cell_defined(front_pos+WEST+SOUTH));// just for theory
        cells[front_pos+WEST] = deduce_backward(cells[front_pos+WEST+SOUTH],cells[front_pos]);
        //printf("POP FRONT %d %d\n", cells_on_edge.front().pos.x, cells_on_edge.front().pos.y);
        cells_on_edge.pop_front();

        // Deal with cyclicity
        if(!cell_exists(front_pos+WEST+WEST+SOUTH) && cell_exists(front_pos+WEST+SOUTH-pv.to_2D_vec())) {
            cells[front_pos+WEST-pv.to_2D_vec()] = cells[front_pos+WEST];
            cells_on_edge.push_front({front_pos+WEST-pv.to_2D_vec(),nb_macro_iterations});
            return;
        }

        if(cell_exists(front_pos+WEST+WEST+SOUTH)) {
            cells_on_edge.push_front({front_pos+WEST,nb_macro_iterations});
            //printf("PUSH FRONT %d %d\n", cells_on_edge.front().pos.x, cells_on_edge.front().pos.y);
        } else {
            cells_on_edge.push_back({front_pos+WEST,nb_macro_iterations});
            //printf("PUSH BACK %d %d\n", cells_on_edge.back().pos.x, cells_on_edge.back().pos.y);
        }
    }
}