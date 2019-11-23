# include <iostream>
# include <fstream>
#include <istream>
#include "lex.h"
#include "parsetreeSKEL.h"
#include "parse.h"

using namespace std;


int main(int argc, char *argv[])
{
    istream *in = &cin;
    ifstream inFile;
    int linenum = 0;
	
    for(int i=1; i<argc; i++){
        //string tesFlag = argv[i];
         if ( in != &cin )
            {
                cout << "TOO MANY FILENAMES" << endl;
                exit(0);
            }
        else
        {
            inFile.open(argv[i]);
            if( inFile.fail()) {
                cout << "COULD NOT OPEN " << argv[1] << endl;
                exit(0);
            }
            in = &inFile;
	    }

    }
    //cout << "begin" << endl;
    ParseTree *prog = Prog(*in, linenum);
   // cout << "end" << endl;
    if( prog == 0 )
    {
        return 0; 
    } 
       cout << "BANG COUNT: " << prog->BangCount() << endl;
       cout << "MAX DEPTH: " << prog->MaxDepth() << endl;
    

    return 0;
}