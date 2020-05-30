#ifndef DEF_GLOBAL_H
#define DEF_GLOBAL_H

#ifdef _WIN32
#define OS_PATH_SEPARATOR "\\"
#endif

#ifdef linux
#define OS_PATH_SEPARATOR "/"
#endif

#define PROG_NAME "Nanopipes"

#include <set>
#include <assert.h>

#define pritnf printf

#include <SFML/Graphics.hpp> // for sf::Vector2i
#define EAST sf::Vector2i({1,0})
#define WEST sf::Vector2i({-1,0})
#define SOUTH sf::Vector2i({0,1})
#define NORTH sf::Vector2i({0,-1})

struct compare_world_positions 
{
    bool operator()(const sf::Vector2<int>& a, const sf::Vector2<int>& b) const {
        if(a.x == b.x)
            return a.y < b.y;
        return a.x < b.x;
    }
};

typedef std::set<sf::Vector2i, compare_world_positions> Poset;

struct compare_colors
{
    bool operator()(const sf::Color& a, const sf::Color& b) const {
        if(a.r == b.r) {
            if(a.g == b.g)
                return a.b < b.b;
            return a.g < b.g;
        }
        return a.r < b.r;
    }
};

#endif