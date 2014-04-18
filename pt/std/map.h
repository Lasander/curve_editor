#pragma once
#ifndef PT_STD_MAP_H
#define PT_STD_MAP_H

#include "../defs.h"
#include "iterator.h"

// std::map prints warnings on max warning level.

#pragma warning(push)
#pragma warning(disable: 4512)  // assignment operator could not be generated
#pragma warning(disable: 4702)  // unreachable code
#include <map>
#pragma warning(pop)

#endif
