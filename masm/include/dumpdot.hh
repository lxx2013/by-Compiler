#ifndef _DUMPDOT_HH_
#define _DUMPDOT_HH_

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include "global.hh"
using namespace std;
class DumpDOT {
    FILE *fp;
    int count;
public:
    DumpDOT(FILE *fp) : fp(fp) {
        fprintf(fp, "digraph {\n");
        fprintf(fp, "    node [shape = record];\n");
        count = 0;
    }
    ~DumpDOT() { fprintf(fp, "}"); }
    int newNode(int num, ...);
    int newNode(std::vector<std::string> list);
    void drawLine(int nSrc, int fSrc, int nDst);
};


#endif
