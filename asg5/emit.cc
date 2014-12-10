#include "emit.h"

size_t ptr_count = 0;
size_t char_count = 0;
size_t string_count = 0;
size_t int_count = 0;
size_t ind_ptr_count = 0;
symbol_stack symstack;
void emit_node(FILE* file, astree* node);
void emit_expr(FILE* file, astree* node);

void indent(FILE* file){
   fprintf(file, "        ");
}
string new_vreg(char type){
   string builder;
   switch(type){
      case 'p':
         builder += type;
         builder += to_string(++ptr_count);
         break;
      case 'c':
         builder += type;
         builder += to_string(++char_count);
         break;
      case 'i':
         builder += type; 
         builder += to_string(++int_count);
         break;
      case 'a':
         builder += type; 
         builder += to_string(++ind_ptr_count);
         break;
      case 's':
         builder += type;
         builder += to_string(++string_count);
         break;
      default:
      errprintf("Invalid register type: %c \n", type);
   }
   return builder;
}

void emit_while(FILE* file, astree* node){
   fprintf(file, "while_%zu_%zu_%zu:;\n",
      node->filenr, node->linenr, node->offset);
   switch(node->children[0]->symbol){
      case TOK_IDENT:
      case TOK_CHARCON:
      case TOK_INTCON:
         break;
      default:
         indent(file);
         emit_expr(file, node->children[0]);
   }
   indent(file);
   fprintf(file,
      "if (!%s) goto break_%zu_%zu_%zu\n", 
      node->children[0]->vreg.c_str(), 
      node->children[1]->filenr,
      node->children[1]->linenr,
      node->children[1]->offset);
   emit_node(file, node->children[1]);
   indent(file);
   fprintf(file,
      "goto while_%zu_%zu_%zu\n", 
      node->filenr, 
      node->linenr, 
      node->offset);
   fprintf(file,
      "break_%zu_%zu_%zu):;\n",
      node->children[1]->filenr,
      node->children[1]->linenr,
      node->children[1]->offset);
}

void emit_if(FILE* file, astree* node){
   switch(node->children[0]->symbol){
      case TOK_IDENT:
      case TOK_CHARCON:
      case TOK_INTCON:
         break;
      default:
         indent(file);
         emit_expr(file, node->children[0]);
   }
      fprintf(file,
         "if (!%s) goto fi_%zu_%zu_%zu\n", 
         node->children[0]->vreg.c_str(), 
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);
      for(auto child : node->children[0]->children){
         indent(file);
         emit_node(file, child);
      }
      indent(file);
      fprintf(file,
         "fi_%zu_%zu_%zu:;\n",
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);

}

void emit_ifelse(FILE* file, astree* node){
   switch(node->children[0]->symbol){
      case TOK_IDENT:
      case TOK_CHARCON:
      case TOK_INTCON:
         break;
      default:
         indent(file);
         emit_expr(file, node->children[0]);
   }
      fprintf(file,
         "if (!%s) goto else_%zu_%zu_%zu\n", 
         node->children[0]->vreg.c_str(), 
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);
      for(auto child : node->children[0]->children){
         indent(file);
         emit_node(file, child);
      }
      indent(file);
      fprintf(file,
         "goto fi_%zu_%zu_%zu:;\n",
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);
      fprintf(file,
         "else_%zu_%zu_%zu:;\n",
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);
      for(auto child : node->children[1]->children){
         indent(file);
         emit_node(file, child);
      }
      fprintf(file,
         "fi_%zu_%zu_%zu:;\n",
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);

}

void emit_goto(astree* node){
//   fprintf(file, 
}

void emit_expr(FILE* file, astree* node){
   bool unary = false;
   bool nunary = false;
   enum{left, right};
   symbol* tree[2];
   if(!(node->symbol == TOK_BLOCK && *node->lexinfo == ";")){
      printf("node: %s lex: %s\n", get_yytname(node->symbol), node->lexinfo->c_str());
      fflush(NULL);
      if(node->children.empty())
         nunary = true;
      if(!node->children.empty() && node->children[1] == nullptr){
         unary = true;
         tree[left] = symstack.lookup_ident(node->children[0]);
         if(tree[left] != nullptr){
            if(tree[left]->vreg.empty())
               tree[left] = nullptr;
         }
      }
      if(node->children.size()>1){
         nunary = unary = false;
         tree[left] = symstack.lookup_ident(node->children[0]);
         tree[right] = symstack.lookup_ident(node->children[1]);
         if(tree[left] != nullptr){
            if(tree[left]->vreg.empty())
               tree[left] = nullptr;
         }
         if(tree[right] != nullptr){
            if(tree[right]->vreg.empty())
               tree[right] = nullptr;
         }
      }
      if(node->attr[attr_bool])
         node->vreg = new_vreg('c');
      else if (node->attr[attr_int])
         node->vreg = new_vreg('i');
      else if (node->attr[attr_struct])
         node->vreg = new_vreg('p');
      if(nunary){    
         fprintf(file, "%s = %s", node->vreg.c_str(), node->lexinfo->c_str());
      }
      else if(unary){
         if(tree[left] == nullptr)
            fprintf(file, "%s = %s", node->vreg.c_str(), node->children[0]->vreg.c_str());
         else
            fprintf(file, "%s = %s", node->vreg.c_str(), tree[left]->vreg.c_str());

      }
      //case binary
      else if(!unary && !nunary){
         fprintf(file, "%s = %s %s %s", 
            node->vreg.c_str(), 
            node->children[0]->vreg.c_str(), 
            node->lexinfo->c_str(), 
            node->children[1]->vreg.c_str());

      }
   fprintf(file, "\n");
   }
}

