#include "typecheck.h"

bool compare_prim(astree* left, astree* right){
   for(size_t i = 0; i < attr_function; ++i){
      if(left->attr[i]==1 && right->attr[i]==1)
         return true;
   }
   return false;
}

bool compare_ref(astree* left, astree* right){
   if(left->struct_table == nullptr || right->struct_table == nullptr)
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
      if(parent->attr[i])
         child->attr[i] = 1;   
   }
}


void type_check_body(astree* node, symbol_stack s, size_t depth){
   astree* lchild = nullptr;
   astree* rchild = nullptr;
   symbol *sym;
   if(node->children.size() >= 1)
      lchild = node->children[0];
   if(node->children.size() >= 2)
      rchild = node->children[1];
   switch(node->symbol){
      case TOK_IDENT:
         sym = s.lookup_ident(node);
         if(sym == nullptr)
            errprintf("Error %d %d %d: Reference to undefined variable", node->filenr, node->linenr, node->offset);
         break;
      case TOK_STRUCT:
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
            errprintf("Error %d %d %d: Invalid types", node->filenr, node->linenr, node->offset);
            break;
         }
      case TOK_NE:
      case TOK_EQ:
         {
            if(compare_prim(lchild, rchild) || compare_ref(lchild, rchild)){
               adopt_type(node, lchild);
               node->attr[attr_vreg] = 1;
               return;
            }
            errprintf("Error %d %d %d: Invalid types", node->filenr, node->linenr, node->offset);
            break;
         }
      case '=':
         {
            if(lchild->attr[attr_lval] && rchild->attr[attr_vreg]){
               adopt_type(node, lchild); 
               node->attr[attr_vreg] = 1;
               return;
            }
            errprintf("Error %d %d %d: Invalid types", node->filenr, node->linenr, node->offset);
            break;
         }
      case '+':
      case '-':
         {
            node->attr[attr_vreg] = 1;
            node->attr[attr_int] = 1;
            if(rchild == nullptr){
               //unary operator
               if(lchild->symbol != TOK_INT){
                  errprintf("Error %d %d %d: Non int arithmetic operator", node->filenr, node->linenr, node->offset);
               }
            }
            else{
               if(!(lchild->symbol == TOK_INT && rchild->symbol == TOK_INT)){
                  errprintf("Error %d %d %d: Non int arithmetic operator", node->filenr, node->linenr, node->offset);
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
            if(!(lchild->symbol == TOK_INT && rchild->symbol == TOK_INT)){
               errprintf("Error %d %d %d: Non int with arithmetic operator", node->filenr, node->linenr, node->offset);
            }
            break;
         }
      case '!':
         node->attr[attr_bool] = 1;
         node->attr[attr_vreg] = 1;
         if(!(lchild->symbol == TOK_TRUE || lchild->symbol == TOK_FALSE)){
            errprintf("Error %d %d %d: Non bool with bool operator", node->filenr, node->linenr, node->offset);
         }
         break;
      case TOK_ORD:
         node->attr[attr_vreg] = 1;
         node->attr[attr_char] = 1;
         if(lchild->symbol != TOK_INT) 
            errprintf("Error %d %d %d: Cannot make non int type into ord", node->filenr, node->linenr, node->offset);
         break;
      //NEED TO DO TYPEIDS AND STRINGS AND ARRAYS
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
         if(!node->child[0]->attr[attr_bool])
            errprintf("Error %d %d %d: If or while must be bool", node->filenr, node->linenr, node->offset);
      default:
         errprintf("Invalid symbol %s\n", get_yytname(node->symbol));
         
   }
   
}

void type_check_rec(astree* root, symbol_stack s, size_t depth){
   for(auto child : root->children){
      type_check_rec(child, s, depth+1);
   }
   type_check_body(root, s, depth);
}

void type_check(astree* root, symbol_stack s){
   type_check_rec(root, s, 0);
}
