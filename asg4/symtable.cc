#include "symtable.h"
#include "astree.h"

symbol* new_symbol(astree* node){
   symbol* sym = new symbol();
   sym->filenr = node->filenr;
   sym->linenr = node->linenr;
   sym->blocknr = node->blocknr;
   return sym;
}

void st_insert(symbol_table st, astree* node){
   symbol* sym = new_symbol(node);
   symbol_entry ent = symbol_entry(const_cast<string*>(node->lexinfo), sym);
   st.insert(ent);
}

symbol* st_lookup(symbol_table st, astree* node){
   string* lexinfo = const_cast<string*>(node->lexinfo);
   if(st.count(lexinfo))
      return nullptr;
   symbol_entry ent = *st.find(lexinfo);
   return ent.second;
}
