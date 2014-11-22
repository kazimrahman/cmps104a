#include <cstdlib>
#include <stack>
#include "symtable.h"
#include <typeinfo>
#include <iostream>
#include "yyparse.h"

size_t symstack::new_block(){
   s.push_front(nullptr);
   return next_block++;
}

void symstack::leave_block(){
   s.pop_front();
}

void symstack::define_ident(astree* ast, size_t blocknr){
   if (s.front() == nullptr){
      s.push_front(new symtable);
   }
   sym* symbol = new sym(ast, blocknr);
   syment sym_entry = syment(const_cast<string*>(ast->lexinfo), symbol);
   s.front()->insert(sym_entry);
}

syment symstack::find_ident(string* id){
   for(auto iter = s.begin(); iter != s.end(); ++iter){
      if((*iter)->count(id)){
         //find returns an iterator not the actual object
         return *(*iter)->find(id);
      }
   }
   return syment (nullptr, nullptr);
}

sym::sym(astree* ast, size_t blocknr){
   filenr = ast->filenr;
   linenr = ast->linenr;
   offset = ast->offset;
   blocknr = blocknr;
}

sym::sym(astree* ast){
   filenr = ast->filenr;
   linenr = ast->linenr;
   offset = ast->offset;
   blocknr = 0;
}

void symstack::build_stack_rec(astree* root, int depth){
   for(auto child = root->children.begin(); child != root->children.end(); ++root){
      build_stack_rec(*child, depth+1);
   }
   //ready your anus
   if (root->symbol == TOK_TYPEID){
      sym *struct_sym = new sym(root);
      string* lexinfo_str = new string(*root->lexinfo);
      syment symbol_entry = syment(lexinfo_str, struct_sym);
      struct_table.insert(symbol_entry); 
   }

}

void symstack::build_stack(astree* root){
   build_stack_rec(root, 0);
}
