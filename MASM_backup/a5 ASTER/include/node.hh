#ifndef _NODE_HH_
#define _NODE_HH_

#include <stdio.h>
#include <string>
#include <vector>
#include "dumpdot.hh"
#include "calldot.hh"
#include <cctype>
#include <cstdio>
#include <map>
using namespace std;
//using namespace llvm;

class  RegNode{
public:
    virtual int getvalue() = 0;
    virtual int getlcl() = 0;
    virtual int gettrend() = 0;
    virtual void setlcl( int i){
        return;
    }
    virtual void settrend( int t){
        return;
    }
    virtual void setvalue(int t){
        return;
    }
    virtual string* getname(){
        return new std::string ("error");
    }
};
class RegENode : public RegNode{
public:
    string* name;
    int lcl;   //last changed line 1-9998
    int value;  // Num for Reg or Mem or Num or Var , -9998-9998, 9999 for NULL
    int trend;  // 9999 for null, 0 for stay , +x for inc ,-x for dec
    RegENode() {
        lcl = 0 ; 
        value = 0; 
        trend = 0; 
    };
    int getlcl() { return lcl;}
    int gettrend() {return trend;}
    int getvalue() {return value;}
    void setlcl( int i ){
        lcl = i;
    }
    void settrend(int t){
        trend = t;
    }
    void setvalue(int t){
        value = t;
    }
    string* getname(){
        return name;
    }
};
class RegSNode : public RegNode{
public:
    string* name;
    RegENode *h,*l;
    RegSNode() {
        h = new RegENode(); 
        l = new RegENode(); 
    };
    int getvalue() { return h->value * 256 + l->value;}
    int getlcl()   { return h->lcl > l->lcl ? h->lcl : l->lcl ;}
    int gettrend() { return h->trend * 256 + l->trend;}
    void setlcl( int i ){
        h->lcl = i;
        l->lcl = i;
    }
    void settrend(int t){
        h->trend = t/256;
        l->trend = t%256;
    }
    void setvalue(int t){
        h->value = t/256;
        l->value = t%256;
    }
    string* getname(){
        return name;
    }
};
typedef enum {
    ALLSEG,
    CONTENT,
    ASSUME,
    INPUT,
    BLK,
    MACRO,
    BLKIT,
    NUM,
    VAR,
    CMP_INS,  //cmp
    JMP_INS,  //jmp,je,jle ,.., loop
    MOV_INS,  //mov,
    ADD_INS,
    SUB_INS,
    MUL_INS,
    DIV_INS,
    PUSH_INS,
    POP_INS,
    CALL_INS, //call
    INT_INS,
    RET_INS,
    OTHER
} NodeType;

typedef struct {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} Loc;

class Node {
public:
    NodeType type;
    Loc* loc;
    int lcl;   //last changed line 1-9998
    int value;  // Num for Reg or Mem or Num or Var , -9998-9998, 9999 for NULL
    int trend;  // 9999 for null, 0 for stay , +x for inc ,-x for dec
    Node() {loc = (Loc*)malloc(sizeof(Loc)); lcl = 1998 ; value = 9999; trend = 9999;}
    virtual ~Node() {delete loc;}
    void setLoc(Loc* loc);
    // virtual int dumpdot(DumpDOT *dumper) = 0;
};

