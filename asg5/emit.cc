#include "astree.h"
#include "lyutils.h"
#include "auxlib.h"
#include "emit.h"
#include <string>
#include <iostream>

size_t ptr_count = 0;
size_t char_count = 0;
size_t string_count = 0;
size_t int_count = 0;
size_t ind_ptr_count = 0;

void emit_expr_rec(FILE* file, astree* root){

}

void emit_func_rec(FILE* file, astree* root){

}

void emit_stringcon(FILE* file, astree* node){
   fprintf(file, "char* %s = %s;\n", new_vreg('s').c_str(), node->lexinfo->c_str());
}


string emit_decl(astree* node){
  //Node had better be a vardecl

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
   //builder += node->
   return builder;
}

string mangle_ident(astree* node){
   string builder = *node->lexinfo + " ";
   astree* child = node->children[0];
   string* clexinfo = const_cast<string*>(child->lexinfo);
   //conversion
   
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
void emit_oil(FILE* file, astree* root){
   //first structs
   for(auto child : root->children){
      if(child->symbol == TOK_STRUCT){
         fprintf(file, mangle_struct(child).c_str());
      }
   }
   //next stringcon
   for(auto node : stringcon_list){
      emit_stringcon(file, node);
   }
   //global vars
   for(auto child : root->children){
      if(child->symbol == TOK_VARDECL){
         fprintf(file, mangle_vardecl(child->children[0]).c_str());
      }
   }
}
