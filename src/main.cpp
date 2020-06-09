#include <cstdio>
#include "world.h"
#include "graphic_engine.h"
#include "input_parser.hpp"

int main(int argc, char** argv)
{
    InputParser input_parser(argc,argv);

    World world;

    if( input_parser.cmdOptionExists("-line") ) {
        world.set_mode(LINE_MODE);
    }
    const std::string &mode_line_input = input_parser.getCmdOption("-line");
    if (!mode_line_input.empty()) {
        world.set_initial_cells(mode_line_input);
        world.bootstrap_Collatz();
    }

    if( input_parser.cmdOptionExists("-col") ) {
        world.set_mode(COL_MODE);
    }
    const std::string &mode_col = input_parser.getCmdOption("-col");
    if (!mode_col.empty()) {
        world.set_initial_cells(mode_col);
        world.bootstrap_Collatz();
    }

    if( input_parser.cmdOptionExists("-cycle") ) {
        world.set_mode(CYCLE_MODE);
    }
    const std::string &parity_vector_input = input_parser.getCmdOption("-cycle");
    if (!parity_vector_input.empty()) {
        world.set_parity_vector(parity_vector_input);
        world.bootstrap_Collatz();
    }

    if( input_parser.cmdOptionExists("-border") ) {
        world.set_mode(BORDER_MODE);
    }
    const std::string &parity_vector_input_border = input_parser.getCmdOption("-border");
    if (!parity_vector_input_border.empty()) {
        world.set_parity_vector(parity_vector_input_border);
        world.bootstrap_Collatz();
    }

    GraphicEngine engine(world, 800*1.5,600*1.5);
    engine.run();
}