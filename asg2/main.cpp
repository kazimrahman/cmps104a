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



void fill_string_table(FILE* pipe, char* filename){
	unsigned token_type;
	unsigned linenr = 1;
	unsigned charnr = 1;
	unsigned filenr = 2;
	astree* root = new_astree(0, filenr, linenr, charnr, "root");
	while((token_type = yylex())){
		if (token_type == YYEOF)
			return;
		astree* child = new_astree(token_type, filenr, linenr, charnr, yytext);
		adopt1(root, child);
		if (token_type == TOK_IDENT)
	      intern_stringset(strdup(yytext));
		if (*yytext == '\n'){
			linenr++;
			charnr = 1;
		}
		else {
			charnr+= sizeof(*yytext);
			DEBUGF('a', "%u\n", charnr);
		}
	}
	dump_astree(stdout, root);
}

char* append_extension(char* ocfname, string app_extension){
   char out_fname[BUFSIZE];
   strcpy(out_fname, basename(ocfname));
   auto dot_index = strrchr(out_fname, '.');
   *dot_index = '\0';
   strcat(out_fname, app_extension.c_str());
	return strdup(out_fname);
}

void parse(FILE* yyin){
	while(yylex() != YYEOF){
		printf(yytext);
	}
}

int main (int argc, char **argv) {
   int c;
	bool yflag = false;
	int lflag = (int)false;
   string dflag;
   string atflag;

   while((c = getopt(argc-1, argv, "lyD:@:")) != -1)
      switch(c){
         case 'l':
				lflag = true;
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
		int yy_flex_debug = lflag;
   char* ocfname = argv[argc-1];
   if (!strstr(ocfname, ".oc")){
      fprintf(stderr, "File %s must be an .oc file\n", ocfname);
      exit(1);
   }
   FILE* tmp = fopen(ocfname, "r");
   if (tmp == NULL){
      fprintf(stderr, "File not found\n");
      exit(1);
   }
   fclose(tmp);

   //File parsing
   yyin = preprocess(ocfname, dflag);   
   fill_string_table(yyin, ocfname);
	//parse(yyin);
   fclose(yyin);
	char* out_fname = append_extension(ocfname, ".str");

   //Dump str to file
   FILE* outfile = fopen(out_fname, "w");
   dump_stringset(outfile);
   fclose(outfile);
   return EXIT_SUCCESS;
}
