#include "typecheck.h"

bool compare_prim(astree* left, astree* right){
   for(size_t i = 0; i < attr_function; ++i){
      if(left->attr[i]==1 && right->attr[i]==1)
         return true;
   }
   return false;
}

bool compare_ref(astree* left, astree* right){
   if(left->struct_table == nullptr || 
      right->struct_table == nullptr)
      return false;
   //compare typeid
   if(left->children[0]->lexinfo == right->children[0]->lexinfo)
      return true;
   if(right->attr[attr_null])
      return true;
   return false;
}

void adopt_type(astree* parent, astree* child){
   for(size_t i=0; i < attr_function; ++i){
      if(child->attr[i])
         parent->attr[i] = 1;   
   }
}

void adopt_attrs(astree* parent, astree* child){
   for(size_t i=0; i < attr_bitset_size; ++i){
      if(child->attr[i])
         parent->attr[i] = 1;   
   }
}

void block_recurse(astree* node, symbol_stack* s){
   //make sure to leave block after using this function
   if(node->symbol == TOK_BLOCK)
      s->enter_block();
   node->blocknr = next_block;
   for(auto child : node->children){
      block_recurse(child, s);
   }
}

void func_recurse(astree* node, symbol_stack* s){
   symbol* func_sym = new_symbol(node);
   st_insert(s->stack[0],  node->children[0]->children[0]);
   adopt_attrs(node, node->children[0]);
   s->enter_block();
   func_sym->parameters = new vector<symbol*>;
   for(auto child : node->children[1]->children){
      //symbol* sym = s->lookup_ident(child);
      //func_sym->parameters->push_back(sym);
      s->define_ident(child);
   }
   if(func_sym != nullptr){
      if(func_sym->parameters == nullptr){
         errprintf("Error %d %d %d: \
            Function name %s is identical to identifier\n",
            node->filenr, node->linenr, node->offset, 
            node->lexinfo->c_str());
         return;
      }
      s->define_ident(node->children[0]->children[0]);
      block_recurse(node->children[2], s);
      s->leave_block();
      s->leave_block();

   }
}

void proto_recurse(astree* node, symbol_stack* s){
   symbol* func_sym = new_symbol(node);
   //s->define_ident(node->children[0]->children[0]);
   st_insert(s->stack[0],  node->children[0]->children[0]);
   s->enter_block();
   adopt_attrs(node, node->children[0]);
   func_sym->parameters = new vector<symbol*>;
   for(auto child : node->children[1]->children){
      symbol* sym = s->lookup_ident(child);
      func_sym->parameters->push_back(sym);
   }
   if(func_sym != nullptr){
      if(func_sym->parameters == nullptr){
         errprintf("Error %d %d %d: \
            Function name %s is identical to identifier\n",
            node->filenr, node->linenr, node->offset, 
            node->lexinfo->c_str());
         return;
      }
   }
   s->leave_block();
}


