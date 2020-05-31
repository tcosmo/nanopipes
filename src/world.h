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

    void next_micro(); // next micro iteration
    int nb_macro_iterations;

    static std::vector<int> base3_to_3p(std::string base3);
private:
    std::string record_initial_cells_input;

    // Cells routines
    bool cell_exists(const sf::Vector2i& pos);
    void assert_cell_exists(const sf::Vector2i& pos);
    bool cell_defined(const sf::Vector2i& pos);
    void assert_cell_defined(const sf::Vector2i& pos);

    // Iterate line mode
    std::pair<int,bool> line_mode_macro_iteration_one_found;
    void set_initial_cells_line_mode(const std::string& initial_cells_input);
    void bootstrap_Collatz_line();
    void next_micro_line(); // micro Collatz iteration in line mode
    
    // Iterate col mode
    int col_mode_last_macro_it_added;
    void set_initial_cells_col_mode(const std::string& initial_cells_input);
    void next_micro_col(); // micro Collatz iteration in col mode
};

#endif