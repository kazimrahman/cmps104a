#include "astree.h"
#include "symstack.h"
#include "lyutils.h"
#include "auxlib.h"
#include <string>
#include <iostream>

string mangle_struct(astree* node);
string mangle_ident(astree* node);
string new_vreg(char type);

void emit_oil(FILE* file, astree* root);
