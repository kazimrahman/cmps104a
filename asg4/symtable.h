#include <cstdlib>
#include <bitset>
#include <unordered_map>
#include <vector>
#include <stack>
#include "astree.h"
using namespace std;

enum { ATTR_void, ATTR_bool, ATTR_char, ATTR_int, ATTR_null,
ATTR_string, ATTR_struct, ATTR_array, ATTR_function,
ATTR_variable, ATTR_field, ATTR_typeid, ATTR_param, ATTR_lval,
ATTR_const, ATTR_vreg, ATTR_vaddr, ATTR_bitset_size,
};

using attr_bitset = bitset<ATTR_bitset_size>;

class sym;

using syment = pair<string*,sym*>;
using symtable = unordered_map<string*, sym*>;

class sym{
   public:
      sym(astree* ast, size_t blocknr);
      attr_bitset attribute;
      symtable* fields;
      size_t filenr, linenr, offset;
      size_t blocknr;
      string lexinfo;
      stack<sym*>* parameters; 
};


class symstack{
   size_t next_block = 1;
   stack <symtable*> s;
   void define_ident(astree* ast, size_t blocknr);
   syment find_ident(string id);
   size_t new_block();
   void leave_block();

};
