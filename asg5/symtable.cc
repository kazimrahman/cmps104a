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
   sym->vreg = node->vreg;
   return sym;
}

void st_insert(symbol_table* st,  astree* node){
   symbol* sym = new_symbol(node);
   symbol_entry ent = symbol_entry(
      const_cast<string*>(node->lexinfo), sym);
   st->insert(ent);
   //symbol_entry* s = st->find(const_cast<string*>(node->lexinfo));
   symbol_entry s = *st->find(ent.first);
   //cout<<"insert "<<*node->lexinfo<<" "<<s.second->filenr<< " "
   //<<s.second->linenr<<" "<<s.second->offset<<endl;
}

symbol* st_lookup(symbol_table* st,  astree* node){
   string* lexinfo = const_cast<string*>(node->lexinfo);
   if(!st->count(lexinfo))
      return nullptr;
   symbol_entry ent = *st->find(const_cast<string*>(node->lexinfo));
   node->deflinenr = ent.second->linenr;
   node->deffilenr = ent.second->filenr;
   node->defoffset = ent.second->offset;
   //cout<<"lookup "<<*node->lexinfo<<" "<<ent.second->filenr
   //<<" "<<ent.second->linenr<<" "<<ent.second->offset<<endl;
   return ent.second;
}

void dump(symbol_table* st){
   cout<<"\tTable size: "<<st->size()<<endl;
   int i = 0;
   for(auto iter = st->begin(); iter != st->end(); iter++){
      cout<<"\tEntry "<<i++<<" "<<*(*iter).first<<endl;
   }
}
