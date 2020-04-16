/*
Samantha Wagner
820643493
misc0266
Vanessa Chammas
820476794
misc0219
filename: Functions.C
*/


#include <map>
#include <vector>
#include <iostream>
#include <bitset>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "math.h"
#include "functions.h"
#include "byutr.h"
#define M 32


unsigned int frame = 0;

/* Function: Readalladdres
 *  It will read in all the addresses in the binary file and insert into the Pagetable.
 */


void readalladdres(const char *filename,const char *outputfilename, bool translate,PAGETABLE *pagetable) {
    FILE *fp = fopen(filename, "r");
    p2AddrTr trace_item;
    bool done = false;
    unsigned int num = 0;
    int numaddresses = 0;
    float hits = 0;
    float misses = 0;
    float hitpercent = 0;
    float misspercent = 0;
    if (pagetable->nflag) {
        while (numaddresses < pagetable->nnum) {  //set to n
            int bytesread = NextAddress(fp, &trace_item);
            if (!(done = bytesread == 0)) {
                if(PageLookup(pagetable,trace_item.addr) != NULL) {
                    hits++;
                }else {
                    misses++;
                }
                PageInsert(pagetable, trace_item.addr, frame);
                numaddresses++;
            }
        }
    } else {
        while (!done) {
            int bytesread = NextAddress(fp, &trace_item);
            if (!(done = bytesread == 0)) {
                if (PageLookup(pagetable, trace_item.addr) != NULL) {
                    hits++;
                } else {
                    misses++;
                }
                PageInsert(pagetable, trace_item.addr, frame);
                numaddresses++;
            }
        }
    }

    if(pagetable->pflag) {
        poutput(pagetable, outputfilename);
    }
    hitpercent = (float)(hits/numaddresses) * (float)100;
    misspercent = (float)(misses/numaddresses) * (float)100;

    cout<< "Hits: "<<hits<< "( "<<roundnum(hitpercent)<<"% )"<<" , Misses: "<< misses<< "( "<<roundnum(misspercent)<<"% )"<<" ,"<<"# of addresses: "<< numaddresses<<endl;

}

/* Function: roundnum
    It will round a float to 2 decimal places and round up.
 */

float roundnum(float var)
{
    float value = (int)(var * 100 + .5);
    return (float)value / 100;
}

/* Function: createlevel
 * It will create a level will the correct depth,pointer back to page table, and dynamically
 * allocate a level array.
 * It will return a pointer to the Nulllevel created.
 */


    level *createlevel(int depth, int Maxlevel, PAGETABLE *pagetable) {
        level *Nulllevel = new level();
        Nulllevel->levelnum = depth;
        Nulllevel->PTPtr = pagetable;
        Nulllevel->levelarray = new level *[pagetable->entrycount[depth]];
        if (depth != pagetable->Levelcount) {
            for (int i = 0; i < pagetable->entrycount[depth]; i++) {
                Nulllevel->levelarray[i] = nullptr;
            }
        }
        return Nulllevel;
    }

/* Function: LogicalToPage
 * It will take the Logical address and 'and' it will the correct bitmask.
 * It will return the page number.
 */

    unsigned int LogicalToPage(unsigned int LogicalAddress, unsigned int Mask, unsigned int Shift) {
        unsigned int final;
        final = LogicalAddress & Mask;
        final = final >> Shift;
        return final;
    }

/* Function: entrycountfunc
 * This will fill the entry count array with 2 to the page sizes.The page sizes will change at each depth.
 */

    void entrycountfunc(std::vector<int> pagesizes, int count, int *myarr) {
        for (int i = 0; i < count; i++) {
            myarr[i] = pow(2, pagesizes[i]);
        }
    }

/* Function: shiftaryfunc
 * This will fill the shift array with the page sizes being subtracted from 32.
 */

    void shiftaryfunc(std::vector<int> pagesizes, int count, int *myarr) {
        int total = 32;
        for (int i = 0; i < count; i++) {
            total = total - pagesizes[i];
            myarr[i] = total;
        }
    }

/* Function: bitmaskfunc
 * This will fill the bitmask array by setting a bitset of 32
 * and flipping bits to 1's for however many 'counts' there are
 *  counts were extra command line arguments
 */

    void bitmaskfunc(std::vector<int> pagesizes, int count, uint32_t *myarr) {
        int k = 0;
        for (int i = 0; i < count; i++) {
            std::bitset<M> mybits;
            for (int j = k; j < pagesizes[i] + k; j++) {
                mybits |= 1 << (31 - j); // same as .flip(j) //
            }
            k = k + pagesizes[i];
// storing the value in decimal.. not Hex could be problamtic signed -vanessa
            myarr[i] = mybits.to_ulong();
        }

    }