void emit_rec(FILE* file, astree* node){
   for(auto child : node->children)
      emit_rec(file, child);
   emit_node(file, node);

}

void emit_node(FILE* file, astree* node){
   switch(node->symbol){
      case TOK_WHILE:
         emit_while(file, node);
         break;
      case TOK_IF:
         emit_if(file, node);
         break;
      case TOK_IFELSE:
         emit_ifelse(file, node);
         break;
      case TOK_VARDECL:
      case TOK_RETURN:
      break; 


      case '+':
      case '-':
      case '*':
      case '%':
      case TOK_EQ:
      case TOK_NE:
      case TOK_LE:
      case TOK_GE:
      case TOK_LT:
      case TOK_GT:
      case TOK_POS:
      case TOK_NEG:
      case TOK_INTCON:
      case TOK_IDENT:
      case TOK_CHARCON:
      emit_expr(file, node);
      break; 
   }
}

void swap_bool_char(astree* root){
   for(auto child : root->children)
      swap_bool_char(child);
   if(root->symbol == TOK_BOOL)
      root->symbol = TOK_CHAR;
}

void emit_stringcon(FILE* file, astree* node){
   fprintf(file, "char* %s = %s;\n", new_vreg('s').c_str(), node->lexinfo->c_str());
}

string mangle_struct(astree* node){
   //node is structdecl
   string builder;
   builder += "struct s_" + *node->children[0]->lexinfo + "{" + "\n";
   for(auto field = node->children.cbegin()+1; field != node->children.cend(); field++){
      builder += "        ";
      builder += *(*field)->lexinfo;
      builder += " _f";
      builder += *(*field)->children[0]->lexinfo;
      builder += "_";
      builder += *(*field)->children[0]->lexinfo;
      builder += ";\n";
   }
   builder += "};\n";
   return builder;
}

string mangle_label(astree* node){
   string builder;
   builder += *node->lexinfo + "_";
   builder += node->filenr + "_";
   builder += node->linenr + "_";
   builder += node->offset;
   return builder;
}

string mangle_vardecl(astree* node){
   string builder = mangle_ident(node);   
   return builder;
}

string mangle_ident(astree* node){
   string builder = *node->lexinfo + " ";
   astree* child = node->children[0];
   //global
   if (node->blocknr == 0){
      builder += "__";
      builder += *child->lexinfo;
   }
   //local
   else{
      builder += "_";
      builder += child->blocknr;
      builder +="_";
      builder += *child->lexinfo;
   }
   builder += ";\n";
   return builder;
}

string mangle_param(astree* node){
   string builder = *node->lexinfo + " ";
   astree* child = node->children[0];
   builder += "_";
   builder += to_string(child->blocknr);
   builder +="_";
   builder += *child->lexinfo;
   return builder;
}
void emit_func(FILE* file, astree* node){
   symstack.enter_block();
   string builder = "__" + *node->children[0]->lexinfo + " ";
   builder += *node->children[0]->children[0]->lexinfo + " (\n";
   for(auto param : node->children[1]->children){
      builder += "        " + mangle_param(param) + ",\n";
   }
   //replace last comma with paren
   builder.pop_back();
   builder.pop_back();
   builder += ")\n{\n";
   fprintf(file, builder.c_str());
   for(auto func_block : node->children[2]->children){
      emit_node(file, func_block);
   }
   fprintf(file, "}\n");
   symstack.leave_block();
}

void emit_oil(FILE* file, astree* root){
   //change all bool to char
   swap_bool_char(root);
   //first structs
   for(auto child : root->children){
      if(child->symbol == TOK_STRUCT){
         fprintf(file, mangle_struct(child).c_str());
      }
   }
   //ocmain
   fprintf(file, "void __ocmain (void) {\n");
   //next stringcon
   for(auto node : stringcon_list){
      fprintf(file, "        ");
      emit_stringcon(file, node);
   }
   //global vars
   for(auto child : root->children){
      if(child->symbol == TOK_VARDECL){
         fprintf(file, "        ");
         fprintf(file, mangle_vardecl(child->children[0]).c_str());
      }
   }
   fprintf(file, "}\n");
   //funcs
   for(auto child : root->children){
      if(child->symbol == TOK_FUNCTION){
         emit_func(file, child);
      }
   }
}

