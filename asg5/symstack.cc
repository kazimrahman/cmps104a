#include "symstack.h"
#include <iostream>

void symbol_stack::enter_block(){
   ++next_block;
   stack.push_back(nullptr);
}

void symbol_stack::leave_block(){
   stack.pop_back();
}

void symbol_stack::define_ident(astree* node){
   if(stack.back() == nullptr)
      stack.back() = new symbol_table;
   st_insert(stack.back(), node);
}

symbol* symbol_stack::lookup_ident(astree* node){
   for(auto sym_table : stack){
      if(sym_table == nullptr || sym_table->empty())
         continue;
      symbol* sym = st_lookup(sym_table, node);
      if(sym != nullptr)
         return sym;
   }
   return nullptr;
}

void symbol_stack::dump(){
   int i = 0;
   for(auto sym_table : stack){
      cout<<"Table "<<i++<<endl;
      if(sym_table == nullptr)
         continue;
      ::dump(sym_table);
   }
}
