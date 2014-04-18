#pragma once
#ifndef PT_STD_STRING_H
#define PT_STD_STRING_H

#include "../defs.h"

// std::string prints warnings on max warning level.

#pragma warning(push)
//#pragma warning(disable: 4018)  // signed/unsigned mismatch
//#pragma warning(disable: 4100)  // unreferenced formal parameter
//#pragma warning(disable: 4702)  // unreachable code
//#pragma warning(disable: 4511)  // copy constructor could not be generated
//#pragma warning(disable: 4512)  // assignment operator could not be generated
#include <string>
#pragma warning(pop)

#endif
