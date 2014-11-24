#include <cstdlib>
#include <stack>
#include "symtable.h"
#include <typeinfo>
#include <iostream>
#include "lyutils.h"

void symstack::new_block(astree* node){
   ident_table.push_front(nullptr);
   block_stack.push(++next_block);
   node->blocknr = next_block;
}

void symstack::leave_block(){
   ident_table.pop_front();
   block_stack.pop();
}

sym* symstack::define_ident(astree* ast){
   if (ident_table.front() == nullptr){
      ident_table.push_front(new symtable);
   }
   sym* symbol = new sym(ast, next_block);
   syment sym_entry = syment(const_cast<string*>(ast->lexinfo), symbol);
   ident_table.front()->insert(sym_entry);
   return symbol;
}

sym* symstack::find_ident(const string* id){
   std::cout<<"FINDING "<<id<<std::endl;
   string* s = const_cast<string*>(id);
   for(auto table : ident_table){
      //might be in a new block, in which case stack top will be null
      if(table == nullptr) continue;
      std::cout<<"ITER "<<table<<std::endl;
      if(table->count(s)){
         return table->at(s);
      }
   }
   return nullptr;
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
   for(auto child : root->children){
      build_stack_rec(child, depth+1);
   }
   //ready your anus
   //build_sym(root);
   //typecheck(root);
}

void symstack::build_stack(astree* root){
   //build_stack_rec(root, 0);
   //init our stacks
//   struct_table.push_front(new symtable);
//   ident_table.push_front(new symtable);
   build_sym(root);
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
//

void symstack::node_error(astree* node, string msg){
   fprintf(stderr, "error: %zu %zu %zu: %s", 
      node->filenr, node->linenr, node->offset, msg.c_str());
   set_exitstatus(4);
}

sym* symstack::build_sym(astree* node){
   sym* symbol;
   switch(node->symbol){
      case TOK_ROOT:
         for(auto child : node->children)
            build_sym(child);
         break;
      case TOK_VOID:
      case TOK_BOOL:
      case TOK_CHAR:
      case TOK_INT:
      case TOK_NULL:
      case TOK_STRING:
         {
            symbol = type_var(node, node->symbol);
            //string* lexinfo_str = new string(*node->children[0]->lexinfo);
            syment* symbol_entry = new syment(const_cast<string*>(node->children[0]->lexinfo), symbol);
            ident_table.front()->insert(*symbol_entry);
            define_ident(node->children[0]);
            break;
         }
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
      case TOK_DECLID:
         {
            sym* entry = find_ident(node->lexinfo);
            if(entry){
               symbol = entry;
            }
            else{
               define_ident(node->children[0]);
            }
            break;
         }
      case TOK_VARDECL:
         {
            symbol = find_ident(node->lexinfo);
            if (symbol != nullptr){
               node_error(node, "Duplicate variable declaration");
            }
         }
      case TOK_PROTOTYPE:
      case TOK_FUNCTION:
         {
            new_block(node);
            symbol = new sym(node, next_block);
            for(auto child : node->children){
               symbol->parameters->push_back(build_sym(child));
            }
            leave_block();
            break;
         }

      case TOK_BLOCK:
         {
            new_block(node);
            for(auto child : node->children)
               build_sym(child);
            leave_block();
            break;
         }
            
      default:
         fprintf(stderr, "Unkown token %s\n", get_yytname(node->symbol));
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
         fprintf(stderr, "No attribute assigned to token %s\n", get_yytname(tokid));

      string* lexinfo_str = new string(*node->children[0]->lexinfo);
      syment symbol_entry = syment(lexinfo_str, symbol);
      ident_table.front()->insert(symbol_entry); 
      
      return symbol;
   }
}

void symstack::dump_stack(FILE* out){
   fprintf(out, "Output\n");
}
