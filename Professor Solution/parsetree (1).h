/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
using std::vector;
using std::map;

// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE };

// a "forward declaration" for a class to hold values
class Value;

class ParseTree {
	int			linenum;
	ParseTree	*left;
	ParseTree	*right;

public:
	ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
		: linenum(linenum), left(l), right(r) {}

	virtual ~ParseTree() {
		delete left;
		delete right;
	}

	int GetLineNumber() const { return linenum; }

	int Traverse(int input, int (ParseTree::*func)(void)) {
		if( left ) input += left->Traverse(input, func);
		if( right ) input += right->Traverse(input, func);
		return Visit(input, func);
	}

	virtual int Visit(int input, int (ParseTree::*func)(void)) {
		return input;
	}

	int MaxDepth() const {
        int lc = 0, rc = 0;
		if( left ) lc = left->MaxDepth();
		if( right ) rc += right->MaxDepth();
		if( lc > rc ) return lc + 1;
        return rc + 1;
	}

	virtual bool IsIdent() const { return false; }
	virtual bool IsVar() const { return false; }
	virtual string GetId() const { return ""; }
    virtual int IsBang() const { return 0; }
    virtual bool IsLet() const { return false; }

	int BangCount() const {
		int cnt = 0;
		if( left ) cnt += left->BangCount();
		if( right ) cnt += right->BangCount();
		cnt += IsBang();
		return cnt;
	}

	int CheckLetBeforeUse(map<string,bool>& var) {
        int cnt = 0;
		if( left ) cnt += left->CheckLetBeforeUse(var);
		if( right ) cnt += right->CheckLetBeforeUse(var);
		if( IsLet() )
			var[ this->GetId() ] = true;
        if( IsIdent() && var.find(GetId()) == var.end() ) {
            cout << "UNDECLARED VARIABLE " << GetId() << endl;
            cnt++;
        }
        return cnt;

	}

	//virtual Value Eval();
};

class StmtList : public ParseTree {

public:
	StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}

//	int Visit(int input, int (ParseTree::*func)(void)) {
//		for( auto s : statements ) {
//			input += s->Visit(input, func);
//		}
//		return input;
//	}
};

class Let : public ParseTree {
	string id;

public:
	Let(Lex& t, ParseTree *e) : ParseTree(t.GetLinenum(), e), id(t.GetLexeme()) {}

	string GetId() const { return id; }
	bool IsLet() const { return true; }
};

class Print : public ParseTree {
public:
	Print(int line, ParseTree *e) : ParseTree(line, e) {}
};

class Loop : public ParseTree {

public:
	Loop(int line, ParseTree *e, ParseTree *s) : ParseTree(line, e, s) {}
};

class If : public ParseTree {

public:
	If(int line, ParseTree *e, ParseTree *s) : ParseTree(line, e, s) {}
};


class PlusExpr : public ParseTree {
public:
	PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class MinusExpr : public ParseTree {
public:
	MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class TimesExpr : public ParseTree {
public:
	TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class DivideExpr : public ParseTree {
public:
	DivideExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class BangExpr : public ParseTree {
public:
	BangExpr(int line, ParseTree *l) : ParseTree(line,l) {}
    virtual int IsBang() const { return 1; }
};

class IConst : public ParseTree {
	int val;

public:
	IConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}
};

class SConst : public ParseTree {
	string val;

public:
	SConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = t.GetLexeme();
	}
};

class Ident : public ParseTree {
	string id;

public:
	Ident(Lex& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {}

	bool IsIdent() const { return true; }
	string GetId() const { return id; }
};

#endif /* PARSETREE_H_ */
