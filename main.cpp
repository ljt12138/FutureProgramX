#include <bits/stdc++.h>
using namespace std;

const int MAX_ARRAY_SIZE = INT_MAX;

///
/// ::Log
///

#define LEXLOG "lexer.log"
#define PARSERLOG "parser.log"

///
/// ::Lib
///

///
/// ::Terminals
///

const int INCLUDE = INT_MAX;
const int IOSTREAM = INCLUDE - 1;
const int CSTDIO = IOSTREAM - 1;
const int USING = CSTDIO - 1;
const int NAMESPACE = USING - 1;
const int STD = NAMESPACE - 1;

const int INT = STD - 1;
const int IDENT = INT - 1;
const int NUMBER = IDENT - 1;
const int KEYWORD = NUMBER - 1;

const int CIN = KEYWORD - 1;
const int COUT = CIN - 1;
const int ENDL = COUT - 1;
const int PUTCHAR = ENDL - 1;

const int FOR = PUTCHAR - 1;
const int WHILE = FOR - 1;
const int IF = WHILE - 1;
const int ELSE = IF - 1;
const int RETURN = ELSE - 1;

const int LE = RETURN - 1;
const int GE = LE - 1;
const int EQ = GE - 1;
const int NEQ = EQ - 1;
const int LAND = NEQ - 1;
const int LOR = LAND - 1;

const int SHIFTL = LOR - 1; 
const int SHIFTR = SHIFTL - 1;  
const int ERROR = SHIFTR - 1;
const int EXIT = ERROR - 1;

const int INDEX = EXIT - 1;
const int EXEC = INDEX - 1;
const int COMMENT = EXEC - 1;

const int LP = '(';
const int RP = ')';
const int LC = '[';
const int RC = ']';
const int BEGIN = '{';
const int END = '}';

const int LNOT = '!';

const int MUL = '*';
const int DIV = '/';
const int MOD = '%';

const int PLUS = '+';
const int MINUS = '-';

const int LS = '<';
const int GT = '>';

const int XOR = '^';
const int ASSIGN = '=';
const int SEP = ';';

///
/// ::Location
///

class Location {
public:
	int line, col;

	Location()
	{ }

	Location(int line, int col)
	{ this->line = line, this->col = col; }

	friend ostream& operator << (ostream &oss, const Location &loc)
	{
		oss << "(" << loc.line << "," << loc.col << ")";
		return oss;
	}
};

///
/// ::Report Error
///

class CppError {

public:
	Location loc;
	virtual void print() const = 0;
};

class Report {
	bool error;

public:
	Report()
	{ error = false; }

	void issueError(const CppError &err)
	{
		err.print();
		error = true;
	}

	inline bool isError()
	{ return error; }
};

class Killer : public Report {

public:

	Killer()
	{ }

	void issueError(const CppError &err)
	{
		cerr << "!!!!!!!! Runtime Error !!!!!!!!!" << endl;
		cerr << "-- Interpreter has been killed--" << endl;
		err.print();
		cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		exit(1);
	}
} killer;

class NumberStartWithZero : public CppError {

public:
	NumberStartWithZero(const Location &loc)
	{ this->loc = loc; }

	virtual void print() const
	{
		cerr << "** Lexer Error ** : At " << loc << ", number except 0 can not start with 0" << endl; 
	}
};

class UnmatchableKeyword : public CppError {

public:
	UnmatchableKeyword(const Location &loc)
	{ this->loc = loc; }

	virtual void print() const
	{
		cerr << "** Lexer Error ** : At " << loc << ", there is no matchable keyword" << endl; 
	}
};

class SyntaxError : public CppError {

public:
	SyntaxError(const Location &loc)
	{ this->loc = loc; }

	virtual void print() const
	{
		cerr << "** Parser Error ** : At " << loc << ", Syntax error" << endl;
		exit(0);
	}
};

class SegmentError : public CppError {

public:
	SegmentError(const Location &loc)
	{ this->loc = loc; }

	virtual void print() const
	{
		cerr << "** Runtime Error ** : At " << loc << ", Segment Error" << endl;
	}
};

class BadArrayOp : public CppError {

public:
	BadArrayOp()
	{ }

	virtual void print() const
	{
		cerr << "Bad Array Operation" << endl;
	}
};

class VariableDefined : public CppError {

	string name;

public:

	VariableDefined(const Location &loc, const string &name)
	{
		this->loc = loc;
		this->name = name;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Variable \'" << name << "\' has been defined before. " << endl;
	}
};

class FunctionDefined : public CppError {

	string name;

public:

	FunctionDefined(const Location &loc, const string &name)
	{
		this->loc = loc;
		this->name = name;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Function \'" << name << "\' has been defined before. " << endl;
	}
};

class VariableOrFunctionNotDefined : public CppError {

	string name;

public:

	VariableOrFunctionNotDefined(const Location &loc, const string &name)
	{
		this->loc = loc;
		this->name = name;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Variable or Function \'" << name << "\' has been used before defined. " << endl;
	}
};

class VariableIsNotArray : public CppError {

	string name;

public:

	VariableIsNotArray(const Location &loc, const string &name)
	{
		this->loc = loc;
		this->name = name;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Variable \'" << name << "\' is NOT array." << endl;
	}
};

class ArrayInExpr : public CppError {

	string name;

public:

	ArrayInExpr(const Location &loc, const string &name)
	{
		this->loc = loc;
		this->name = name;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", \'" << name << "\' is a Array instead of Variable" << endl;
	}
};

class ArrayLengthError : public CppError {

	string name;

public:

	ArrayLengthError(const Location &loc, const string &name)
	{
		this->name = name;
		this->loc = loc;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", \'" << name << "\' length error. " << endl;
	}
};

///
/// ::Token : type, value  
///

class Token {

public:
	Location loc;
	int type;
	string ident;
	int number;

	Token()
	{ }

	Token(Location loc, int id)
	{ this->loc = loc, type = id; }

	Token(Location loc, int id, const string &str)
	{ this->loc = loc, type = id, ident = str; }

	Token(Location loc, int id, int num)
	{ this->loc = loc, type = id, number = num; }

	void print()
	{
		if (type == ERROR)
			cout << "error" << endl;
		else if (type == IDENT)
			cout << "ident " << ident << endl;
		else if (type == NUMBER)
			cout << "number " << number << endl;
		else cout << "keyword " << ident << endl;
	}
};

///
/// ::Lexer
///

class Lexer {
	map<int, string> table;
	map<string, int> anti;
	set<string> prefix;

	Report report;
	int IDENT;
	int NUMBER;
	int ERROR;

	Location loc;
	char ch;


	inline bool isblank(int c)
	{ return c == ' ' || c == '\n' || c == '\t' || c == '\r'; }

	inline void read()
	{
		ch = getchar();
		if (ch == '\n') loc.line++, loc.col = 0;
		else loc.col++;
	}

	inline void skip()
	{
		assert(isblank(ch));
		while (isblank(ch)) 
			read();
	}

	inline Token getIdent()
	{
		assert(isalpha(ch) || ch == '_');
		Location curloc = loc;
		string cur;
		while (isalnum(ch) || ch == '_') {
			cur += ch;
			read();
		}
		if (anti.count(cur)) return Token(curloc, anti[cur], cur);
		return Token(curloc, IDENT, cur);
	}

