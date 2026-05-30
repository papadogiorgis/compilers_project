#ifndef AVM_LOADER_HPP
#define AVM_LOADER_HPP

#include "../compiler/inc/vm_target_code.h"

extern unsigned totalInstructions;
extern unsigned currInstructions;
extern unsigned totalNumConsts;
extern unsigned totalStringConsts;
extern unsigned totalNamedLibfuncs;
extern unsigned totalUserFuncs;
extern unsigned programVarOffset;

extern bool debug_flag;

bool load_binary(int argc, char* argv[]);

#endif