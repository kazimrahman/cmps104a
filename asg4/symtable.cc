#include <cstdlib>
#include <stack>
#include "symtable.h"
#include <typeinfo>
#include <iostream>

size_t symstack::new_block(){
   s.push(nullptr);
   return next_block++;
}

void symstack::leave_block(){
   s.pop();
}

void symstack::define_ident(astree* ast, size_t blocknr){
   if (s.top() == nullptr){
      s.push(new symtable);
   }
   sym* symbol = new sym(ast, blocknr);
   syment sym_entry = syment(const_cast<string*>(ast->lexinfo), symbol);
   s.top()->insert(sym_entry);
}

syment symstack::find_ident(string id){
   
}

sym::sym(astree* ast, size_t blocknr){
   filenr = ast->filenr;
   linenr = ast->linenr;
   offset = ast->offset;
   blocknr = blocknr;
}
