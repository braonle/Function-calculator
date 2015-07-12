#pragma once
#include <string>
#include <vector>
#include "function.h"

#define _USE_MATH_DEFINES

#include <math.h>

#define ABS(X) ((X > 0) ? X : -1 * X)
#define M2Pi M_PI*2

using namespace std;

/** Type, used as a key one for search in a tree*/
typedef string TKeyData;

#include "AVL.h"

///Default arithmetic operators which are recognised
const char ar_operators [] = { '+', '-', '*', '/', '^', '\0' };

///Recognisable Quantum types - Function, Value, Const, Variable, OpenBracket, StdFunction, BinaryOp, NULL
enum TypeOfQuantum { func, val, con, var, op_br, std_func, bin_op, nothing };

///Error codes given through exceptions
enum ErrCode 
{
	invalid_name, invalid_expression, division_by_zero, invalid_operand, invalid_value, \
	memory_error, need_bracket, unknown_name, you_are_fool, wrong_number_of_argument,
	bad_power, invalid_bracket_position, invalid_alloc, bad_re, bad_position_of_reserved_symbol
};

class Quantum;

///Used for exceptions, extention to ErrCode, maintains correct deleting of function names (pointer)
struct ErrStruct
{
	string* pointer;
	ErrCode _code;
	ErrStruct* code(ErrCode c)
	{
		_code = c;
		return this;
	}
};

/// Type of main data used for calculations
typedef long double TInternalData;

///Used for tree of arguments (created when Function is called)
struct ArgData
{
	ArgData(TKeyData *input, TInternalData);
	/** Returns copy of key, no direct access to it*/
	TKeyData* GetName();
	/** Data which is used to replace a name of a variable*/
	TInternalData data;
	~ArgData();
private:
	/** Symbolic name of a variable*/
	TKeyData *key;
};

/// Basic parent class for elements of Evaluation List (EvalList)
class Quantum
{
public:
	///Abstract function of self-evaluation using static Stack
	virtual void evaluate(AVLTree < ArgData > *tree = NULL) = 0;
	virtual bool isUnchengable();
	virtual TypeOfQuantum GetType() = 0;
	///Used to recognise OpenBracket
	virtual bool isOpenedBracket();
	virtual short getNumOfArguments();
	///Returns arithmetic priority of operation
	virtual char getPriority();								
	virtual TKeyData* GetName();
	virtual ~Quantum(){};
};

/// Contains a constant value of a function
class Value : public Quantum
{
	/** Stored value*/
	TInternalData *_data;

public:
	/** Accepts data to store
	\param data		-	data to be stored	
	*/
	Value(TInternalData* data);
	Value(Value *);
	/** Gives the actual value of itself */
	virtual void evaluate(AVLTree < ArgData > *tree = NULL);
	virtual TypeOfQuantum GetType();
	virtual ~Value();
};

/// Contains a variable which returns a value according to a Parameter Tree
class Variable : public Quantum
{
	/** Variable name used as a key for search in Parameter Tree*/
	TKeyData *_name;
public:
	/**
		\param	name	-	symbolic name of this variable
	*/
	Variable(TKeyData *name);												//—троку надо копировать
	Variable(Variable *);
	/** Gives the actual value of itself according to Parameter Tree*/
	virtual void evaluate(AVLTree < ArgData > *tree);
	TKeyData* GetName();
	virtual TypeOfQuantum GetType();
	//virtual bool isOpenedBreacket();
	virtual ~Variable();
};

/// Element of Evaluation List
struct EvalNode
{
	/** Pointer to the next member of the list*/
	EvalNode *next;
	/** Pointer to the actual member of the list: Value, Variable, Function*/
	Quantum *data;
	EvalNode(Quantum* input) : data(input), next(NULL) {};
};

/// Storage of expression as a List
class EvalList
{
	/**Beginning of the List*/
	EvalNode *_root;
public:
	/**End of the List - for quicker adding*/
	EvalNode *_tail;

