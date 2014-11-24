#include <cstdlib>
#include <bitset>
#include <unordered_map>
#include <vector>
#include <stack>
#include <deque>
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
      //this constructor for structs
      sym(astree* ast);
      attr_bitset attribute;
      symtable* fields;
      size_t filenr, linenr, offset;
      size_t blocknr;
      string lexinfo;
      deque<sym*>* parameters; 
};

class symstack{
   public:
      void build_stack(astree* root);
   private:
      size_t next_block = 1;
      deque <symtable*> struct_table;
      deque <symtable*> ident_table;
      symtable* type_table;
      void define_ident(astree* ast, size_t blocknr);
      syment find_ident(string* id);
      size_t new_block();
      void leave_block();

      void build_stack_rec(astree* root, int depth);
      
      //changes exitval
      void typecheck(astree* node);
      //for simple token typing
      sym* type_var(astree* node, int tokid);
      
      //does the right thing with each token type
      sym* build_sym(astree* node);

};