void type_check_body(astree* node, symbol_stack* s, 
   symbol_table *type_table, size_t depth){
   astree* lchild = nullptr;
   astree* rchild = nullptr;
   symbol *sym;
   if(node->children.size() >= 1)
      lchild = node->children[0];
   if(node->children.size() >= 2)
      rchild = node->children[1];
   switch(node->symbol){
      case TOK_ROOT:
      case TOK_DECLID:
      case TOK_FIELD:
      case TOK_PARAM:
         break;
      case TOK_NEW:
         adopt_attrs(node, lchild); 
         break;
      case TOK_CALL:
         {
            sym = st_lookup(s->stack[0],  node->children.back());
            if(sym == nullptr){
               errprintf("Error %d %d %d:"
                  "No matching function %s\n",
                  node->filenr, node->linenr, node->offset, 
                  node->children.back()->lexinfo->c_str());
               break;
            }
            for(size_t i=0; i < attr_bitset_size; ++i){
               if(sym->attr[i])
                  node->attr[i] = 1;   
            }
            break;
         }
      case TOK_FUNCTION:
           func_recurse(node, s); 
           break;
      case TOK_PROTOTYPE:
            proto_recurse(node, s);
         break;
      case '.':
         node->attr[attr_vaddr];
         node->attr[attr_lval];
      case TOK_TYPEID:
         node->attr[attr_typeid] = 1;
         break;
      case TOK_VOID:
         lchild->attr[attr_void] = 1;
         break;
      case TOK_BOOL:
         if(lchild == nullptr)
            break;
         lchild->attr[attr_bool] = 1;
         adopt_attrs(node, lchild);
         break;
      case TOK_INT:
         if(lchild == nullptr)
            break;
         lchild->attr[attr_int] = 1;
         adopt_attrs(node, lchild);
         break;
      case TOK_CHAR:
         if(lchild == nullptr)
            break;
         lchild->attr[attr_char] = 1;
         adopt_attrs(node, lchild);
         break;
      case TOK_STRING:
         if(lchild == nullptr)
            break;
         lchild->attr[attr_string] = 1;
         adopt_attrs(node, lchild);
         break;
      case TOK_ARRAY:
         lchild->attr[attr_array];
         if(lchild == nullptr || lchild->children.empty())
            break;
         lchild->children[0]->attr[attr_array] = 1;
         break;
      case TOK_VARDECL:
         lchild->children[0]->attr[attr_lval] = 1;
         adopt_attrs(node, lchild);
         if(s->lookup_ident(lchild->children[0]))
            errprintf("Error %d %d %d: Duplicate declaration %s\n",
               node->filenr, node->linenr, node->offset, 
               lchild->children[0]->lexinfo->c_str());
         s->define_ident(lchild->children[0]);
         break;
      case TOK_IDENT:
         sym = s->lookup_ident(node);
         if(sym == nullptr){
            errprintf("Error %d %d %d: Reference to undefined "
               "variable %s\n", node->filenr, node->linenr,
               node->offset, node->lexinfo->c_str());
            break;
         }
         node->attr = sym->attr;
         break;
      case TOK_STRUCT:
         {
            st_insert(type_table,  lchild);            
            symbol* sym = st_lookup(type_table,  lchild);
            sym->fields = new symbol_table;
            for(auto child = node->children.begin()+1;
               child != node->children.end(); ++child){
               st_insert(sym->fields,  *child);
            }
         break;
         }
      case TOK_BLOCK:
         block_recurse(node, s);
         s->leave_block();
         break;
      case TOK_LT:
      case TOK_LE:
      case TOK_GE:
      case TOK_GT:
         {
            lchild = node->children[0];
            rchild = node->children[1];
            if(compare_prim(lchild, rchild)){
               node->attr.set(attr_bool);
               return;
            }
            errprintf("Error %d %d %d: Invalid types\n",
               node->filenr, node->linenr, node->offset);
            break;
         }
      case TOK_NE:
      case TOK_EQ:
         {
            if(compare_prim(lchild, rchild) || 
               compare_ref(lchild, rchild)){
               adopt_type(node, lchild);
               node->attr[attr_vreg] = 1;
               return;
            }
            errprintf("Error %d %d %d: Invalid types\n",
               node->filenr, node->linenr, node->offset);
            break;
         }
      case '=':
         {
            if(lchild == nullptr)
               break;
            if(lchild->attr[attr_lval] && 
               rchild->attr[attr_vreg]){
               adopt_type(node, lchild); 
               node->attr[attr_vreg] = 1;
               return;
            }
            errprintf("Error %d %d %d: Invalid types\n", 
               node->filenr, node->linenr, node->offset);
            break;
         }
      case '+':
      case '-':
         {
            node->attr[attr_vreg] = 1;
            node->attr[attr_int] = 1;
            if(rchild == nullptr){
               //unary operator
               if(lchild == nullptr)
                  break;
               if(!lchild->attr[attr_int]){
                  errprintf("Error %d %d %d: "
                     "Non int arithmetic operator\n", 
                     node->filenr, node->linenr, node->offset);
               }
            }
            else{
               if(!lchild->attr[attr_int] || 
                  !rchild->attr[attr_int]){
                  errprintf("Error %d %d %d: "
                     "Non int arithmetic operator\n", 
                     node->filenr, node->linenr, node->offset);
               }
            }
               break;
         }
      case '/':
      case '*':
      case '%':
         {
            node->attr[attr_vreg] = 1;
            node->attr[attr_int] = 1;
            if(!lchild->attr[attr_int] || 
               !rchild->attr[attr_int]){
               errprintf("Error %d %d %d: "
                  "Non int with arithmetic operator\n", 
                  node->filenr, node->linenr, node->offset);
            }
            break;
         }
      case '!':
         node->attr[attr_bool] = 1;
         node->attr[attr_vreg] = 1;
         if(!(lchild->attr[attr_bool])){
            errprintf("Error %d %d %d: "
               "Non bool with bool operator\n",
               node->filenr, node->linenr, node->offset);
         }
         break;
      case TOK_ORD:
         node->attr[attr_vreg] = 1;
         node->attr[attr_char] = 1;
         if(lchild->attr[attr_int]) 
            errprintf("Error %d %d %d: "
               "Cannot make non int type into ord\n", 
               node->filenr, node->linenr, node->offset);
         break;
      case TOK_INTCON:
         node->attr[attr_int] = 1;
         node->attr[attr_const] = 1;
         break;
      case TOK_CHARCON:
         node->attr[attr_char] = 1;
         node->attr[attr_const] = 1;
         break;
      case TOK_STRINGCON:
         node->attr[attr_string] = 1;
         node->attr[attr_const] = 1;
         break;
      case TOK_FALSE:
      case TOK_TRUE:
         node->attr[attr_bool] = 1;
         node->attr[attr_const] = 1;
         break;
      case TOK_NULL:
         node->attr[attr_null] = 1;
         node->attr[attr_const] = 1;
         break;
      case TOK_WHILE:
      case TOK_IF:
         if(!lchild->attr[attr_bool])
            errprintf("Error %d %d %d: "
               "If or while must be bool\n", 
               node->filenr, node->linenr, node->offset);
         break;
      default:
         errprintf("Invalid symbol %s\n", 
            get_yytname(node->symbol));
   }  
}

void type_check_rec(astree* root, symbol_stack* s, 
   symbol_table *type_table, size_t depth){
   for(auto child : root->children){
      type_check_rec(child, s, type_table, depth+1);
   }
   type_check_body(root, s, type_table, depth);
}

void type_check(astree* root, symbol_stack* s, 
   symbol_table *type_table){
   type_check_rec(root, s, type_table, 0);
}
