 /*
 * parse.cpp
 */

#include "parse.h"
#include "lex.h"
#include "parsetreeSKEL.h"

namespace Parser {
bool pushed_back = false;
Lex	pushed_token;

static Lex GetNextToken(istream& in, int& line) {
	if( pushed_back ) {
		pushed_back = false;
		return pushed_token;
	}
	return getNextToken(in, line);
}

static void PushBackToken(Lex& t) {
	if( pushed_back ) {
		abort();
	}
	pushed_back = true;
	pushed_token = t;
}

}


static int error_count = 0;

void
ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

ParseTree *Prog(istream& in, int& line)
{
	ParseTree *sl = Slist(in, line);

	if( sl == 0 )
		ParseError(line, "No statements in program");

	if( error_count )
		return 0;

	return sl;
}

// Slist is a Statement followed by a Statement List
ParseTree *Slist(istream& in, int& line) {
   
	ParseTree *s = Stmt(in, line);
   
	if( s == 0 )
		return 0;
    if(Parser::GetNextToken(in, line) != SC){
        ParseError(line, "Missing colon");
        return 0;
    }

	return new StmtList(s, Slist(in,line));
}

ParseTree *Stmt(istream& in, int& line) {
		ParseTree *s= NULL;
     
	Lex tok = Parser::GetNextToken(in, line);
   
	switch( tok.GetToken() ) {
	case IF:
		s = IfStmt(in, line);
		break;

	case PRINT:
		s = PrintStmt(in, line);
		break;
            
    case LET:
		s = LetStmt(in, line);
		break;
            
    case LOOP:
		s = LoopStmt(in, line);
		break;
    case ERR:
           
		ParseError(line, "Invalid statement");
		return 0;
    case END:
          Parser::PushBackToken(tok);  
            return 0;
	
        
	default:
            if (s==0 && tok !=DONE){
			    ParseError(line, "Invalid statement");
            }
            break;
	}
    
	return s;

}

ParseTree *LetStmt(istream& in, int& line) {
	Lex tok = Parser :: GetNextToken(in, line);
	if(tok != ID){
		ParseError(line, "Missing ID");
        Parser::PushBackToken(tok);
        return 0;
	}
	ParseTree *ex = Expr(in, line);
    if (ex == 0)
    {
        ParseError(line, "Missing expression after let");
        return 0;
    }
  // return 0;
   return new Let(tok, ex);

}

ParseTree *PrintStmt(istream& in, int& line) {
	
	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(line, "Missing expression after print");
		return 0;
	}
   // return 0;
	return new Print(line, ex);
}

ParseTree *LoopStmt(istream& in, int& line) {
	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(line, "Missing expression after Loop");
		return 0;
	}

	Lex tok = Parser::GetNextToken(in, line);

	if( tok != BEGIN ) {
		ParseError(line, "Missing BEGIN after expression");
		return 0;
	}
    
    //This will execute if Expr doesn't equal 0
	ParseTree *ex2 = Slist(in, line);
	if( ex2== 0 ) {
		ParseError(line, "Missing statement after begin");
		return 0;
	}
    tok = Parser::GetNextToken(in, line);
	if( tok != END ) {
		ParseError(line, "Missing END after expression");
		return 0;
	}

	return new Loop(line, ex, ex2);

}

ParseTree *IfStmt(istream& in, int& line) {
    
	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(line, "Missing expression after if");
		return 0;
	}

	Lex tok = Parser::GetNextToken(in, line);

	if( tok != BEGIN ) {
		ParseError(line, "Missing Begin after expression");
		return 0;
	}

	ParseTree *ex2 = Slist(in, line);
	if( ex2== 0 ) {
		ParseError(line, "Missing statement after BEGIN");
		return 0;
	}
	if( tok != END ) {
		ParseError(line, "Missing END after expression");
		return 0;
	}
	//return 0;

	return new If(line, ex, ex2);
}

ParseTree *Expr(istream& in, int& line) {
	ParseTree *t1 = Prod(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Lex tok = Parser::GetNextToken(in, line);

		if( tok != PLUS && tok != MINUS ) {
			Parser::PushBackToken(tok);
			return t1;
		}

		ParseTree *t2 = Prod(in, line);
		if( t2 == 0 ) {
			ParseError(line, "Missing expression after operator");
			return 0;
		}

		if( tok == PLUS )
			t1 = new PlusExpr(tok.GetLinenum(), t1, t2);
		else
			t1 = new MinusExpr(tok.GetLinenum(), t1, t2);
	}
}

ParseTree *Prod(istream& in, int& line) {
   ParseTree *pr = Rev(in, line);
	if( pr == 0 ) {
		return 0;
	}
    while ( true ) {
		Lex tok = Parser::GetNextToken(in, line);

		if( tok != STAR && tok != SLASH ) {
			Parser::PushBackToken(tok);
			return pr;
		}

		ParseTree *pr2 = Rev(in, line);
		if( pr2 == 0 ) {
			ParseError(line, "Missing expression after operator");
			return pr2;
		}

		if( tok == STAR )
			pr = new TimesExpr(tok.GetLinenum(), pr, pr2);
		else
			pr = new DivideExpr(tok.GetLinenum(), pr, pr2);
	}
}

ParseTree *Rev(istream& in, int& line) {
    Lex tok = Parser::GetNextToken(in, line);
    if(tok==BANG){
        ParseTree *pr = Rev(in, line);
        return new BangExpr(line, pr);
    }
    
    else{
        Parser::PushBackToken(tok);
        ParseTree *pr1 = Primary(in, line);
        return pr1;
    }
}

ParseTree *Primary(istream& in, int& line) {
	Lex t = Parser::GetNextToken(in, line);

	if( t == ID ) {
		return new Ident(t);
	}
	else if( t == INT ) {
		return new IConst(t);
	}
	else if( t == STR ) {
		return new SConst(t);
	}
	else if( t == LPAREN ) {
		ParseTree *ex = Expr(in, line);
		if( ex == 0 ) {
			ParseError(line, "Missing expression after (");
			return 0;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;

		ParseError(line, "Missing ) after expression");
		return 0;
	}

	ParseError(line, "Primary expected");
	return 0;
}

