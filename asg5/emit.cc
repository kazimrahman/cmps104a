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
      indent(file);
      fprintf(file,
         "if (!%s) goto fi_%zu_%zu_%zu;\n", 
         node->children[0]->vreg.c_str(), 
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);
      for(auto child : node->children[0]->children){
         indent(file);
         emit_node(file, child);
      }
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
      indent(file);
      fprintf(file,
         "if (!%s) goto else_%zu_%zu_%zu;\n", 
         node->children[0]->vreg.c_str(), 
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);
      for(auto child : node->children[0]->children){
         indent(file);
         emit_node(file, child);
      }
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

void emit_expr(FILE* file, astree* node){
   indent(file);
   switch(node->symbol){
      case TOK_IDENT:
      case TOK_INTCON:
      case TOK_CHARCON:
         if(node->attr[attr_bool] || node->attr[attr_char])
            node->vreg = new_vreg('c');
         if(node->attr[attr_int])
            node->vreg = new_vreg('i');
         if(node->attr[attr_struct])
            node->vreg = new_vreg('p');
         fprintf(file, "%s = %s;\n", 
            node->vreg.c_str(), node->lexinfo->c_str());
         break;
      case '+':
      case '-':
      case '/':
      case '*':
      case '%':
      case TOK_EQ:
      case TOK_NE:
      case TOK_LT:
      case TOK_LE:
      case TOK_GT:
      case TOK_GE:
         if(node->attr[attr_bool] || node->attr[attr_char])
            node->vreg = new_vreg('c');
         if(node->attr[attr_int])
            node->vreg = new_vreg('i');
         if(node->attr[attr_struct])
            node->vreg = new_vreg('p');
         //for 90-c8q.oc
         if(node->children.empty() || 
               node->children[0] == nullptr || 
               node->children[1] == nullptr)
            break;
         fprintf(file, "%s = %s;\n", 
            node->children[0]->vreg.c_str(), 
            node->children[1]->vreg.c_str()); 
         break;
         default:
            errprintf("unkown expression: %s", node->lexinfo->c_str());
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
   fprintf(file, "char* %s = %s;\n", 
      new_vreg('s').c_str(), node->lexinfo->c_str());
}

string mangle_struct(astree* node){
   //node is structdecl
   string builder;
   builder += "struct s_" + *node->children[0]->lexinfo + "{" + "\n";
   for(auto field = node->children.cbegin()+1; 
         field != node->children.cend(); field++){
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
      emit_rec(file, func_block);
   }
   fprintf(file, "}\n");
   symstack.leave_block();
}

void emit_oil(FILE* file, astree* root){
   //change all bool to char
//   swap_bool_char(root);
   //first structs
   for(auto child : root->children){
      if(child->symbol == TOK_STRUCT){
         fprintf(file, mangle_struct(child).c_str());
      }
   }
   //ocmain
   fprintf(file, "void __ocmain (void) {\n");
   emit_rec(file, root);
   //next stringcon
   for(auto node : stringcon_list){
      indent(file);
      emit_stringcon(file, node);
   }
   //global vars
   for(auto child : root->children){
      if(child->symbol == TOK_VARDECL){
         fprintf(file, "        ");
         fprintf(file, mangle_ident(child->children[0]).c_str());
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

