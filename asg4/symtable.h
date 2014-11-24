#include <cstdlib>
#include <bitset>
#include <unordered_map>
#include <vector>
#include <stack>
#include <deque>
#include "astree.h"
using namespace std;

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
      deque<sym*>* parameters; 
};

class symstack{
   public:
      void build_stack(astree* root);
      void dump_stack(FILE* out);
   private:
      size_t next_block = 1;
      stack <int> block_stack;
      deque <symtable*> struct_table;
      deque <symtable*> ident_table;
      symtable* type_table;

      sym* define_ident(astree* ast);
      sym* find_ident(const string* id);
      void new_block(astree* node);
      void leave_block();

      void build_stack_rec(astree* root, int depth);
      
      //changes exitval
      void typecheck(astree* node);
      //for simple token typing
      void type_var(astree* node);
      
      //does the right thing with each token type
      sym* build_sym(astree* node);
      void node_error(astree* node, string msg);

};