	EvalList();
	EvalList(EvalList &l);
	/** Add a new member
		\param	data	-	Value, Variable or Function	
		\return		0 if successful, 1 if error occurred
	*/
	void add(Quantum *data);
	void add(Quantum *, EvalNode *);
	///Deletes Value and Variable, Function and Const are skipped
	void goodFree();
	///Maintains change of poland list to a new one
	void change(EvalList&);
	///_root = NULL
	void nu();
	/** Starts evaluation of the whole list*/
	void evaluate(AVLTree < ArgData > *tree = NULL);
	~EvalList();
};

///Contains function with evaluating instruments
class Function : public Quantum
{
	///Function name
	TKeyData *_name = NULL;
	///Poland list od input
	EvalList _list;
	///Number of arguments
	short _ArgNum;
	///Argument names
	vector < TKeyData > _ArgNames;
	bool isConst = false;
public:
	//const char priority = 3;
	Function(string *input);
	Function(Function*);
	TKeyData* GetName();
	void becomeConst();
	short getNumOfArguments();
	virtual bool isUnchengable();
	virtual char getPriority();
	virtual void evaluate(AVLTree < ArgData > *tree = NULL);
	void change(Function*);
	virtual TypeOfQuantum GetType();
	void goodListFree();
	virtual ~Function();
};

///Stack node
struct StNode
{
	StNode *prev;
	TInternalData data;
	StNode(TInternalData, StNode *p = NULL);
};

///Used for calculations
class Stack
{
	static StNode *pointer;
public:
	static void push(TInternalData );
	static TInternalData pop();
	static bool isEmpty();
	static void clear();
	static bool isNormalRezult();
	~Stack();
};

///The same as Function in action, stores value
class Const : public Quantum
{
	TInternalData *value = NULL;
	TKeyData *name = NULL;
	bool isConst = false;
public:
	Const(string *input);
	Const(Const*);
	virtual void evaluate(AVLTree < ArgData > *tree = NULL);
	virtual TypeOfQuantum GetType();
	virtual bool isUnchengable();
	void becomeConst();
	void change(Const*);
	TKeyData* GetName();
	virtual ~Const();
};

///Abstract parent class for standart arithmetic functions
class StdFunction : public Quantum
{
	//const char priority = 3;
	short _ArgNum;
protected:
	TKeyData *key;
public:
	StdFunction(short ArgNum);
	short getNumOfArguments();
	virtual void evaluate(AVLTree < ArgData > *tree = NULL) = 0;
	virtual TypeOfQuantum GetType();
	virtual char getPriority();
	virtual TKeyData* GetName();
	//virtual bool isOpenedBreacket();
	virtual ~StdFunction();
};

///Abstract parent class for binary operations
class BinaryOp : public Quantum
{
protected:
	TKeyData *key;
public:
	short getNumOfArguments();
	virtual void evaluate(AVLTree < ArgData > *tree = NULL) = 0;
	virtual TypeOfQuantum GetType();
	virtual TKeyData* GetName() = 0;
	//virtual bool isOpenedBreacket();
	virtual ~BinaryOp();
};

///For service use
class OpenedBracket : public Quantum
{
	//const char priority = 0;
	TKeyData *key;
public:
	OpenedBracket();
	virtual TypeOfQuantum GetType();
	virtual char getPriority();
	virtual void evaluate(AVLTree < ArgData > *tree = NULL);
	virtual TKeyData* GetName();
	//virtual bool isOpenedBreacket();
	~OpenedBracket();
};

class Plus : public BinaryOp
{
public:
	//const char priority = 1;
	Plus();
	virtual void evaluate(AVLTree < ArgData > *tree);
	virtual char getPriority();
	virtual TKeyData* GetName();
	//virtual bool isOpenedBreacket();
};

class Substract : public BinaryOp
{
public:
	//const char priority = 1;
	Substract();
	virtual void evaluate(AVLTree < ArgData > *tree);
	virtual char getPriority();
	virtual TKeyData* GetName();
	//virtual bool isOpenedBreacket();
};

class Multiplication : public BinaryOp
{
	//const char priority = 2;
public:
	Multiplication();
	virtual void evaluate(AVLTree < ArgData > *tree);
	virtual char getPriority();
	virtual TKeyData* GetName();
	//virtual bool isOpenedBreacket();
};