/* Function: createMap
 * This will create a new map with an invalid frame number and valid boolean.
 * It will return the map created.
 */

    MAP *createMap() {
        MAP *mymap = new MAP();
        mymap->framenum = -1;
        mymap->isvalid = -1;
        return mymap;
    }

/* Function: physicaladdress
 * This will take the frame number found,and the page size (2 ^ offset) and multiple them together to
 * get the physical address.
 * It will return the physical address.
 */

    unsigned int physicaladdress(int frame, int offset) {

        unsigned int PA = 0;
        int pagesize = pow(2, offset);
        PA = frame * pagesize;
        return PA;
    }

/* Function: PageInsert
 * It will insert the level 0 at the page table if not already there. Else, it will start the traversal of tree.
 */

    void PageInsert(PAGETABLE *PageTable, unsigned int LogicalAddress, unsigned int Frame) {
        if (PageTable->RootNodePtr == nullptr) {
            PageTable->RootNodePtr = createlevel(0, PageTable->Levelcount, PageTable);
        }
        PageInsert(PageTable->RootNodePtr, LogicalAddress, Frame);

    }

/* Function: PageInsert
 * This will travers through each level in the Pagetable and insert the correct pointers. Once it reaches
 * the final level, it will insert the frame number at a map. If needed (-t), it will translate
 * the addresses to physical addresses.
 */

    void PageInsert(level *levelptr, unsigned int LogicalAddress, unsigned int Frame) {
        std::bitset<M> oset = std::bitset<M>(LogicalAddress);
        int depth = levelptr->levelnum;
        unsigned int index = LogicalToPage(LogicalAddress, levelptr->PTPtr->bitmaskary[depth],
                                           levelptr->PTPtr->shiftary[depth]);
        if (levelptr->levelnum == levelptr->PTPtr->Levelcount) {
            if (levelptr->PTPtr->maparry[index].isvalid == -1) {
                levelptr->PTPtr->maparry[index].isvalid = 1;
                levelptr->PTPtr->maparry[index].framenum = Frame;
                frame++;
            }
            if (levelptr->PTPtr->tflag) {
                std::bitset<M> mybits;
                for (int i = 0; i < levelptr->PTPtr->offset; i++) {
                    if (oset[i] == 1) {
                        mybits |= 1 << i;
                    }
                }
                unsigned int PA = physicaladdress(levelptr->PTPtr->maparry[index].framenum, levelptr->PTPtr->offset);
                PA = PA + mybits.to_ulong();
                std::cout << std::setw(8) << std::setfill('0') << std::hex << LogicalAddress << " -> " << std::setw(8)
                          << std::setfill('0') << std::hex << PA << std::endl;
            }

        } else {
            if (levelptr->levelarray[index] == nullptr) {
                depth += 1;
                levelptr->levelarray[index] = createlevel(depth, levelptr->PTPtr->Levelcount, levelptr->PTPtr);
            }
            PageInsert(levelptr->levelarray[index], LogicalAddress, Frame);
        }

    }

/* Function: PageLookup
 * It will travers through the page table to try and see if there is a map that is associated with the
 * logical address. If there is, it will return the pointer to that map. If it is not found, it
 * will return null.
 */

    MAP *PageLookup(PAGETABLE *PageTable, unsigned int LogicalAddress) {
        unsigned int index = LogicalToPage(LogicalAddress, PageTable->bitmaskary[0], PageTable->shiftary[0]);
        int depth = 0;
        if (PageTable->RootNodePtr == nullptr) {
            return NULL;
        }
        level mylevel = *PageTable->RootNodePtr;
        while (mylevel.levelnum != PageTable->Levelcount) {
            if (mylevel.levelarray[index] == nullptr) {
                return NULL;
            } else {
                mylevel = *mylevel.levelarray[index];
                depth += 1;
                index = LogicalToPage(LogicalAddress, PageTable->bitmaskary[depth], PageTable->shiftary[depth]);
            }
        }
        if (mylevel.levelnum == PageTable->Levelcount) {
            if (mylevel.PTPtr->maparry[index].isvalid == 1) {
                return &mylevel.PTPtr->maparry[index];
            } else {
                return NULL;
            }
        }
        return NULL;
    }

    /* Function: Poutput
     * prints out the -p by printing the values within the map array in order of index
*/
    void poutput(PAGETABLE *pagetable, const char *outputfile) {
        std::ofstream outfile;
        outfile.open(outputfile);
        for (int i = 0; i < pagetable->entrycount[pagetable->Levelcount]; i++) {
            if (pagetable->maparry[i].isvalid != -1) {
                outfile << std::setw(8) << std::setfill('0')<<hex << i << " -> " <<std::setw(8) << std::setfill('0')<< pagetable->maparry[i].framenum << endl;
            }
        }

    }



