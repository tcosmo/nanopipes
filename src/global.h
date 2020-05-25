#ifndef DEF_GLOBAL_H
#define DEF_GLOBAL_H

#ifdef _WIN32
#define OS_PATH_SEPARATOR "\\"
#endif

#ifdef linux
#define OS_PATH_SEPARATOR "/"
#endif

#define PROG_NAME "Nanopipes"

#define pritnf printf

#endif