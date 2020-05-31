#include <cstdio>
#include "world.h"
#include "graphic_engine.h"
#include "input_parser.hpp"

int main(int argc, char** argv)
{
    InputParser input_parser(argc,argv);

    World world;

    if( input_parser.cmdOptionExists("-l") ) {
        world.set_mode(LINE_MODE);
    }

    const std::string &mode_line_input = input_parser.getCmdOption("-l");
    if (!mode_line_input.empty()) {
        world.set_initial_cells(mode_line_input);
        world.bootstrap_Collatz();
    }

    if( input_parser.cmdOptionExists("-c") ) {
        world.set_mode(COL_MODE);
    }

    const std::string &mode_col = input_parser.getCmdOption("-c");
    if (!mode_col.empty()) {
        world.set_initial_cells(mode_col);
        world.bootstrap_Collatz();
    }

    GraphicEngine engine(world, 800*1.5,600*1.5);

    engine.run();
}