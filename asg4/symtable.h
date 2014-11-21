#include <cstdlib>
#include <bitset>
#include <unordered_map>
#include <vector>
using namespace std;

enum { ATTR_void, ATTR_bool, ATTR_char, ATTR_int, ATTR_null,
ATTR_string, ATTR_struct, ATTR_array, ATTR_function,
ATTR_variable, ATTR_field, ATTR_typeid, ATTR_param, ATTR_lval,
ATTR_const, ATTR_vreg, ATTR_vaddr, ATTR_bitset_size,
};

using attr_bitset = bitset<ATTR_bitset_size>;

class sym;

using syment = pair<string*,sym*>;
using symtable = unordered_map<string*, sym*>;

class sym{
   public:
      attr_bitset attribute;
      symtable* fields;
      size_t filenr, linenr, offset;
      size_t blocknr;
      char* lexinfo;
      vector<sym*>* parameters; 
};

class symstack{
   uint_32 next_block = 1;
   vector <symtable> s;
   void insert_sym();
   syment find_sym();
   uint_32 new_block();

};
