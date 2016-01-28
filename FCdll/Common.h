#pragma once
#include <string>
#include <vector>
#include "function.h"
#include "Matrix.h"

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

bool isEqu(const long double &arg1, const long double &arg2);

///Recognisable Quantum types - Function, Value, Const, Variable, OpenBracket, StdFunction, BinaryOp, NULL
enum TypeOfQuantum { func, val, con, var, op_br, std_func, bin_op, nothing };

///Error codes given through exceptions
enum ErrCode 
{
	invalid_name, invalid_expression, division_by_zero, invalid_operand, invalid_value,
	memory_error, need_bracket, unknown_name, you_are_fool, wrong_number_of_argument,
	bad_power, invalid_bracket_position, invalid_alloc, bad_re, bad_position_of_reserved_symbol
};

class Quantum;

///Used for exceptions, extention to ErrCode, maintains correct deleting of function names (pointer)
struct ErrStruct
{
	vector <void * > vec;
	ErrCode _code;
	void add(void *ptr);
	ErrStruct* code(ErrCode c);
};

/// Type of main data used for calculations
class TInternalData
{
	DataNS::Data::DataWrap* data_num = nullptr;
	MatrixNS::Matrix* data_mat = nullptr;
	bool must_not_delete = false;

	bool _isNumber();
	bool _isMatrix();
	TInternalData(DataNS::Data::DataWrap* tmp);
	void deleteContent();
public:
	TInternalData(InsideType re, InsideType im = 0);
	TInternalData(MatrixNS::Matrix* ptr);
	TInternalData(TInternalData& arg);
	TInternalData& notToDeleteContent();
	/**
		It is nessesary to use it when the object is returned from a function.
	*/
	TInternalData& mustDeleteContent();
	DataNS::Data::DataWrap* getData();
	~TInternalData();
	TInternalData clone();
	bool isZero();
	void setValue(InsideType re, InsideType im = 0);
	//You should not use theese operators till you don't realise the meaning of the flag "must_not_delete".
	TInternalData& operator += (TInternalData& op);
	TInternalData& operator -= (TInternalData& op);
	TInternalData& operator *= (TInternalData& op);
	TInternalData& operator /= (TInternalData& op);
	/*TInternalData& operator = (TInternalData& tmp)
	{
		if (!must_not_delete)
			deleteContent();
		tmp.notToDeleteContent();
		this->mustDeleteContent();
		this->data_mat = tmp.data_mat;
		this->data_num = tmp.data_num;
		return *this;
	}*/
	friend ostream& operator<<(ostream& out, TInternalData& src);
};

namespace SpecMath
{
	TInternalData pow(TInternalData, TInternalData);
	TInternalData sin(TInternalData);
	TInternalData cos(TInternalData);
	TInternalData sinh(TInternalData);
	TInternalData cosh(TInternalData);
	TInternalData tan(TInternalData);
	TInternalData tanh(TInternalData);
	TInternalData asin(TInternalData);
	TInternalData acos(TInternalData);
	TInternalData asinh(TInternalData);
	TInternalData acosh(TInternalData);
	TInternalData atan(TInternalData);
	TInternalData atanh(TInternalData);
	TInternalData log(TInternalData);
	TInternalData log10(TInternalData);
	TInternalData exp(TInternalData);
}

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
	virtual ~Quantum();
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
	EvalNode(Quantum* input);
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
	StNode(TInternalData&, StNode *p = NULL);
	void notToDeletePTR();
	~StNode();
};

///Used for calculations
class Stack
{
	static StNode *pointer;
public:
	static void push(TInternalData&);
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
	TKeyData *key;
public:
	OpenedBracket();
	virtual TypeOfQuantum GetType();
	virtual char getPriority();
	virtual void evaluate(AVLTree < ArgData > *tree = NULL);
	virtual TKeyData* GetName();
	~OpenedBracket();
};

class Plus : public BinaryOp
{
public:
	Plus();
	virtual void evaluate(AVLTree < ArgData > *tree);
	virtual char getPriority();
	virtual TKeyData* GetName();
};

class Substract : public BinaryOp
{
public:
	Substract();
	virtual void evaluate(AVLTree < ArgData > *tree);
	virtual char getPriority();
	virtual TKeyData* GetName();
};

class Multiplication : public BinaryOp
{
public:
	Multiplication();
	virtual void evaluate(AVLTree < ArgData > *tree);
	virtual char getPriority();
	virtual TKeyData* GetName();
};

class Division : public BinaryOp
{
public:
	Division();
	virtual void evaluate(AVLTree < ArgData > *tree);
	virtual char getPriority();
	virtual TKeyData* GetName();
};

class Power : public BinaryOp
{
public:
	Power();
	virtual void evaluate(AVLTree < ArgData > *tree);
	virtual char getPriority();
	virtual TKeyData* GetName();
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
//void getName(string *name, string *s, int *iter);
///Go to the next treated symbol (not from empty[])
//void goToNextSymbol(string *s, int *iter);

///For service use
void kostyl(Quantum *);
///Exchanges function if EvalList
void change(Quantum*, Quantum*);
///Does nothing (only override)
void change(ArgData*, ArgData*);
///Searches for an argument
bool find(vector < string > *, string*);

///Calculating class
class Drobot
{
	static vector < string > input_history;
	Drobot();
	static bool f;
public:
	static AVLTree < Quantum > global_tree;
	static Quantum* ope_br;
	
	/**Calculates the algebraic expression
	\param	s	-	pointer to expression (do not delete it manually!)
	\return	calculated result
	*/
	static TInternalData* drive(string *);
	vector < string >* getHistory();
	~Drobot();
};

class Interpreter
{
	string *s;
	int *iter;
	ErrStruct *p;
	vector < string > *v;
public:
	Interpreter(string*, int *iter_ = nullptr, ErrStruct *p_ = nullptr, vector < string > *v_ = nullptr);
	void setVector(vector < string > *);
	void throw_(ErrCode);
	void getName(string*);
	bool isConstName();
	void getValue(TInternalData*);
	void goToNextSymbol();
	bool isConstMod();

	class HistoryStack;
	class NodeOfHistoryStack;
	///Stack of arithmetic operands, used to convert usual expression to polish form
	class PolStack;
	class NodeOfPolStack;

	bool find(string*);
	bool find(Quantum*);
	void makePolandList(EvalList*);
	TInternalData evaluateConstExpr();
};

///Returns true
bool isQ(Quantum *);
///Returns false
bool isQ(ArgData *);