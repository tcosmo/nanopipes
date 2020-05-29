#ifndef DEF_GLOBAL_H
#define DEF_GLOBAL_H

#ifdef _WIN32
#define OS_PATH_SEPARATOR "\\"
#endif

#ifdef linux
#define OS_PATH_SEPARATOR "/"
#endif

#define PROG_NAME "Nanopipes"

#include <assert.h>

#define pritnf printf

#include <SFML/Graphics.hpp> // for sf::Vector2i
#define EAST sf::Vector2i({1,0})
#define WEST sf::Vector2i({-1,0})
#define SOUTH sf::Vector2i({0,1})
#define NORTH sf::Vector2i({0,-1})

#endif