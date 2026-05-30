#include "avm_loader.hpp"

#include <iostream>
#include <cstdio>
#include <cstdlib>

instruction* instructions = nullptr;
unsigned totalInstructions=0;
unsigned currInstructions=0;
double* numConsts=nullptr;
unsigned totalNumConsts=0;
char** stringConsts=nullptr;
unsigned totalStringConsts=0;
char** namedLibfuncs=nullptr;
unsigned totalNamedLibfuncs=0;
userfunc* userFuncs = nullptr;
unsigned totalUserFuncs=0;
unsigned programVarOffset=0;

bool debug_flag = false;

bool load_binary(int argc, char* argv[]);