#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include <bitset>
#include "astree.h"
struct symbol;

using symbol_table = unordered_map<string*,symbol*>;
using symbol_entry = pair<string*,symbol*>;

struct symbol{
   attr_bitset attr;
   symbol_table* fields;
   size_t filenr, linenr, offset;
   size_t blocknr;
   vector<symbol*>* parameters;
   size_t deffilenr, deflinenr, defoffset;
};

symbol* new_symbol(astree* node);
void st_insert(symbol_table* st,  astree* node);
symbol* st_lookup(symbol_table* st,  astree* node);
void dump(symbol_table* st);