	inline Token getNumber()
	{
		assert(isdigit(ch));
		int number = 0;
		Location curloc = loc;
		if (ch == '0') {
			read();
			if (isdigit(ch)) {
				report.issueError(NumberStartWithZero(loc));
				while (isdigit(ch))
					read();
				// skip error
			}
		} else {
			while (isdigit(ch)) {
				number = number * 10 + ch - '0';
				read();
			}
		}
		return Token(curloc, NUMBER, number);
	}

	inline Token getKeyword()
	{
		Location curloc = loc;
		string cur;
		while (prefix.count(cur + ch)) {
			cur += ch;
			read();
		}
		if (cur == "") {
			report.issueError(UnmatchableKeyword(loc));
			while (!isblank(ch))
				read();
			return Token(curloc, ERROR, cur);
		}
		return Token(curloc, anti[cur], cur);
	}

public:	
	Lexer()
	{
		ch = ' ';
		loc.line = 1, loc.col = 0;
	}

	inline void append(int id, const string &str)
	{
		assert(!table.count(id));
		assert(!anti.count(str));
		table[id] = str;
		anti[str] = id;
		string now;
		for (size_t i = 0; i < str.size(); i++) {
			now = now + str[i];
			prefix.insert(now);
		}
	}

	inline void setIdent(int Id)
	{ IDENT = Id; }

	inline void setNumber(int Id)
	{ NUMBER = Id; }

	inline void setError(int Id)
	{ ERROR = Id; }

	inline void setSimple(const string &str)
	{
		for (size_t i = 0; i < str.size(); i++)
			append(str[i], str.substr(i, 1));
	}

	inline Token getN()
	{
		if (isblank(ch)) skip();
		if (ch == EOF) 
			return Token(loc, EXIT, "eof");
		if (ch == '_' || isalpha(ch)) return getIdent();
		else if (isdigit(ch)) return getNumber();
		else return getKeyword();
	}

	inline Token getNext()
	{
		Token tok = getN();
		while (tok.type == COMMENT) {
			while (ch != '\n')
				read();
			read();
			tok = getN();
		}
		return tok;
	}

	inline void start()
	{ skip(); }

	inline bool isError()
	{ return report.isError(); }
} lexer;

///
/// ::SemValue
///

class SemValue {
public:

	int retv;
	int pos;

	SemValue()
	{ retv = pos = 0; }

	SemValue(int retv, int pos)
	{ this->retv = retv, this->pos = pos; } 
};

/*

///
/// ::Variable
///

class Variable {

public:

	virtual bool isArray() const 
	{ return false; }

	virtual int getLength() const
	{ return -1; }

	virtual bool isInt() const
	{ return false; }

	virtual bool isChar() const
	{ return false; }

	virtual bool isVoid() const
	{ return false; }

	virtual int getElement(const vector<int> &index) const
	{ assert(false); }

	virtual void setElement(const vector<int> &index, int dat) 
	{ assert(false); }

	virtual void setSize(const vector<int> &index)
	{ assert(false); }

	virtual int getVal() const
	{ assert(false); }

	virtual void setVal(int dat)
	{ assert(false); }

};

class Char : public Variable {

	int dat;

public:

	virtual bool isChar() const
	{ return true; }

	virtual int getVal() const
	{ return dat; }

	virtual void setVal(int dat) 
	{ this->dat = dat; }

};

class Int : public Variable {

	int dat;

public:

	virtual bool isInt() const
	{ return true; }

	virtual int getVal() const
	{ return dat; }

	virtual void setVal(int dat) 
	{ this->dat = dat; }

};

class Void : public Variable {

public:

	virtual bool isVoid() const
	{ return true; }
};

const int MAX_ARRAY_SIZE = INT_MAX;

class Array : public Variable {

	vector<int> dat;
	bool error;
	vector<int> index;

	inline bool safe_mul(int a, int b)
	{ return (long long) a * b <= MAX_ARRAY_SIZE; }

	inline int checkIndex(const vector<int> &index)
	{
		int cur_size = 1;
		for (size_t i = 0; i < index.size(); i++) {
			if (index[i] < 0 || !safe_mul(cur_size, index[i])) 
				killer.issueError(BadArrayOp());
			cur_size = cur_size * index[i];
			// cout << index[i] << " ";
		}
		//cout << " __ " << endl;
		return cur_size;
	}

public:

	virtual bool isArray()
	{ return true; }

	virtual void setSize(const vector<int> &index)
	{
		assert(dat.size() == 0);
		this->index = index;
		int pos = checkIndex(index);
		for (int i = 0; i < pos; i++)
			dat.push_back(0);
	}

	virtual int getPos(const vector<int> &index)
	{
		if (this->index.size() != index.size())
			killer.issueError(BadArrayOp());
		int pre = 1, cnt = 0;
		for (int i = index.size(); i >= 0; i--) {
			if (this->index[i] <= index[i])
				killer.issueError(BadArrayOp());
			cnt = pre * (this->index[i] ? this->index[i] - 1 : 0) + cnt;
			pre = pre * this->index[i];
		}
		return cnt;
	}

	virtual int getElement(int pos)
	{
		if (pos < dat.size()) {
			return dat[pos];
		} else killer.issueError(BadArrayOp());
	}

	virtual void setElement(int pos, int dat)
	{
		if (pos < this->dat.size()) {
			this->dat[pos] = dat;
		} else killer.issueError(BadArrayOp());
	}
};
*/

class Tree;
class TopLevel;
class VarDef;
class FunDef;
class Block;
class BinaryOp;
class CinOp;
class CoutOp;
class Ident;
class If;
class While;
class For;
class Exec;
class ArrayAt;
class Return;
class Putchar;

///
/// ::Visitor
///

class Visitor {

public:

	virtual void visitTree(Tree *that)  
	{ assert(false); }

	virtual void visitTopLevel(TopLevel *that)  
	{}

	virtual void visitVarDef(VarDef *that)  
	{}

	virtual void visitFunDef(FunDef *that)  
	{}

	virtual void visitBlock(Block *that)  
	{}

	virtual void visitBinaryOp(BinaryOp *that)  
	{}

	virtual void visitCinOp(CinOp *that)  
	{}

	virtual void visitCoutOp(CoutOp *that)  
	{}

	virtual void visitIdent(Ident *that)  
	{}

	virtual void visitExec(Exec *that)  
	{}

	virtual void visitArrayAt(ArrayAt *that)  
	{}

	virtual void visitIf(If *that)  
	{}

	virtual void visitWhile(While *that)  
	{}

	virtual void visitFor(For *that)  
	{}

	virtual void visitReturn(Return *that)  
	{}

	virtual void visitPutchar(Putchar *that)
	{}
};

///
/// ::AST
///

class Tree {

	Location loc;

public:

	Tree()
	{ }

	Tree(const Location &loc)
	{ setLoc(loc); }

	inline virtual ~Tree()
	{ }

	void setLoc(const Location &loc)
	{ this->loc = loc; }

	Location getLoc()
	{ return this->loc; }

	inline virtual void accept(Visitor &v)
	{ v.visitTree(this); }

