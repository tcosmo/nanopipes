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

void World::set_initial_cells(const std::string& initial_cells_input)
{
    record_initial_cells_input = initial_cells_input;
    if(mode == LINE_MODE)
        set_initial_cells_line_mode(initial_cells_input);
    if(mode == COL_MODE) 
        set_initial_cells_col_mode(initial_cells_input);
}

Cell World::deduce_backward(Cell south, Cell east)
{
    assert(east.status() == DEFINED && south.bit != UNDEFINED);
    Cell to_ret = {south.bit != (east.bit+east.carry)%2,0};
    to_ret.carry = int((to_ret.bit + east.bit + east.carry) >= 2);
    return to_ret;
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

int World::deduce_south_bit(Cell me, Cell east)
{
    assert(me.bit != UNDEFINED && east.bit != UNDEFINED && east.carry != UNDEFINED);
    return (me.bit + east.bit + east.carry)%2;
}

int World::deduce_me_carry(Cell me, Cell east)
{
    assert(me.bit != UNDEFINED && east.bit != UNDEFINED && east.carry != UNDEFINED);
    return (int)((me.bit + east.bit + east.carry)>1);
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