/*
Samantha Wagner
820643493
misc0266
Vanessa Chammas
820476794
misc0219
filename: main.C
*/

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <stdexcept>
#include "math.h"
#include "byutr.h"
#include "functions.h"



int main(int argc, char **argv) {
//initialization of all variables and structures
    bool addrtranslate = false;
    bool filefound = false;
    const char *filename;
    const char *outputfile;
    std::vector<int> myvector;
    std::vector<int> extraval;
    int Option;
    int count = 0;
    int Pagesize = 0;

    PAGETABLE *mypagetable = new PAGETABLE();
    mypagetable->tflag=false;
    mypagetable->pflag=false;
    mypagetable->nflag=false;
    mypagetable->offset = 32;


//Parse through the command line arguments and set the correct flags
    while ((Option = getopt(argc, argv, "n:p:t")) != -1) {
        switch (Option) {
            case 'n':
                mypagetable->nnum = atoi(optarg);
                mypagetable->nflag=true;
                break;
            case 'p':
                mypagetable->pflag=true;
                outputfile = optarg;
                break;
            case 't':
                addrtranslate = true;
                mypagetable->tflag = true;
                break;
            default:
                std::cout << "print something about the usage and die…" << std::endl;
                exit(BADFLAG);
                break;
        }
    }
//will use these arguments to fill up the arrays in the pagetable
    for (; optind < argc; optind++) {
        if(isdigit(argv[optind][0])){
            extraval.push_back(atoi(argv[optind]));
            count++;
            mypagetable->offset = mypagetable->offset - atoi(argv[optind]);
        }else{
            filefound = true;
            filename = argv[optind];
        }
    }


//check to make sure the file is found and can be opened
    if (!filefound) {
        throw std::runtime_error("Error. No file specified");
    }

    std::ifstream myfile(filename);
    if (myfile.fail()) {
        throw std::runtime_error("Couldn't open the file");
    }

//fill the arrays in the pagetable by passing them into the correlating functions
    int entrycountarr[count];
    int shiftaryarr[count];
    uint32_t bitmaskarr[count];
    mypagetable->Levelcount = count-1;
    bitmaskfunc(extraval, count, bitmaskarr);
    mypagetable->bitmaskary = bitmaskarr;
    shiftaryfunc(extraval, count, shiftaryarr);
    mypagetable->shiftary = shiftaryarr;
    entrycountfunc(extraval, count, entrycountarr);
    mypagetable->entrycount = entrycountarr;
    mypagetable->maparry = new MAP[mypagetable->entrycount[mypagetable->Levelcount]];
    mypagetable->RootNodePtr = nullptr;
//fill the entries in the map array
    for (int i = 0; i < mypagetable->entrycount[mypagetable->Levelcount]; i++) {
        mypagetable->maparry[i] = *createMap();
    }

//parse through the addresses and add them into the pagetable
    Pagesize = pow(2,mypagetable->offset);
    cout<<"Pagesize: "<<Pagesize<<endl;
    readalladdres(filename,outputfile, addrtranslate,mypagetable);
}