	inline virtual void printTo(int tab)
	{
		assert(false);
	}

};

class TopLevel : public Tree {

public:
	vector<Tree*> lst;

	TopLevel(const Location &loc, const vector<Tree*> &lst)
	{
		setLoc(loc);
		this->lst = lst;
	}

	inline virtual ~TopLevel()
	{
		for (size_t i = 0; i < lst.size(); i++)
			delete lst[i];
	} 

	inline virtual void accept(Visitor &v)
	{ v.visitTopLevel(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		puts("TopLevel");
		for (size_t i = 0; i < lst.size(); i++)
			lst[i]->printTo(tab+1);
	}
};

class Var {

	inline bool safe_mul(int a, int b)
	{ return (long long) a * b <= MAX_ARRAY_SIZE; }

	inline int checkIndex(const vector<int> &index)
	{
		int cur_size = 1;
		for (int i = 0; i < index.size(); i++) {
			if (index[i] < 0 || !safe_mul(cur_size, index[i])) 
				killer.issueError(BadArrayOp());
			cur_size = cur_size * index[i];
		}
		return cur_size;
	}

public:

	string type;
	string name;
	vector<int> index;
	int idScope;
	int idInScope;
	bool isLeftValue;
	int size;

	Var()
	{ }

	Var(const string &type, const string &name, const vector<int> &index)
	{
		this->type = type;
		this->name = name;
		this->index = index;
		idInScope = -1;
		idScope = 0;
		isLeftValue = true;
		size = checkIndex(index);
	}

	Var(const string &type, const string &name)
	{
		this->type = type;
		this->name = name;
		idInScope = -1;
		idScope = 0;
		isLeftValue = true;
		size = 1;
	}

	inline int getPos(const vector<int> &index)
	{
#ifdef safe
		if (this->index.size() != index.size())
			killer.issueError(BadArrayOp());
#endif
		int pre = 1, cnt = 0;
		for (int i = index.size()-1; i >= 0; i--) {
#ifdef safe 
			if (this->index[i] <= index[i])
				killer.issueError(BadArrayOp());
#endif
			cnt = pre * index[i] + cnt;
			pre = pre * this->index[i];
		}
		return cnt;
	}

	inline int getPos(int *index, int len)
	{
#ifdef safe
		if (this->index.size() != len)
			killer.issueError(BadArrayOp());
#endif
		int pre = 1, cnt = 0;
		for (int i = len-1; i >= 0; i--) {
#ifdef safe 
			if (this->index[i] <= index[i])
				killer.issueError(BadArrayOp());
#endif
			cnt = pre * index[i] + cnt;
			pre = pre * this->index[i];
		}
		return cnt;
	}



	void print()
	{
		cout << type << " " << name;
		if (type == "array") {
			for (int j = 0; j < index.size(); j++)
				cout << "[" << index[j] << "]";
		}
	}
};

class VarDef : public Tree {

public:
	vector<Var> vars;
	int totSiz;
	
	VarDef(const Location &loc, const vector<Var> &v)
	{
		setLoc(loc);
		vars = v;
	}

	inline virtual ~VarDef()
	{

	}

	inline virtual void accept(Visitor &v)
	{ v.visitVarDef(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("VarDef : ");
		assert(vars.size() > 0);
		for (int i = 0; i < vars.size(); i++) {
			vars[i].print();
			putchar((i == vars.size()-1) ? '\n' : ',');
		}
	}

};

class FunDef : public Tree {

public:
	string name;
	vector<Var> arg;
	Tree *stmt;
	bool isMain;
	size_t size;
	
	FunDef(const Location &loc, const string &name, const vector<Var> &arg, Tree *stmt)
	{
		setLoc(loc);
		this->name = name;
		this->arg = arg;
		this->stmt = stmt;
		isMain = (name == "main");
		size = arg.size();
	}

	inline virtual ~FunDef()
	{
		if (stmt != nullptr)
			delete stmt;
	}

	inline virtual void accept(Visitor &v)
	{ v.visitFunDef(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("FunDef : ");
		cout << name << " ";
		if (arg.size() == 0)
			printf("<empty>\n");
		for (int i = 0; i < arg.size(); i++) {
			arg[i].print();
			putchar((i == arg.size()-1) ? '\n' : ',');
		}
		if (stmt == nullptr)
			puts("<empty>");
		else stmt->printTo(tab+2);
	}
};

class Block : public Tree {

public:

	vector<Tree*> lst;

	Block(const Location &loc, const vector<Tree*> lst)
	{ setLoc(loc), this->lst = lst; }

	inline virtual void accept(Visitor &v)
	{ v.visitBlock(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("Block : \n");
		for (size_t i = 0; i < lst.size(); i++)
			lst[i]->printTo(tab+1);
	}
};

class Expr : public Tree {

	inline virtual void accept(Visitor &v) = 0;
	inline virtual void printTo(int tab) = 0;
};

class BinaryOp : public Expr {

public:

	string ident;
	int type;
	Tree *left, *right;	


	BinaryOp(const Location &loc, const int type, const string &ident, Tree *left, Tree *right)
	{
		setLoc(loc);
		this->type = type;
		this->ident = ident;
		this->left = left;
		this->right = right;
	}

	~BinaryOp()
	{
		if (left != nullptr)
			delete left;
		if (right != nullptr)
			delete right;
	}

	inline virtual void accept(Visitor &v)
	{ v.visitBinaryOp(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("Binary Op : ");
		cout << ident << endl;
		if (left != nullptr) left->printTo(tab+1);
		else puts("<empty>");
		if (right != nullptr) right->printTo(tab+1);
		else puts("<empty>");
	}
};

class CinOp : public Expr {

public:

	vector<Tree*> vars;

	CinOp(const Location &loc, const vector<Tree*> &vars)
	{
		setLoc(loc);
		this->vars = vars;
	}

	inline virtual ~CinOp()
	{ }

	inline virtual void accept(Visitor &v)
	{ v.visitCinOp(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("cin : \n");
		for (size_t i = 0; i < vars.size(); i++) {
			vars[i]->printTo(tab+1);
		}
	}
};

class CoutOp : public Expr {

public:

	vector<Tree*> expr;

	CoutOp(const Location &loc, const vector<Tree*> &T)
	{
		setLoc(loc);
		expr = T;
	}

	inline virtual ~CoutOp()
	{
		for (size_t i = 0; i < expr.size(); i++)
			delete expr[i];
	}

	inline virtual void accept(Visitor &v)
	{ v.visitCoutOp(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("cout : \n");
		for (size_t i = 0; i < expr.size(); i++)
			expr[i]->printTo(tab+1);
	}
};

class Ident : public Expr {

public:	

	int dat;
	int type;
	string name;	

	Var var;

	Ident(const Location &loc, int dat)
	{
		setLoc(loc);
		this->dat = dat;
		type = INT;
	}

	Ident(const Location &loc, int type, int dat)
	{
		setLoc(loc);
		this->dat = dat;
		this->type = type;
	}

	Ident(const Location &loc, const string &name)
	{
		setLoc(loc);
		this->type = KEYWORD;
		this->name = name;
	}

