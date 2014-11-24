#include <cstdlib>
#include <stack>
#include "symtable.h"
#include <typeinfo>
#include <iostream>
#include "yyparse.h"

size_t symstack::new_block(){
   ident_table.push_front(nullptr);
   return next_block++;
}

void symstack::leave_block(){
   ident_table.pop_front();
}

void symstack::define_ident(astree* ast, size_t blocknr){
   if (ident_table.front() == nullptr){
      ident_table.push_front(new symtable);
   }
   sym* symbol = new sym(ast, blocknr);
   syment sym_entry = syment(const_cast<string*>(ast->lexinfo), symbol);
   ident_table.front()->insert(sym_entry);
}

syment symstack::find_ident(string* id){
   for(auto iter = ident_table.begin(); iter != ident_table.end(); iter++){
      if((*iter)->count(id)){
         //find returns an iterator not the actual object
         return *(*iter)->find(id);
      }
   }
   return syment (nullptr, nullptr);
}

sym::sym(astree* ast, size_t blocknr){
   //ident_table constructor
   filenr = ast->filenr;
   linenr = ast->linenr;
   offset = ast->offset;
   blocknr = blocknr;
}

sym::sym(astree* ast){
   //type_table constructor
   filenr = ast->filenr;
   linenr = ast->linenr;
   offset = ast->offset;
   blocknr = 0;
}

void symstack::build_stack_rec(astree* root, int depth){
   for(vector<astree*>::iterator child = root->children.begin(); child != root->children.end(); child++){
      build_stack_rec(*child, depth+1);
   }
   //ready your anus
   build_sym(root);
   typecheck(root);
}

void symstack::build_stack(astree* root){
   build_stack_rec(root, 0);
}

//void symstack::typecheck(astree* root){
//   typecheck_rec(root, 0);
//}
//
//void typecheck_rec(astree* root, int depth){
//   for(vector<astree*>::iterator child = root->children.begin(); child != root->children.end(); child++){
//      typecheck_rec(*child, depth+1);
//   }
//
//}

sym* symstack::build_sym(astree* node){
   sym* symbol;
   switch(node->symbol){
      case TOK_VOID:
      case TOK_BOOL:
      case TOK_CHAR:
      case TOK_INT:
      case TOK_NULL:
      case TOK_STRING:
         symbol = type_var(node, node->symbol);
      case TOK_TYPEID:
         {
            //struct decl
            //insert struct type into typetable
            symbol = new sym(node);
            string* lexinfo_str = new string(*node->children[0]->lexinfo);
            syment symbol_entry = syment(lexinfo_str, symbol);
            type_table->insert(symbol_entry); 
            //insert fields into struct_table
            break;
         }
      case TOK_PROTOTYPE:
      case TOK_FUNCTION:
         {
            new_block();
            symbol = new sym(node, next_block);
            for(auto child = node->children.begin(); child != node->children.end(); child++){
               symbol->parameters->push_back(build_sym(*child));
            }
            leave_block();
            break;
         }

      default:
         fprintf(stderr, "Unkown token %d\n", node->symbol);
         break;
   }
   return symbol;
}

sym* symstack::type_var(astree* node, int tokid){
   sym* symbol = new sym(node, next_block);
   switch(tokid){
      case TOK_VOID:
         symbol->attribute = ATTR_void;
      case TOK_BOOL:
         symbol->attribute = ATTR_bool;
      case TOK_CHAR:
         symbol->attribute = ATTR_char;
      case TOK_INT:
         symbol->attribute = ATTR_int;
      case TOK_NULL:
         symbol->attribute = ATTR_null;
      case TOK_STRING:
         symbol->attribute = ATTR_string;
      case TOK_STRUCT:
         symbol->attribute = ATTR_struct;
      case TOK_ARRAY:
         symbol->attribute = ATTR_array;
      case TOK_FUNCTION:
         symbol->attribute = ATTR_function;
//      case TOK_VAR:
//         symbol->attribute = ATTR_variable;
      case TOK_FIELD:
         symbol->attribute = ATTR_field;
      case TOK_TYPEID:
         symbol->attribute = ATTR_typeid;
      case TOK_PARAM:
         symbol->attribute = ATTR_param;
      default: 
         fprintf(stderr, "No attribute assigned to tokid %d\n", tokid);

      string* lexinfo_str = new string(*node->children[0]->lexinfo);
      syment symbol_entry = syment(lexinfo_str, symbol);
      ident_table.front()->insert(symbol_entry); 
      
      return symbol;
   }
}
