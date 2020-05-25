#include "routines.h"

std::string path_join(const std::vector<std::string>& to_join)
{
    std::string to_ret = "";
    for( auto s : to_join ) {
        to_ret += s + std::string(OS_PATH_SEPARATOR);
    }
    return to_ret.substr(0, to_ret.size()-1);
}