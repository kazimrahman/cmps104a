#ifndef __symtable_h
#define __symtable_h
#include "symtable.h"
extern size_t next_block;

class symbol_stack{
   public:
      vector<size_t> block_stack;
      vector<symbol_table*> stack;
      void enter_block();

      void leave_block();

      void define_ident(astree* node);
      symbol* lookup_ident(astree* node);
      void dump();
};
#endif
