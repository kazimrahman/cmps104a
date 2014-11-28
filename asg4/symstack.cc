#include "symstack.h"
void symbol_stack::enter_block(){
   ++next_block;
   stack.push_back(nullptr);
}

void symbol_stack::leave_block(){
   stack.pop_back();
}

void symbol_stack::define_ident(astree* node){
   if(stack.back() == nullptr)
      stack.push_back(new symbol_table);
   st_insert(*stack.back(), node);
}

symbol* symbol_stack::lookup_ident(astree* node){
   for(auto sym_table : stack){
      if(sym_table == nullptr)
         continue;
      return st_lookup(*sym_table, node);
   }
   return nullptr;
}
