/*
Samantha Wagner
820643493
misc0266
Vanessa Chammas
820476794
misc0219
filename: Functions.h
*/

#ifndef A04_FUNCTIONS_H
#define A04_FUNCTIONS_H


using namespace std;
class PAGETABLE;

class MAP{
public:
    int isvalid;
    int framenum;
};

class level{
public:
    PAGETABLE *PTPtr;
    int levelnum;
    level **levelarray;

};


class PAGETABLE{
public:
    MAP *maparry;
    int Levelcount;
    unsigned int *bitmaskary;
    int *shiftary;
    int *entrycount;
    level *RootNodePtr;
    bool tflag;
    bool nflag;
    bool pflag;
    int nnum;
    int offset;
};

typedef enum {
    BADFLAG
}RESULTTYPE;

void readalladdres(const char *filename,const char *outputfilename, bool translate,PAGETABLE *pagetable);
level * createlevel(int levelcount, int maxlevel, int size,PAGETABLE *pagetable);
MAP * PageLookup(PAGETABLE *PageTable, unsigned int LogicalAddress);
void PageInsert(PAGETABLE *PageTable, unsigned int LogicalAddress, unsigned int Frame);
void PageInsert(level *levelptr, unsigned int LogicalAddress, unsigned int Frame);
unsigned int physicaladdress(int frame, int offset);
MAP * createMap();
float roundnum(float var);
void poutput(PAGETABLE *pagetable, const char* outputfile);
void entrycountfunc(std::vector<int> pagesizes, int count, int *myarr);
void shiftaryfunc(std::vector<int> pagesizes, int count, int *myarr);
void bitmaskfunc(std::vector<int> pagesizes, int count, unsigned int *myarr);
unsigned int LogicalToPage(unsigned int LogicalAddress, unsigned int Mask, unsigned int Shift);


#endif //A04_FUNCTIONS_H