	inline virtual void accept(Visitor &v)
	{ v.visitIdent(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		if (type == INT) 
			printf("int_const : %d\n", dat);
		else if (type == KEYWORD)
			printf("var : %s\n", name.c_str());
		else printf("end of line\n");
	}
};

class ArrayAt : public Expr {

public:

	string name;
	vector<Tree*> at;

	int idInScope;
	int idScope;
	Var var;

	ArrayAt(const Location &loc, const string &name, const vector<Tree*> &index)
	{
		setLoc(loc);
		this->name = name;
		this->at = index;
	}

	inline virtual ~ArrayAt()
	{}

	inline virtual void accept(Visitor &v)
	{
		v.visitArrayAt(this);
	}

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("Array : ");
		cout << name << " size = " << at.size() << endl;
		for (size_t i = 0; i < at.size(); i++)
			at[i]->printTo(tab+1);
	}
};

class Exec : public Tree {

public:

	string name;
	vector<Tree*> args;

	FunDef *target;

	Exec(const Location &loc, const string &name, const vector<Tree*> &args)
	{
		setLoc(loc);
		this->name = name;
		this->args = args;
		target = nullptr;
	}

	inline virtual ~Exec()
	{
		for (size_t i = 0; i < args.size(); i++)
			delete args[i];
	}

	inline virtual void accept(Visitor &v)
	{ v.visitExec(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("exec : ");
		cout << name << endl;
		for (size_t i = 0; i < args.size(); i++)
			args[i]->printTo(tab+1);
	}
};

class If : public Tree {

public:

	Tree* expr;
	Tree* trueBranch;
	Tree* falseBranch;

	If(const Location &loc, Tree *expr, Tree *trueBranch)
	{
		setLoc(loc);
		this->expr = expr;
		this->trueBranch = trueBranch;
		this->falseBranch = nullptr;
	}

	If(const Location &loc, Tree *expr, Tree *trueBranch, Tree *falseBranch)
	{
		setLoc(loc);
		this->expr = expr;
		this->trueBranch = trueBranch;
		this->falseBranch = falseBranch;
	}

	inline virtual ~If()
	{
		if (expr != nullptr)
			delete expr;
		if (trueBranch != nullptr)
			delete trueBranch;
		if (falseBranch != nullptr)
			delete falseBranch;
	}

	inline virtual void accept(Visitor &v)
	{ v.visitIf(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("IF\n");
		expr->printTo(tab+1);
		if (trueBranch != nullptr)
			trueBranch->printTo(tab+1);
		else {
			for (int i = 0; i < tab; i++)
				putchar(' ');
			puts("<empty>");
		}
		if (falseBranch != nullptr)
			falseBranch->printTo(tab+1);
		else {
			for (int i = 0; i < tab; i++)
				putchar(' ');
			puts("<empty>");
		}
	}
};

class While : public Tree {

public:

	Tree* expr;
	Tree* stmt;

	While(const Location &loc, Tree *expr, Tree *stmt)
	{
		setLoc(loc);
		this->expr = expr;
		this->stmt = stmt;
	}

	inline virtual ~While()
	{
		if (expr != nullptr)
			delete expr;
		if (stmt != nullptr)
			delete stmt;
	}

	inline virtual void accept(Visitor &v)
	{ v.visitWhile(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("WHILE\n");
		expr->printTo(tab+1);
		if (stmt != nullptr)
			stmt->printTo(tab+1);
		else puts("<empty>");
	}
};

class For : public Tree {

public:

	Tree *init;
	Tree *expr;
	Tree *delta;
	Tree *stmt;


	For(const Location &loc, Tree *init, Tree *expr, Tree *delta, Tree *stmt)
	{
		setLoc(loc);
		this->init = init;
		this->expr = expr;
		this->delta = delta;
		this->stmt = stmt;
	}

	inline virtual ~For()
	{
		if (init != nullptr)
			delete init;
		if (expr != nullptr)
			delete expr;
		if (delta != nullptr)
			delete delta;
		if (stmt != nullptr)
			delete stmt;
	}

	inline virtual void accept(Visitor &v)
	{ v.visitFor(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("FOR\n");
		if (init != nullptr)
			init->printTo(tab+1);
		else puts("<empty>");
		if (expr != nullptr)
			expr->printTo(tab+1);
		else puts("<empty>");
		if (delta != nullptr)
			delta->printTo(tab+1);
		else puts("<empty>");
		if (stmt != nullptr)
			stmt->printTo(tab+1);
		else puts("<empty>");
	}
};

class Return : public Tree {
public:

	Tree *expr;

	Return(const Location &loc, Tree *expr)
	{
		setLoc(loc);
		this->expr = expr;
	}

	inline virtual ~Return()
	{
		if (expr != nullptr)
			delete expr;
	}

	inline virtual void accept(Visitor &v)
	{ v.visitReturn(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("RETURN\n");
		if (expr != nullptr)
			expr->printTo(tab+1);
	}
};

class Putchar : public Tree {
public:

	Tree *expr;

	Putchar(const Location &loc, Tree *expr)
	{
		setLoc(loc);
		this->expr = expr;
	}

	inline virtual ~Putchar()
	{
		delete expr;
	}

	inline virtual void accept(Visitor &v)
	{ v.visitPutchar(this); }

	inline virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("PUTCHAR\n");
		if (expr != nullptr)
			expr->printTo(tab+1);
	}

};

///
/// ::Scope
///

const int GLOBAL = -2;

template<typename T>
class Scope {

	map<string, pair<T, int> > table;
	int id;

public:

	Scope()
	{ id = 0; }

	inline bool count(const string &str) 
	{ return table.count(str); }

	inline pair<T, int> lookup(const string &str) 
	{
		assert(count(str));
		return table[str];
	}

	inline int create(const string &str, const T &var, int len = 1)
	{
		assert(!count(str));
		id += len;
		return table[str] = make_pair(var, id-len), id-len;
	}

	inline int size()
	{ return id; }
};

class ScopeStack {

	vector<Scope<Var> > stk;
	int id;
	int max_siz;

public:

	ScopeStack()
	{ id = 0, max_siz = 0; }

	inline void open()
	{ stk.push_back(Scope<Var>()); }

	inline int topSize()
	{ return stk[stk.size()-1].size(); }

	inline void close()
	{
		id = id - stk[stk.size()-1].size();
		stk.pop_back();
	}

	inline bool count(const string &str, bool through = true) 
	{
		if (!through)
			return stk[stk.size()-1].count(str);
		for (int i = stk.size()-1; i >= 0; i--)
			if (stk[i].count(str))
				return true;
		return false;
	}

	// pair<pair<Varibale, IdInScope>, NumberOfScope>
	inline pair<pair<Var, int>, int> lookup(const string &str) 
	{
		assert(count(str));
		for (int i = stk.size()-1; i >= 0; i--)
			if (stk[i].count(str))
				return make_pair(stk[i].lookup(str), i == 0 ? GLOBAL : stk.size()-1-i);
	}

	inline int create(const string &str, const Var &var, int len = 1)
	{
		id += len;
		max_siz = max(max_siz, id);
		return stk[stk.size()-1].create(str, var, len);
	}

