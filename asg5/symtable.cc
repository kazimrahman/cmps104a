#include "symtable.h"
#include <iostream>
#include "astree.h"

symbol* new_symbol(astree* node){
   symbol* sym = new symbol();
   sym->filenr = node->filenr;
   sym->linenr = node->linenr;
   sym->blocknr = node->blocknr;
   sym->attr = node->attr;
   sym->parameters = nullptr;
   return sym;
}

void st_insert(symbol_table* st,  astree* node){
   symbol* sym = new_symbol(node);
   symbol_entry ent = symbol_entry(
      const_cast<string*>(node->lexinfo), sym);
   st->insert(ent);
}

symbol* st_lookup(symbol_table* st,  astree* node){
   string* lexinfo = const_cast<string*>(node->lexinfo);
   if(!st->count(lexinfo))
      return nullptr;
   symbol_entry ent = *st->find(const_cast<string*>(node->lexinfo));
   node->deflinenr = ent.second->linenr;
   node->deffilenr = ent.second->filenr;
   node->defoffset = ent.second->offset;
   return ent.second;
}

void dump(symbol_table* st){
   cout<<"\tTable size: "<<st->size()<<endl;
   int i = 0;
   for(auto iter = st->begin(); iter != st->end(); iter++){
      cout<<"\tEntry "<<i++<<" "<<*(*iter).first<<endl;
   }
}
