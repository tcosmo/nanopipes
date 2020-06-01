#ifndef DEF_WORLD_H
#define DEF_WORLD_H

#include <map>
#include <vector>
#include <algorithm>
#include <deque>

#include "global.h"

enum WorldMode {
    FREE = 0,
    LINE_MODE,
    COL_MODE,
    CYCLE_MODE
};

enum CellStatus {
    UNDEFINED = -1,
    BIT_DEFINED,
    CARRY_DEFINED,
    DEFINED
};

struct Cell {
    int bit, carry;
    bool special_carry;
    Cell(int bit = UNDEFINED, int carry = UNDEFINED, bool special_carry = false) : 
    bit(bit), carry(carry), special_carry(special_carry) {};
    CellStatus status() const { 
        if( bit == UNDEFINED && carry == UNDEFINED ) return UNDEFINED;
        if( bit == UNDEFINED ) return CARRY_DEFINED;
        if( carry == UNDEFINED ) return BIT_DEFINED;
        return DEFINED;
    }
    int to_index() const {
        return 2*bit + carry;
    }
};

struct MicroIteration {
    sf::Vector2i pos;
    int macro_iteration;
};

static sf::Vector2i PARITY_MOVES[2] = {WEST, SOUTH+WEST};

struct ParityVector {
    std::vector<bool> pv;
    sf::Vector2i _vec_2D;
    int _first_one;
    ParityVector(std::vector<bool> pv = {}) : pv(pv) { _vec_2D = {0,0}; _first_one = -2; }
    int norm() { return pv.size(); }
    int span() 
    { 
        int result = 0;
        for( auto e: pv) result += (int)e;
        return result;
    }

    int find_first_1(bool recompute=false)
    {
        if(_first_one != -2 && !recompute)
            return _first_one;
        _first_one = -1;
        int i = 0;
        for( auto e: pv ) {
            if(e) { _first_one = i; break;}
            i++;
        }
        return _first_one;
    }

    sf::Vector2i to_2D_vec(bool recompute=false)
    {
        if(_vec_2D != sf::Vector2i({0,0}) && !recompute)
            return _vec_2D;
        sf::Vector2i to_return = {0,0};
        for( auto e: pv ) to_return += PARITY_MOVES[e];
        _vec_2D = to_return;
        return to_return;
    }

    bool operator[](std::size_t idx)  { return pv[idx]; }
    const bool operator[](std::size_t idx) const { return pv[idx]; }


    void push_back(bool e) { pv.push_back(e); }
};

class World {

public:
    World();
    ~World();

    WorldMode mode;
    std::map<sf::Vector2i,Cell,compare_world_positions> cells;
    std::deque<MicroIteration> cells_on_edge;

    void reset();
    void bootstrap_Collatz();

    void set_mode(WorldMode p_mode);
    void set_initial_cells(const std::string& initial_cells_input);
    void set_parity_vector(const std::string& pv_str);

    // Cells routines
    bool cell_exists(const sf::Vector2i& pos);
    void assert_cell_exists(const sf::Vector2i& pos);
    bool cell_defined(const sf::Vector2i& pos);
    void assert_cell_defined(const sf::Vector2i& pos);

    //Cycles
    sf::Vector2i cyclic_equivalent_pos(const sf::Vector2i& pos);

    ParityVector pv;
    static const sf::Vector2i CYCLIC_ORIGIN;

    void next_micro(); // next micro iteration
    int nb_macro_iterations;

    static std::vector<int> base3_to_3p(std::string base3);
private:
    std::string record_initial_cells_input;

    // Iterate line mode
    std::pair<int,bool> line_mode_macro_iteration_one_found;
    void set_initial_cells_line_mode(const std::string& initial_cells_input);
    void bootstrap_Collatz_line();
    void next_micro_line(); // micro Collatz iteration in line mode
    
    // Iterate col mode
    int col_mode_last_macro_it_added;
    void set_initial_cells_col_mode(const std::string& initial_cells_input);
    void next_micro_col(); // micro Collatz iteration in col mode

    // Iterate cycle mode
    int cycle_mode_last_macro_it_added;
    void bootstrap_Collatz_cycle();
    void next_micro_cycle();
    Cell deduce_backward(Cell south, Cell east);
};

#endif