	inline int getSize()
	{ return max_siz; }

} scopeStack;

///
/// ::SystemIO
///

class SystemIO {

public:
	virtual int read() = 0;
	virtual int put(int c) = 0;
	virtual int print(int d) = 0;
} ;

SystemIO *sysIO;

class Uoj98IO : public SystemIO {

	vector<int> vec;
	int pos;

public:
	Uoj98IO()
	{
		int n, d;
		pos = 0;
		scanf("%d", &n);
		for (int i = 1; i <= n; i++) {
			scanf("%d", &d);
			vec.push_back(d);
		}
	}

	virtual int read()
	{
		if (pos == vec.size()) return 0;
		return vec[pos++];
	}

	virtual int put(int c)
	{
		putchar(c);
		if (c == '\n')
			fflush(stdout);
		return c;
	}

	virtual int print(int d)
	{
		printf("%d", d);
		return d;
	}
} ;

///
/// ::TypeVisitor
///

/*
 * For Type Checking & Var Marking
 */

class TypeVisitor : public Visitor {

	ScopeStack scopeStack;
	Report report;
	Scope<FunDef*> functionScope;

	inline void newVar(const Location &loc, Var &cur)
	{
		if (scopeStack.count(cur.name, false)) {
			report.issueError(VariableDefined(loc, cur.name));
		} else {
			if (cur.type != "array") {
				cur.isLeftValue = true;
				cur.idInScope = scopeStack.create(cur.name, cur);
				cur.idScope = 0;
			} else {
				cur.isLeftValue = false;
				cur.idInScope = scopeStack.create(cur.name, cur, cur.size);
				cur.idScope = 0; 
			}
		}
	}

	inline void useVar(const Location &loc, Var &cur)
	{
		if (!scopeStack.count(cur.name)) {
			report.issueError(VariableOrFunctionNotDefined(loc, cur.name));
		} else {
			pair<pair<Var, int>, int> v = scopeStack.lookup(cur.name);
			if (v.first.first.type == "array") {
				report.issueError(ArrayInExpr(loc, cur.name));
			} else {
				cur.idScope = v.second;
				cur.idInScope = v.first.second;
				// cout << "At " << loc << ", var " << cur.name << " -> <" << cur.idScope << "," << cur.idInScope << ">" << endl;
			}
		}
	}

	inline void useArray(const Location &loc, const string &name, ArrayAt *arrayAt)
	{
		if (!scopeStack.count(name))
			report.issueError(VariableOrFunctionNotDefined(loc, name));
		else {
			pair<pair<Var,int>, int> v = scopeStack.lookup(name);
			if (v.first.first.type != "array")
				report.issueError(VariableIsNotArray(loc, name));
			else if (arrayAt->at.size() != v.first.first.index.size()) {
				report.issueError(ArrayLengthError(loc, name));
			} else {
				arrayAt->idScope = v.second;
				arrayAt->idInScope = v.first.second;
				arrayAt->var = v.first.first;
			} 
		}
	}

public:

	virtual void visitTree(Tree *that) 
	{ assert(false); }

	virtual void visitTopLevel(TopLevel *that) 
	{
		scopeStack.open();
		for (size_t i = 0; i < that->lst.size(); i++)
			that->lst[i]->accept(*this);
		scopeStack.close();
	}

	virtual void visitVarDef(VarDef *that) 
	{
		that->totSiz = 0;
		for (size_t i = 0; i < that->vars.size(); i++) {
			newVar(that->getLoc(), that->vars[i]);
			that->totSiz += that->vars[i].size;
		}
	}

	virtual void visitFunDef(FunDef *that) 
	{
		if (functionScope.count(that->name))
			report.issueError(FunctionDefined(that->getLoc(), that->name));
		else {
			functionScope.create(that->name, that);
			scopeStack.open();
			for (size_t i = 0; i < that->arg.size(); i++) 
				newVar(that->getLoc(), that->arg[i]);
			if (that->stmt != nullptr) 
				that->stmt->accept(*this);
			scopeStack.close();
		}
	}

	virtual void visitBlock(Block *that)
	{
		scopeStack.open();
		for (size_t i = 0; i < that->lst.size(); i++)
			that->lst[i]->accept(*this);
		scopeStack.close();
	}

	virtual void visitIf(If *that)
	{
		that->expr->accept(*this);
		if (that->trueBranch != nullptr)
			that->trueBranch->accept(*this);
		if (that->falseBranch != nullptr)
			that->falseBranch->accept(*this);
	}

	virtual void visitWhile(While *that)
	{
		that->expr->accept(*this);
		if (that->stmt != nullptr)
			that->stmt->accept(*this);
	}

	virtual void visitFor(For *that)
	{
		scopeStack.open();
		if (that->init != nullptr)
			that->init->accept(*this);
		that->expr->accept(*this);
		if (that->delta != nullptr)
			that->delta->accept(*this);
		if (that->stmt != nullptr)
			that->stmt->accept(*this);
		scopeStack.close();
	}

	virtual void visitReturn(Return *that)
	{
		that->expr->accept(*this);
	}

	virtual void visitBinaryOp(BinaryOp *that)
	{
		that->left->accept(*this);
		that->right->accept(*this);
	}

	virtual void visitCinOp(CinOp *that)
	{
		for (size_t i = 0; i < that->vars.size(); i++) {
			that->vars[i]->accept(*this);
		}
	}

	virtual void visitCoutOp(CoutOp *that)
	{
		for (size_t i = 0; i < that->expr.size(); i++)
			that->expr[i]->accept(*this);
	}

	virtual void visitIdent(Ident *that)
	{
		if (that->type == KEYWORD) {
			that->var = Var("int", that->name);
			useVar(that->getLoc(), that->var);
		}
	}

	virtual void visitExec(Exec *that)
	{
		if (!functionScope.count(that->name))
			report.issueError(VariableOrFunctionNotDefined(that->getLoc(), that->name));
		else that->target = functionScope.lookup(that->name).first;
		for (size_t i = 0; i < that->args.size(); i++)
			that->args[i]->accept(*this);
	}

	virtual void visitArrayAt(ArrayAt *that)
	{
		useArray(that->getLoc(), that->name, that);
		for (size_t i = 0; i < that->at.size(); i++)
			that->at[i]->accept(*this);
	}

	virtual void visitPutchar(Putchar *that)
	{
		that->expr->accept(*this);
	}

	inline bool isError()
	{ return report.isError(); }

	inline int getSize()
	{ return scopeStack.getSize(); }
} typeVisitor;

///
/// ::RunVisitor
///

class RunVisitor : public Visitor {

	vector<int> pool;
	int top;

	struct state {

		int retv;
		bool finished;

		state()
		{ finished = false, retv = 0; }

	};

	vector<state> funStack;
	vector<int> index;

	SemValue value;

	int retv;

	inline state& up()
	{ return funStack[funStack.size()-1]; }

	inline int boolean(int dat)
	{ return dat != 0; }

	inline void openStack()
	{
		// cout << " OPEN " << pool.size() << endl;
		index.push_back(top);
	}

	inline void closeStack()
	{
		top = index[index.size()-1];
		index.pop_back();
		// cout << " CLOSE TO " << cnt << endl;
	}

	inline int getPosition(int idScope, int idInScope)
	{
		if (idScope == GLOBAL)
			return idInScope;
		return index[index.size()-1-idScope] + idInScope;
	}