class Division : public BinaryOp
{
	//const char priority = 2;
public:
	Division();
	virtual void evaluate(AVLTree < ArgData > *tree);
	virtual char getPriority();
	virtual TKeyData* GetName();
	//virtual bool isOpenedBreacket();
};

class Power : public BinaryOp
{
	//const char priority = 3;
public:
	Power();
	virtual void evaluate(AVLTree < ArgData > *tree);
	virtual char getPriority();
	virtual TKeyData* GetName();
	//virtual bool isOpenedBreacket();
};

class Sine : public StdFunction
{
	// key = "sin"
public:
	Sine();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class Cosine : public StdFunction
{
	// key = "cos"
public:
	Cosine();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class HyperbolicSine : public StdFunction
{
	// key = "sinh"
public:
	HyperbolicSine();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class HyperbolicCosine : public StdFunction
{
	// key = "cosh"
public:
	HyperbolicCosine();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class Tangent : public StdFunction
{
	// key = "tg"
public:
	Tangent();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class Cotangent : public StdFunction
{
	// key = "ctg"
public:
	Cotangent();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class HyperbolicTangent : public StdFunction
{
	// key = "tgh"
public:
	HyperbolicTangent();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class Arcsine : public StdFunction
{
	// key = "arcsin"
public:
	Arcsine();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class Arccosine : public StdFunction
{
	// key = "arccos"
public:
	Arccosine();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class HyperbolicArcsine : public StdFunction
{
	// key = "arcsinh"
public:
	HyperbolicArcsine();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class HyperbolicArccosine : public StdFunction
{
	// key = "arccosh"
public:
	HyperbolicArccosine();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class Arctangent : public StdFunction
{
	// key = "arctg"
public:
	Arctangent();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class Arccotangent : public StdFunction
{
	// key = "arcctg"
public:
	Arccotangent();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class HyperbolicArctangent : public StdFunction
{
	// key = "arctgh"
public:
	HyperbolicArctangent();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class Logarithm : public StdFunction
{
	// key = "log", log(x,y): x - основание, y - аргумент
public:
	Logarithm();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class Ln : public StdFunction
{
	// key = "ln"
public:
	Ln();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class Lg : public StdFunction
{
	// key = "lg"
public:
	Lg();
	virtual void evaluate(AVLTree <ArgData> *tree);
};

class Exp : public StdFunction
{
	// key = "exp"
public:
	Exp();
	virtual void evaluate(AVLTree <ArgData> *tree);
};


/**Extracts the name of argument (Value, Variable, Function)
\param	name	-	target container for the name
\param	s		-	source 
\param	iter	-	current cursor position in the source
*/
void getName(string *name, string *s, int *iter);
///Go to the next treated symbol (not from empty[])
void goToNextSymbol(string *s, int *iter);
/**Convert string into the polish form
\param	list	- pointer to the target list
\param	s		- source
\param	v		- if Function - pointer to the vector of arguments
\param	p		- used to deal with memory leaks
*/
void makePolandList(EvalList *list, string *s, int *iter, vector < string > * v = NULL, ErrStruct* p = NULL);
TInternalData evaluateConstExpr(string *s);

///For service use
void kostyl(Quantum *);
///Exchanges function if EvalList
void change(Quantum* tmp1, Quantum* tmp2);
///Does nothing (only override)
void change(ArgData* tmp1, ArgData* tmp2);
///Searches for an argument
bool find(vector < string > * v, string *s);

///Calculating class
class Drobot
{
	static vector < string > input_history;
public:
	static AVLTree < Quantum > global_tree;
	static Quantum* ope_br;
	Drobot();
	/**Calculates the algebraic expression
	\param	s	-	pointer to expression (do not delete it manually!)
	\return	calculated result
	*/
	TInternalData* drive(string *s);
	vector < string >* getHistory();
	~Drobot();
};

///Returns true
bool isQ(Quantum *);
///Returns false
bool isQ(ArgData *);