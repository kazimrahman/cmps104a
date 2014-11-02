// $Id: main.cpp,v 1.1 2014-10-03 18:45:23-07 - - $

#include <string>
using namespace std;

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <vector>
#include "lyutils.h"
#include "stringset.h"
#include "auxlib.h"
#include "astree.h"

#define BUFSIZE 4096


/*Returns a pipe to a preprocessor which has been fed ocfname*/
FILE* preprocess(string ocfname, string options){
   //run c preprocessor
   string cmd = "cpp ";
   cmd += ocfname + " ";
   if (!options.empty())
      cmd += "-D" + options;

   //read in code and run preprocessor
   return(popen(cmd.c_str(), "r"));
   
}


void dump_tokens(astree* root, FILE* fp){
        unsigned filenr = root->filenr;
        fprintf(fp, "# %d \"%s\"\n", filenr, 
            scanner_filename(filenr)->c_str()); 
    for(auto iter=root->children.begin(); 
        iter!=root->children.end(); ++iter){
        if ((*iter)->filenr != filenr){
            filenr = (*iter)->filenr;
            fprintf(fp, "# %d \"%s\"\n", filenr, 
                scanner_filename(filenr)->c_str());
        }
        fprintf(fp, "%5zu %2zu.%03zu %4u  %-15s (%s)\n", 
            (*iter)->filenr,
            (*iter)->linenr,
            (*iter)->offset,
            (*iter)->symbol,
            get_yytname((*iter)->symbol),
            (*iter)->lexinfo->c_str());
    }
}

astree* fill_string_table(char* filename){
    //Reads yyin, fills stringtable and returns an AST root
    unsigned token_type;
    unsigned filenr = 0;
    astree * root = new_parseroot();
    while((token_type = yylex())){
        if (token_type == YYEOF)
            break;
        if (sscanf(yytext, "# %d \"%[^\"]\"", &scan_linenr, filename) == 2){
            filenr++;
        }
        astree* child = new_astree(
            token_type, included_filenames.size()-1, scan_linenr, scan_offset, yytext);
        adopt1(root, child);
    }
    return root;
}

char* append_extension(char* ocfname, string app_extension){
   char out_fname[BUFSIZE];
   strcpy(out_fname, basename(ocfname));
   auto dot_index = strrchr(out_fname, '.');
   *dot_index = '\0';
   strcat(out_fname, app_extension.c_str());
    return strdup(out_fname);
}

int main (int argc, char **argv) {
   int c;
   bool yflag = false;
   int lflag = 0;
   string dflag;
   string atflag;
    set_execname(argv[0]);

   while((c = getopt(argc-1, argv, "lyD:@:")) != -1)
      switch(c){
         case 'l':
                lflag = 1;
            break;
         case 'y':
            yflag = true;
            break;
         case 'D':
            dflag = optarg;
            break;
         case '@':
                set_debugflags(optarg);
            break;
         default:
            fprintf(stderr, "Unknown option `%s'\n", optarg);
      }
   yy_flex_debug = lflag;
   yydebug = yflag;
   char* ocfname = argv[argc-1];
   if (!strstr(ocfname, ".oc")){
      fprintf(stderr, "File %s must be an .oc file\n", ocfname);
        set_exitstatus(1);
   }
   FILE* tmp = fopen(ocfname, "r");
   if (tmp == NULL){
      fprintf(stderr, "File not found\n");
      exit(1);
   }
   fclose(tmp);

   //File parsing
   yyin = preprocess(ocfname, dflag);   
   astree* ast_root = fill_string_table(ocfname);
   fclose(yyin);
   char* str_fname = append_extension(ocfname, ".str");
   char* tok_fname = append_extension(ocfname, ".tok");

   //Dump str to file
   FILE* strfile = fopen(str_fname, "w");
   FILE* tokfile = fopen(tok_fname, "w");
   dump_stringset(strfile);
   dump_tokens(ast_root, tokfile);
   fclose(strfile);
   fclose(tokfile);
   return get_exitstatus();
}