	inline void newFun()
	{
		funStack.push_back(state());
	}

	inline void endFun()
	{
		funStack.pop_back();
	}

	void execFun(FunDef *fun, int *args, size_t len)
	{
		openStack();
		newFun();
		top += fun->size;
		pool.resize(top);
		size_t cnt = min(fun->size, len);
		for (size_t i = 0; i < cnt; i++) {
			pool[getPosition(fun->arg[i].idScope, fun->arg[i].idInScope)] = args[i];
		}
		fun->stmt->accept(*this);
		value = SemValue(up().retv, -1);
		endFun();
		closeStack();
	}

#define checkFun if (!funStack.empty() && up().finished) return

public:

	virtual void visitTopLevel(TopLevel *that)
	{
		openStack();
		size_t cnt = that->lst.size();
		for (size_t i = 0; i < cnt; i++)
			that->lst[i]->accept(*this);
		closeStack();
	}

	virtual void visitVarDef(VarDef *that)
	{
		checkFun;
		top += that->totSiz; 
		pool.resize(top);
	}

	virtual void visitFunDef(FunDef *that)
	{
		if (that->isMain) {
			int *a;
			execFun(that, a, 0);
		}
	}

	virtual void visitBlock(Block *that)
	{
		checkFun;
		openStack();
		size_t cnt = that->lst.size();
		for (size_t i = 0; i < cnt; i++)
			that->lst[i]->accept(*this);
		closeStack();
	}

	virtual void visitBinaryOp(BinaryOp *that)
	{
		checkFun;
		SemValue left, right;
		that->left->accept(*this);
		left = value;
		that->right->accept(*this);
		right = value;

		switch(that->type) {
		case '+':
			value = SemValue(left.retv + right.retv, -1);
			break;

		case '-':
			// cout << left.retv << " " << left.pos << "  --  " << right.retv << endl;
			value = SemValue(left.retv - right.retv, -1);
			break;

		case '*':
			value = SemValue(left.retv * right.retv, -1);
			break;

		case '/':
			value = SemValue(left.retv / right.retv, -1);
			break;

		case '<':
			value = SemValue(left.retv < right.retv, -1);
			break;

		case '>':
			value = SemValue(left.retv > right.retv, -1);
			break;

		case LE:
			value = SemValue(left.retv <= right.retv, -1);
			break;

		case GE:
			value = SemValue(left.retv >= right.retv, -1);
			break;

		case NEQ:
			value = SemValue(left.retv != right.retv, -1);
			break;

		case EQ:
			value = SemValue(left.retv == right.retv, -1);
			break;

		case LAND:
			value = SemValue(left.retv && right.retv, -1);
			break;

		case LOR:
			value = SemValue(left.retv || right.retv, -1);
			break;

		case '=':
			value = SemValue(right.retv, left.pos);
#ifdef safe
			if (left.pos < 0 || left.pos >= top)
				killer.issueError(SegmentError(that->getLoc()));
#endif
			pool[left.pos] = right.retv;
			break;

		case '^':
			value = SemValue(boolean(left.retv) ^ boolean(right.retv), -1);
			break;

		case '%':
			value = SemValue(left.retv % right.retv, -1);
			break;

		case '!':
			value = SemValue(!right.retv, -1);
			break;

		default:
			assert(false);
		}
	}

	virtual void visitCinOp(CinOp *that)  
	{
		checkFun;
		size_t cnt = that->vars.size();
		for (size_t i = 0; i < cnt; i++) {
			that->vars[i]->accept(*this);
#ifdef safe
			if (value.pos < 0 || value.pos >= top)
				killer.issueError(SegmentError(that->getLoc()));
#endif
			pool[value.pos] = sysIO->read();
			// cout << "SET " << value.pos << " " << pool[value.pos] << endl;
		}
		value = SemValue(0, -1);
	}

	virtual void visitCoutOp(CoutOp *that)  
	{
		// cout << "COUT OP " << that->expr.size() << endl;
		checkFun;
		size_t cnt = that->expr.size();
		for (size_t i = 0; i < cnt; i++) {
			that->expr[i]->accept(*this);
			if (value.pos == -2)
				sysIO->put('\n');
			else 
				sysIO->print(value.retv);
		}
		value = SemValue(0, -1);
		// cout << "FIN" << endl;
	}

	virtual void visitIdent(Ident *that)  
	{
		checkFun;
		int pos;
		switch(that->type) {
		case INT:
			value = SemValue(that->dat, -1);
			break;
		case KEYWORD:
			pos = getPosition(that->var.idScope, that->var.idInScope);
#ifdef safe
			if (value.pos < 0 || value.pos >= top)
				killer.issueError(SegmentError(that->getLoc()));
#endif
			value = SemValue(pool[pos], pos);
			break;
		default:
			value = SemValue(0, -2);
			break;
		}
	}

	virtual void visitExec(Exec *that)  
	{
		checkFun;
	        size_t cnt = that->args.size();
	        int args[cnt];
		for(size_t i = 0; i < cnt; i++) {
			that->args[i]->accept(*this);
			args[i] = value.retv;
		}
		execFun(that->target, args, cnt);
	}

	virtual void visitArrayAt(ArrayAt *that)  
	{
		checkFun;
	        size_t cnt = that->at.size();
		int index[cnt];
		for (size_t i = 0; i < cnt; i++) {
			that->at[i]->accept(*this);
			index[i] = value.retv;
		}
		int pos = getPosition(that->idScope, that->idInScope) + that->var.getPos(index, cnt);

#ifdef safe
		if (pos < 0 || pos >= top)
			killer.issueError(SegmentError(that->getLoc()));
#endif

		value = SemValue(pool[pos], pos);
	}

	virtual void visitIf(If *that)  
	{
		checkFun;
		that->expr->accept(*this);
		if (value.retv) {
			if (that->trueBranch != nullptr)
				that->trueBranch->accept(*this);
		} else {
			if (that->falseBranch != nullptr)
				that->falseBranch->accept(*this);
		}
	}

	virtual void visitWhile(While *that)  
	{
		checkFun;
		while (that->expr->accept(*this), value.retv) 
			that->stmt->accept(*this);
	}

	virtual void visitFor(For *that)  
	{
		checkFun;
		openStack();
		if (that->init != nullptr) that->init->accept(*this);
		for (; that->expr != nullptr && (that->expr->accept(*this), value.retv); that->delta->accept(*this)) 
			that->stmt->accept(*this);
		closeStack();
	}

	virtual void visitReturn(Return *that)  
	{
		checkFun;
		that->expr->accept(*this);
		up().finished = true;
		up().retv = value.retv;
	}

	virtual void visitPutchar(Putchar *that)
	{
		checkFun;
		that->expr->accept(*this);
		sysIO->put(value.retv);
	}


} runVisitor;


///
/// ::Parser
///

class Parser {
	vector<Token> token;
	Report report;
	int pos;
	Tree *prog;
	SystemIO *systemIO;

	inline Token lookahead()
	{ return token[pos]; }

	inline void match(int id)
	{
		if (pos == -1) return;
		else if (pos >= token.size()) {
			report.issueError(SyntaxError(token[token.size()-1].loc));
			pos = -1;
		} else if (token[pos].type != id) {
			report.issueError(SyntaxError(token[pos].loc));
		} else {
			pos++;
		}
	}