class NumNode : public Node {
public:
    NumNode(int val)  { type = NUM; value = val;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};

class VarNode : public Node {
public:
    std::string *name;
    VarNode(std::string* name) : name(name) { type = VAR; };
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};


class CompUnitNode : public Node {
public:
     virtual int dumpdot(DumpDOT *dumper) = 0;
     virtual int calldot(CallDOT *caller) = 0;
};


class InstructionNode  : public Node{
public:
    virtual int dumpdot(DumpDOT *dumper) = 0;
    virtual int calldot(CallDOT *caller) = 0;
};

class IntInsNode : public InstructionNode{
public:
    int val;
    IntInsNode(int val): val(val) {type = INT_INS;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};

class DoubleONode :public Node{
public:
    RegNode *lhs,*rhs;
    DoubleONode(RegNode *lhs,RegNode *rhs) : lhs(lhs),rhs(rhs) {type = OTHER; };
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};

class MovInsNode : public InstructionNode{
public:
    DoubleONode *operand;
    MovInsNode(DoubleONode *operand): operand(operand) {type = MOV_INS;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};

class CmpInsNode : public InstructionNode{
public:
    DoubleONode *operand;
    CmpInsNode(DoubleONode *operand): operand(operand) {type = CMP_INS;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};
class JmpInsNode : public InstructionNode{
public:
    int cmp_trend;  
/*
0: jmp
1: jl
2: jle
3: je
4: jge
5: jg
6: jne
7: loop
*/
    std::string *label;
    JmpInsNode(int cmp_trend,string* label): cmp_trend(cmp_trend),label(label) {type = CMP_INS;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
    void diedcircle();
};
class AddInsNode : public InstructionNode{
public:
    DoubleONode *operand;
    AddInsNode(DoubleONode *operand): operand(operand) {type = ADD_INS;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};
class SubInsNode : public InstructionNode{
public:
    DoubleONode *operand;
    SubInsNode(DoubleONode *operand): operand(operand) {type = SUB_INS;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};

class MulInsNode : public InstructionNode{
public:
    RegNode *lh,*ll,*rh,*rl; // mul ah,al,al,reg8    or mul dx,ax,ax,reg16
    MulInsNode(RegNode *lh,RegNode*ll,RegNode*rh,RegNode*rl): lh(lh),ll(ll),rh(rh),rl(rl) {type = MUL_INS;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};
class DivInsNode : public InstructionNode{
public:
    RegNode *lh,*ll,*rh,*rl,*rf; // div ah,al,ah,al,reg8    
    DivInsNode(RegNode *lh,RegNode*ll,RegNode*rh,RegNode*rl,RegNode*rf): lh(lh),ll(ll),rh(rh),rl(rl),rf(rf) {type = DIV_INS;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};
class PushInsNode : public InstructionNode{
public:
    RegNode *operand;
    PushInsNode(RegNode *operand): operand(operand) {type = PUSH_INS;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};
class PopInsNode : public InstructionNode{
public:
    RegNode *operand;
    PopInsNode(RegNode *operand): operand(operand) {type = POP_INS;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};
class RetInsNode : public InstructionNode{
public:
    RetInsNode() {type = RET_INS;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};

class CallInsNode : public InstructionNode{
public:
    std::string * label;
    CallInsNode(string* label): label(label) {type = CALL_INS;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};

class BlockItemNode : public Node{
public:
    std::vector<InstructionNode*> lines;
    BlockItemNode() {type = BLKIT;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
    void append(InstructionNode *line); 
};
class AllocaLineNode : public Node{
public:
    string *label;
    int length; // 8,16,32
    AllocaLineNode(string *label,int length) : label(label),length(length) {type = ALLSEG;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};

class AllocaSegmentNode : public CompUnitNode{
public:
    std::vector<AllocaLineNode*> lines;
    AllocaSegmentNode() {type = ALLSEG;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
    void append(AllocaLineNode *line); 
};



class BlockNode : public InstructionNode{
public:
    std::string* label;
    BlockItemNode* block_root;
    int calldot_first;
    BlockNode(std::string* str,BlockItemNode * br) :label(str) ,block_root(br) {type = BLK;calldot_first = -1;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
    
};
class MacroNode : public Node{
public:
    std::string* name;
    BlockItemNode * blockitem;
    MacroNode(string *name,BlockItemNode* blockitem) : name(name),blockitem(blockitem)  {type = MACRO;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};

class AssumeNode : public CompUnitNode{
public:
    AssumeNode() {type = ASSUME;};
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};

class ContentNode : public CompUnitNode{
public:
    vector<MacroNode*> macros;
    vector<BlockNode*> block_root;
    ContentNode() {type = CONTENT;};
    void appendB(BlockNode *block);
    void appendM(MacroNode *macro);
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
};

class InputNode : public Node {
public:
    vector<CompUnitNode*> lines;
    InputNode() { type = INPUT; };
    int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
    void append(CompUnitNode *line);
};

class GlobalNode{
public:
    RegSNode *ax;
    RegSNode *bx;
    RegSNode *cx;
    RegSNode *dx;
    RegSNode *si;
    RegSNode *di;
    RegSNode *ds;
    RegSNode *ss;
    RegSNode *sp;
    RegSNode *mem;
    map<string,int>  MemTable; 
    map<string,BlockNode*>  LabelTable;
    vector<int>value_stack;
    int now_length;
    int now_cmp;
    int now_trend;
    int now_lcl;
    GlobalNode(){
        ax = new RegSNode();
        ax->name = new std::string ("ax");
        ax->h->name = new std::string ("ah");
        ax->l->name = new std::string ("al");
        bx = new RegSNode();
        bx->name = new std::string ("bx");
        bx->h->name = new std::string ("bh");
        bx->l->name = new std::string ("bl");
        cx = new RegSNode();
        cx->name = new std::string ("cx");
        cx->h->name = new std::string ("ch");
        cx->l->name = new std::string ("cl");
        dx = new RegSNode();
        dx->name = new std::string ("dx");
        dx->h->name = new std::string ("dh");
        dx->l->name = new std::string ("dl");
        si = new RegSNode();
        si->name = new std::string ("si");
        di = new RegSNode();
        di->name = new std::string ("di");
        ds = new RegSNode();
        ds->name = new std::string ("ds");
        ss = new RegSNode();
        ss->name = new std::string ("ss");
        sp = new RegSNode();
        sp->name = new std::string ("sp");
        mem = new RegSNode();
        mem->name = new std::string ("mem");
    }
    ~GlobalNode(){
      
    }
    int GetMemLength(string *var){
        if(MemTable.count(*var) == 1){
            return MemTable[*var];
        }
        else{
            return 0;
        }
    }
    int SetMemLength(string * var,int x){
        MemTable[*var] = x;
        return 0;
    }
    void push(int x)
    {
        value_stack.push_back(x);
    }
    int pop(){
        int x = value_stack.back();
        value_stack.pop_back();
        return x;
    }
// class BlockNode : public InstructionNode{
// public:
//     std::string* label;
//     BlockItemNode* block_root;
//     BlockNode(std::string* str,BlockItemNode * br) :label(str) ,block_root(br) {type = BLK;};
//     int dumpdot(DumpDOT *dumper);int calldot(CallDOT *caller);
    
// };
    int GetLabelLine(string *var){
        if(LabelTable.count(*var) == 1){
            return LabelTable[*var]->loc->first_line;
        }
        else{
            return 0;
        }
    }
    int SetLabelNode(string * var,BlockNode * block){
        LabelTable[*var] = block;
        return 0;
    }
    BlockNode* GetLabelNode(string * var){
        return LabelTable[*var];
    }
};

#endif
