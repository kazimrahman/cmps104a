#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
#include <bitset>
#include <unordered_map>
using namespace std;

#include "auxlib.h"
struct symbol;
using symbol_table = unordered_map<string*,symbol*>;

enum { attr_void, attr_bool, attr_char, attr_int, attr_null,
attr_string, attr_struct, attr_array, attr_function,
attr_variable, attr_field, attr_typeid, attr_param, attr_lval,
attr_const, attr_vreg, attr_vaddr, attr_bitset_size,
};

using attr_bitset = bitset<attr_bitset_size>;

struct astree {
   int symbol;               // token code
   size_t filenr;            // index into filename stack
   size_t linenr;            // line number from source code
   size_t offset;            // offset of token with current line
   const string* lexinfo;    // pointer to lexical information
   vector<astree*> children; // children of this n-way node
   size_t blocknr;
   attr_bitset attr;
   symbol_table* struct_table;
   size_t deffilenr, deflinenr, defoffset;
   string vreg;
};

extern vector<astree*> stringcon_list;

astree* new_astree (int symbol, int filenr, int linenr,
                    int offset, const char* lexinfo);
astree* new_function (
      astree* identdecl, astree* paramlist, astree* block);
astree* new_proto (astree* identdecl, astree* paramlist);
astree* adopt1 (astree* root, astree* child);
astree* adopt2 (astree* root, astree* left, astree* right);
astree* adopt1sym (astree* root, astree* child, int symbol);
astree* adopt2sym (
      astree* root, astree* left, astree* right, int symbol);
astree* change_sym (astree* root, int symbol);
void dump_astree (FILE* astfile, astree* root);
void yyprint (FILE* outfile, unsigned short toknum,
              astree* yyvaluep);
void free_ast (astree* tree);
void free_ast2 (astree* tree1, astree* tree2);
string enum_bitset(attr_bitset a);

RCSH("$Id: astree.h,v 1.2 2013-10-11 18:52:46-07 - - $")
#endif
