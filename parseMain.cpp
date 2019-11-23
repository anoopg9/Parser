# include <iostream>
# include <fstream>
#include 

int main(int argc, char *argv[])
{
	ifstream inFile;
	istream *in;
	int linenum = 0;

	if( argc == 1 ) {
		in = &cin;
	}

	else if( argc == 2 ) {
		inFile.open(argv[1]);
		if( inFile.is_open() == false ) {
			cout << "COULD NOT OPEN " << argv[1] << endl;
			return 1;
		}
		in = &inFile;
	}
	else {
		cout << "TOO MANY FILENAMES" << endl;
		return 1;
	}
    return 0;
}