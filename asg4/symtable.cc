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
      printf("Build stack: %s\n", get_yytname(child->symbol));
      build_stack_rec(child, depth+1);
   }
   printf("Build sym: %s\n", get_yytname(root->symbol));
   build_sym(root);
}

void symstack::build_stack(astree* root){
   build_stack_rec(root, 0);
}

void symstack::node_error(astree* node, string msg){
   fprintf(stderr, "error: %zu %zu %zu: %s", 
      node->filenr, node->linenr, node->offset, msg.c_str());
   set_exitstatus(4);
}

sym* symstack::build_sym(astree* node){
   sym* symbol;
   printf("switch: %s\n", get_yytname(node->symbol));
   switch(node->symbol){
      case TOK_ROOT:
         //for(auto child : node->children)
         //   build_sym(child);
         break;
//      case TOK_VOID:
      case TOK_BOOL:
      case TOK_CHAR:
      case TOK_INT:
      case TOK_NULL:
      case TOK_STRING:
         {
            //symbol = type_var(node);
            type_var(node);
            //string* lexinfo_str = new string(*node->children[0]->lexinfo);
            break;
         }
      case TOK_TYPEID:
         {
            break;
         }
      case TOK_DECLID:
         {
            sym* entry = find_ident(node->lexinfo);
            if(entry){
               symbol = entry;
            }
            else{
               define_ident(node);
            }
            break;
         }
      case TOK_VARDECL:
         {
            symbol = find_ident(node->lexinfo);
            if (symbol == nullptr){
               define_ident(node->children[0]);
            }
            else{
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

void symstack::type_var(astree* node){
   switch(node->symbol){
      case TOK_VOID:
         node->attr.set(ATTR_void);
			break;
      case TOK_BOOL:
         node->attr.set(ATTR_bool);
			break;
      case TOK_CHAR:
         node->attr.set(ATTR_char);
			break;
      case TOK_INT:
         node->attr.set(ATTR_int);
			break;
      case TOK_NULL:
         node->attr.set(ATTR_null);
			break;
      case TOK_STRING:
         node->attr.set(ATTR_string);
			break;
      case TOK_STRUCT:
         node->attr.set(ATTR_struct);
			break;
      case TOK_ARRAY:
         node->attr.set(ATTR_array);
			break;
      case TOK_FUNCTION:
         node->attr.set(ATTR_function);
			break;
//      case TOK_VAR:
//         node->attr.set(ATTR_variable;
//			break;
      case TOK_FIELD:
         node->attr.set(ATTR_field);
			break;
      case TOK_TYPEID:
         node->attr.set(ATTR_typeid);
			break;
      case TOK_PARAM:
         node->attr.set(ATTR_param);
			break;
      default: 
         fprintf(stderr, "No attribute assigned to token %s\n", get_yytname(node->symbol));
			break;
   }
}

void symstack::dump_stack(FILE* out){
   fprintf(out, "Output\n");
}
