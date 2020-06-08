#include "world.h"

const sf::Vector2i World::CYCLIC_ORIGIN = {-1,0};

World::World() 
{
    mode = FREE;
    nb_macro_iterations = 0;
}

World::~World() 
{
}

void World::set_mode(WorldMode p_mode) 
{
    mode = p_mode;
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

std::vector<int> World::base3_to_3p(std::string base3)
{
    std::reverse(base3.begin(), base3.end());
    std::vector<int> to_return;

    bool last_seen_0 = true;
    int i = 0;
    for(char c : base3) {
        switch(c) {
            case '0':
                if( i == 0 ) {
                    printf("The input cannot be a multiple of 3 (last digit equals 0). Abort.\n");
                    exit(1);
                }
                to_return.push_back(0);
                last_seen_0 = true;
                break;
            case '1':
                if(last_seen_0)
                    to_return.push_back(1);
                else
                    to_return.push_back(2);
                break;
            case '2':
                to_return.push_back(3);
                last_seen_0 = false;
                break;
            default:
                printf("Character `%c` is invalid base 3 digit. Abort.\n", c);
                exit(1);
                break;
        }
        i += 1;
    }

    std::reverse(to_return.begin(), to_return.end());
    return to_return;
}

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

void World::set_initial_cells(const std::string& initial_cells_input)
{
    record_initial_cells_input = initial_cells_input;
    if(mode == LINE_MODE)
        set_initial_cells_line_mode(initial_cells_input);
    if(mode == COL_MODE) 
        set_initial_cells_col_mode(initial_cells_input);
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

Cell World::deduce_backward(Cell south, Cell east)
{
    assert(east.status() == DEFINED && south.bit != UNDEFINED);
    Cell to_ret = {south.bit != (east.bit+east.carry)%2,0};
    to_ret.carry = int((to_ret.bit + east.bit + east.carry) >= 2);
    return to_ret;
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

void World::next_micro()
{
    if(cells_on_edge.empty())
        return;
    if( mode == LINE_MODE )
        next_micro_line();
    if( mode == COL_MODE )
        next_micro_col();
    if( mode == CYCLE_MODE )
        next_micro_cycle();
}

bool World::cell_exists(const sf::Vector2i& pos)
{
    return cells.find(pos) != cells.end();
}

void World::assert_cell_exists(const sf::Vector2i& pos)
{
    assert(cell_exists(pos));
}

bool World::cell_defined(const sf::Vector2i& pos)
{
    return cell_exists(pos) && cells[pos].status() == DEFINED;
}

void World::assert_cell_defined(const sf::Vector2i &pos)
{
    assert(cell_defined(pos));
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

void World::set_parity_vector(const std::string& pv_str)
{
    for( auto c: pv_str ) {
        if( c == '0' ) pv.push_back(false);
        else if( c == '1') pv.push_back(true);
        else { printf("Symbol `%c` is not valid in a parity vector. Abort.", c); exit(1); }
    }
}

void World::rotate_pv(int r)
{
    assert(mode == CYCLE_MODE);
    ParityVector new_pv;
    for( int i = 0 ; i < pv.pv.size() ; i += 1 )
        new_pv.push_back(pv[(pv.norm()+i+r)%(pv.norm())]);
    pv = new_pv;
    reset();
}       

void World::reset() 
{
    cells.clear();
    cells_on_edge.clear();
    cycle_period_detection_map.clear();
    cycle_detected = std::make_pair(-1,-1);
    nb_macro_iterations = 0;
    if( record_initial_cells_input.size() != 0 ) {
        set_initial_cells(record_initial_cells_input);
        bootstrap_Collatz();
    }

    if( pv.pv.size() != 0 ) {
        bootstrap_Collatz_cycle();
    }
}

void World::bootstrap_Collatz() 
{
    if(mode == LINE_MODE)
        bootstrap_Collatz_line();
    if(mode == CYCLE_MODE)
        bootstrap_Collatz_cycle();
    return;
}