	inline void match(int id, const CppError &error)
	{
		if (pos == -1) return;
		else if (pos >= token.size()) {
			report.issueError(error);
			pos = -1;
		} else if (token[pos].type != id) {
			report.issueError(error);
		} else {
			pos++;
		}
	}

public:

#define FIRST_EXPR case IDENT:			\
	case NUMBER:				\
	case '(':case '+':case '-':case '!':	\
	case CIN:case COUT:

	Tree* program()
	{
		match('#'), match(INCLUDE), match('<'), match(IOSTREAM), match('>');
		match('#'), match(INCLUDE), match('<'), match(CSTDIO), match('>');
		match(USING), match(NAMESPACE), match(STD),  match(';');
		return top_level();
	}

	Tree* top_level()
	{
		Tree *tmp = nullptr;
		vector<Tree*> arg;
		Location loc = lookahead().loc;
		while (1) {
			switch(lookahead().type) {
			case EXIT:
				match(EXIT);
				return new TopLevel(loc, arg);
			case INT:
				match(INT);
				break;
			default:
				killer.issueError(SyntaxError(lookahead().loc));
			}
			arg.push_back(var_and_func());
		}
	}

	Tree* var_and_func()
	{
		string name = lookahead().ident;
		Location loc = lookahead().loc;
		vector<Var> opt;
		Tree *T;
		match(IDENT);
		switch(lookahead().type) {
		case '(':
			T = func_def(opt);
			return new FunDef(loc, name, opt, T);
		case ',':
		case ';':
		case '[':
			var_def(name, opt);
			vars_def(opt);
			match(';');
			return new VarDef(loc, opt);
		default:
			killer.issueError(SyntaxError(lookahead().loc));
		}
		assert(false);
	}

	void opt_params(vector<Var> &opt)
	{
		string name;
		switch(lookahead().type) {
		case INT:
			match(INT);
			name = lookahead().ident;
			match(IDENT);
			opt.push_back(Var("int", name));
			if (lookahead().type == ',') {
				match(',');
				params(opt);
			}
			break;
		}
	}

	void params(vector<Var> &opt)
	{
		string name;
		switch(lookahead().type) {
		case INT:
			match(INT);
			name = lookahead().ident;
			match(IDENT);
			opt.push_back(Var("int", name));
			if (lookahead().type == ',') {
				match(',');
				params(opt);
			}
			break;
		}
	}

	Tree* func_def(vector<Var> &opt)
	{
		match('(');
		opt_params(opt);
		match(')');
		Tree *T = block();
		return T;
	}

	Tree* block()
	{
		Location loc = lookahead().loc;
		match('{');
		vector<Tree*> T;
		stmts(T);
		match('}');
		return new Block(loc, T);
	}

	// TODO HERE

	Tree* stmt()
	{
		Location loc = lookahead().loc;
		vector<Var> vars;
		vector<Tree*> T;
		Tree *tree;
		switch(lookahead().type) {
		case PUTCHAR:
			match(PUTCHAR);
			match('(');
			tree = expr();
			match(')');
			match(';');
			return new Putchar(loc, tree);
		case INT:
			match(INT);
			var_def(vars);
			vars_def(vars);
			match(';');
			return new VarDef(loc, vars);
		case '{':
			match('{');
			stmts(T);
			match('}');
			return new Block(loc, T);
		case IF:
			return ifstmt();
		case FOR:
			return forstmt();
		case WHILE:
			return whilestmt();
		case RETURN:
			return returnstmt();
		case ';':
			match(';');
			return nullptr;
		default:
			tree = expr();
			match(';');
			return tree;
		}
	}

	Tree* ifstmt()
	{
		assert(lookahead().type == IF);
		Location loc = lookahead().loc;
		match(IF);
		match('(');
		Tree *e = expr();
		match(')');
		Tree *tb = stmt();
		if (lookahead().type == ELSE) {
			match(ELSE);
			return new If(loc, e, tb, stmt());
		}
		return new If(loc, e, tb);
	}

	Tree* forstmt()
	{
		assert(lookahead().type == FOR);
		Location loc = lookahead().loc;
		match(FOR);
		match('(');
		Tree *init = stmt_in_for();
		match(';');
		Tree *check = opt_expr();
		match(';');
		Tree *delta = opt_expr();
		match(')');
		return new For(loc, init, check, delta, stmt());
	}

	Tree* stmt_in_for()
	{
		Tree *T;
		vector<Var> vars;
		Location loc = lookahead().loc;
		switch(lookahead().type) {
		case INT:
			match(INT);
			var_def(vars);
			vars_def(vars);
			return new VarDef(loc, vars);
			FIRST_EXPR
				return expr();
		}
		return nullptr;
	}

	Tree* opt_expr()
	{
		Location loc = lookahead().loc;
		switch(lookahead().type) {
			FIRST_EXPR
				return expr();
		default:
			return new Ident(loc, 1);
		}
	}

	Tree* whilestmt()
	{
		assert(lookahead().type == WHILE);
		Location loc = lookahead().loc;
		match(WHILE);
		match('(');
		Tree *check = expr();
		match(')');
		return new While(loc, check, stmt());
	}

	Tree* returnstmt()
	{
		assert(lookahead().type == RETURN);
		Location loc = lookahead().loc;
		match(RETURN);
		Tree *e = expr();
		match(';');
		return new Return(loc, e);
	}


	void stmts(vector<Tree*> &vec)
	{
		switch(lookahead().type) {
			FIRST_EXPR
		case '{':
		case INT:
		case IF:
		case FOR:
		case WHILE:
		case RETURN:
		case PUTCHAR:
			vec.push_back(stmt());
			stmts(vec);
			break;
		}
	}

	void var_def(const string &name, vector<Var> &opt)
	{
		vector<int> index;
		switch(lookahead().type) {
		case '[':
			array_index(index);
			opt.push_back(Var("array", name, index));
			break;
		default:
			opt.push_back(Var("int", name));
			break;
		}
	}

	void array_index(vector<int> &index)
	{
		int number;
		switch(lookahead().type) {
		case '[':
			match('[');
			number = lookahead().number;
			match(NUMBER);
			index.push_back(number);
			match(']');
			array_index(index);
		}
	}

	void array_index_expr(vector<Tree*> &index)
	{
		switch(lookahead().type) {
		case '[':
			match('[');
			index.push_back(expr());
			match(']');
			array_index_expr(index);
		}
	}

	void var_def(vector<Var> &opt)
	{
		string name;
		switch(lookahead().type) {
		case IDENT:
			name = lookahead().ident;
			match(IDENT);
			var_def(name, opt);
			break;
		}
	}

	void vars_def(vector<Var> &opt)
	{
		switch(lookahead().type) {
		case ',':
			match(',');
			var_def(opt);
			vars_def(opt);
			break;
		}
	}

	Tree* expr()
	{
		Location loc = lookahead().loc;
		vector<Tree*> exps;
		switch(lookahead().type) {
		case CIN:
			match(CIN);
			match(SHIFTR);
			cin_clause(exps);
			return new CinOp(loc, exps);
		case COUT:
			match(COUT);
			match(SHIFTL);
			cout_clause(exps);
			return new CoutOp(loc, exps);
		default:
			return unit9();
		}
	}

	void cin_clause(vector<Tree*> &vars)
	{
		vars.push_back(left_value());
		switch(lookahead().type) {
		case SHIFTR:
			match(SHIFTR);
			cin_clause(vars);
		}
	}

	void cout_clause(vector<Tree*> &exps)
	{
		exps.push_back(expr());
		switch(lookahead().type) {
		case SHIFTL:
			match(SHIFTL);
			cout_clause(exps);
			break;
		}
	}

	Tree* unit0()
	{
		Location loc = lookahead().loc;
		int dat;
		Tree *T;
		switch(lookahead().type) {
		case NUMBER:
			dat = lookahead().number;
			match(NUMBER);
			return new Ident(loc, dat);
		case '(':
			match('(');
			T = expr();
			match(')');
			return T;
		case IDENT:
			return unit0_ident();
		case ENDL:
			match(ENDL);
			return new Ident(loc, ENDL, '\n');
		default:
			report.issueError(SyntaxError(loc));
		}
	}

	Tree* unit0_ident()
	{
		assert(lookahead().type == IDENT);
		Location loc = lookahead().loc;
		string name = lookahead().ident;
		match(IDENT);
		vector<Tree*> index;
		Tree *T;
		switch(lookahead().type) {
		case '[':
			array_index_expr(index);
			return new ArrayAt(loc, name, index);
		case '(':
			match('(');
			product(index);
			match(')');
			return new Exec(loc, name, index);
		default:
			return new Ident(loc, name);
		}
	}

	Tree* left_value()
	{
		assert(lookahead().type == IDENT);
		Location loc = lookahead().loc;
		string name = lookahead().ident;
		match(IDENT);
		vector<Tree*> index;
		Tree *T;
		switch(lookahead().type) {
		case '[':
			array_index_expr(index);
			return new ArrayAt(loc, name, index);
		default:
			return new Ident(loc, name);
		}
	}


	Tree* unit1()
	{
		Location loc = lookahead().loc;
		switch(lookahead().type) {
		case '+':
			match('+');
			return new BinaryOp(loc, '+', "+", (Tree*)new Ident(loc, 0), unit1());
		case '-':
			match('-');
			return new BinaryOp(loc, '-', "-", (Tree*)new Ident(loc, 0), unit1());
		case '!':
			match('!');
			return new BinaryOp(loc, '!', "!", (Tree*)new Ident(loc, 0), unit1());
		default:
			return unit0();
		}
	}

	Tree* unit2()
	{
		Tree *left = unit1();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case '*':
				match('*');
				left = new BinaryOp(loc, '*', "*", left, unit1());
				break;
			case '/':
				match('/');
				left = new BinaryOp(loc, '/', "/", left, unit1());
				break;
			case '%':
				match('%');
				left = new BinaryOp(loc, '%', "%", left, unit1());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit3()
	{
		Tree *left = unit2();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case '+':
				match('+');
				left = new BinaryOp(loc, '+', "+", left, unit2());
				break;
			case '-':
				match('-');
				left = new BinaryOp(loc, '-', "-", left, unit2());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit4()
	{
		Tree *left = unit3();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case '<':
				match('<');
				left = new BinaryOp(loc, '<', "<", left, unit3());
				break;
			case LE:
				match(LE);
				left = new BinaryOp(loc, LE, "<=", left, unit3());
				break;
			case '>':
				match('>');
				left = new BinaryOp(loc, '>', ">", left, unit3());
				break;
			case GE:
				match(GE);
				left = new BinaryOp(loc, GE, ">=", left, unit3());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit5()
	{
		Tree *left = unit4();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case EQ:
				match(EQ);
				left = new BinaryOp(loc, EQ, "==", left, unit4());
				break;
			case NEQ:
				match(NEQ);
				left = new BinaryOp(loc, NEQ, "!=", left, unit4());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit6()
	{
		Tree *left = unit5();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case '^':
				match('^');
				left = new BinaryOp(loc, '^', "^", left, unit5());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit7()
	{
		Tree *left = unit6();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case LAND:
				match(LAND);
				left = new BinaryOp(loc, LAND, "&&", left, unit6());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit8()
	{
		Tree *left = unit7();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case LOR:
				match(LOR);
				left = new BinaryOp(loc, LOR, "||", left, unit7());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit9()
	{
		Tree *right = unit8();
		Location loc = lookahead().loc;
		switch(lookahead().type) {
		case '=':
			match('=');
			return new BinaryOp(loc, '=', "=", right, unit9());
		default:
			return right;
		}
	}

	void product(vector<Tree*> &exps)
	{
		Location loc = lookahead().loc;
		switch(lookahead().type) {
		case ')':
			break;
			FIRST_EXPR
				exps.push_back(expr());
			switch(lookahead().type) {
			case ',':
				match(',');
				break;
			case ')':
				break;
			default:
				report.issueError(SyntaxError(loc));
			}
			product(exps);
			break;
		default:
			report.issueError(SyntaxError(loc));
		}
	}

	Parser()
	{
		pos = 0;
	}

	~Parser()
	{
		if (prog != nullptr)
			delete prog;
	}

	void main()
	{
		lexer.start();
		Token cur;
		do {
			cur = lexer.getNext();
			token.push_back(cur);
		} while (cur.type != EXIT);
		if (lexer.isError())
			exit(0);
		prog = program();
		if (prog == nullptr) {
			puts("<empty>");
		} 

		prog->accept(typeVisitor);

		if (typeVisitor.isError()) {
			cerr << " --- Killed --- " << endl;
			exit(0);
		}

		prog->accept(runVisitor);
	}
} parser;

///
/// ::userd
///

void lexer_init()
{
	lexer.append(INCLUDE, "include");
	lexer.append(IOSTREAM, "iostream");
	lexer.append(CSTDIO, "cstdio");
	lexer.append(USING, "using");
	lexer.append(NAMESPACE, "namespace");
	lexer.append(STD, "std");

	lexer.append(INT, "int");

	lexer.setIdent(IDENT);
	lexer.setNumber(NUMBER);
	lexer.setError(ERROR);

	lexer.append(CIN, "cin");
	lexer.append(COUT, "cout");
	lexer.append(ENDL, "endl");
	lexer.append(PUTCHAR, "putchar");

	lexer.append(FOR, "for");
	lexer.append(WHILE, "while");
	lexer.append(IF, "if");
	lexer.append(ELSE, "else");
	lexer.append(RETURN, "return");

	lexer.append(LE, "<=");
	lexer.append(GE, ">=");
	lexer.append(NEQ, "!=");
	lexer.append(EQ, "==");
	lexer.append(LAND, "&&");
	lexer.append(LOR, "||");

	lexer.append(SHIFTL, "<<");
	lexer.append(SHIFTR, ">>");

	lexer.append(COMMENT, "//");
	lexer.setSimple("#+-*/%()[]{}=!^;,<>");
}

void init()
{
	lexer_init();
	sysIO = new Uoj98IO;
}

void clean()
{
	delete sysIO;
}

int main()
{
	init();
	parser.main();
	clean();
	return 0;
}

