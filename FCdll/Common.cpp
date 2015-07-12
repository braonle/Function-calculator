#include "Common.h"
#include <algorithm>

///Machinery epsilon
const long double e = 0.0000000001;
StNode* Stack::pointer = NULL;
AVLTree < Quantum > Drobot::global_tree;
Quantum* Drobot::ope_br = new OpenedBracket;
vector < string > Drobot::input_history;

bool isEqu(long double &arg1, long double &arg2)
{
	if (ABS(arg1 - arg2) < e)
		return true;
	else
		return false;
}
void change(Quantum* tmp1, Quantum* tmp2)
{
	if (tmp1->GetType() == func && tmp2->GetType() == func)
	{
		Function* _tmp1 = dynamic_cast<Function*>(tmp1);
		Function* _tmp2 = dynamic_cast<Function*>(tmp2);
		_tmp1->change(_tmp2);
	}
	else
		if (tmp1->GetType() == con && tmp2->GetType() == con)
		{
			Const* _tmp1 = dynamic_cast<Const*>(tmp1);
			Const* _tmp2 = dynamic_cast<Const*>(tmp2);
			_tmp1->change(_tmp2);
		}
		else
			throw(bad_re);
}
void change(ArgData* tmp1, ArgData* tmp2)
{
	return;
}
Quantum* newSame(Quantum* obj)
{
	if (obj->GetType() == func)
		return new Function(dynamic_cast<Function*>(obj));
	if (obj->GetType() == con)
		return new Const(dynamic_cast<Const*>(obj));
}


ArgData::ArgData(TKeyData *input, TInternalData _data)
{
	key = input;
	data = _data;
}
//Не забудь сделать delete
TKeyData* ArgData::GetName()
{
	TKeyData *tmp = new TKeyData;
	*tmp = *key;
	return tmp;
}
ArgData::~ArgData()
{
	if (key) delete key;
}


bool Quantum::isOpenedBracket()
{
	return false;
}
char Quantum::getPriority()
{
	return 0;
}
TKeyData* Quantum::GetName()
{
	return NULL;
}
short Quantum::getNumOfArguments()
{
	return 0;
}
bool Quantum::isUnchengable()
{
	return false;
}


Const::Const(string* input)
{
	ErrStruct* p = new ErrStruct;
	string* name_ = new string;
	p->pointer = name_;
	int iter = 0;
	goToNextSymbol(input, &iter);

	if (iter >= input->size())
		throw(p->code(invalid_alloc));

	if (isEmptySimbol((*input)[iter]) || (*input)[iter] <= '9' && (*input)[iter] >= '0' || (*input)[iter] == '.')
		throw(p->code(invalid_alloc));

	getName(name_, input, &iter);
	name = name_;
	goToNextSymbol(input, &iter);
	if (iter >= input->size())
		throw(p->code(invalid_alloc));

	if ((*input)[iter] == '(')
	{
		iter++;
		goToNextSymbol(input, &iter);
		if ((*input)[iter] != ')')
			throw(p->code(invalid_alloc));
		iter++;
		goToNextSymbol(input, &iter);
	}

	if ((*input)[iter] != '=')
		throw(p->code(invalid_alloc));

	name->push_back(1);
	name->push_back(1);
	iter++;
	
	EvalList pol_list;
	makePolandList(&pol_list, input, &iter, NULL, p);
	pol_list.evaluate();

	if (!Stack::isNormalRezult())
		throw(p->code(invalid_expression));

	value = new TInternalData(Stack::pop());
	delete p;
}
void Const::evaluate(AVLTree < ArgData > *tree)
{
	Stack::push(*value);
}
TypeOfQuantum Const::GetType()
{
	return con;
}
TKeyData* Const::GetName()
{
	string *st = new string(*name);
	return st;
}
Const::~Const()
{
	if (name) delete name;
	if (value) delete value;
}
void Const::change(Const* tmp)
{
	delete value;
	value = new TInternalData(*(tmp->value));
	delete name;
	name = new string(*(tmp->name));
}
void Const::becomeConst()
{
	isConst = true;
}
Const::Const(Const *c)
{
	value = new TInternalData(*(c->value));
	isConst = true;
	name = new string("sdf");
}
bool Const::isUnchengable()
{
	return isConst;
}


Value::Value(TInternalData *data)
{
	_data = data;
}
Value::Value(Value *v)
{
	_data = new TInternalData(*(v->_data));
}
TypeOfQuantum Value::GetType()
{
	return val;
}
void Value::evaluate(AVLTree < ArgData > *tree)
{
	Stack::push(*_data);
}
Value::~Value()
{
	if (_data)
		delete _data;
}


void Variable::evaluate(AVLTree < ArgData > *tree)
{
	Stack::push(tree->find(_name)->data);
}
Variable::Variable(TKeyData *name)
{
	_name = new TKeyData(*name);
}
Variable::Variable(Variable *v)
{
	_name = new TKeyData(*(v->_name));
}
Variable::~Variable()
{
	if (_name) delete _name;
}
TypeOfQuantum Variable::GetType()
{
	return var;
}
TKeyData* Variable::GetName()
{
	string *st = new string(*_name);
	return st;
}


Function::Function(string *input)
{
	ErrStruct* p = new ErrStruct;
	p->pointer;
	string* name_ = new string;
	p->pointer = name_;
	int iter = 0;
	goToNextSymbol(input, &iter);
	if (iter >= input->size())
		throw(p->code(invalid_alloc));
	getName(name_, input, &iter);
	goToNextSymbol(input, &iter);
	_name = name_;
	if (iter >= input->size())
		throw(p->code(invalid_alloc));

	if ((*input)[iter] != '(')
		throw(p->code(invalid_alloc));
	iter++;
	_ArgNum = 0;

	for (;;)
	{
		goToNextSymbol(input, &iter);
		if ((*input)[iter] == ')' && !_ArgNames.empty())
		{
			_ArgNum++;
			iter++;
			break;
		}

		if ((*input)[iter] == ',')
		{
			_ArgNum++;
			iter++;
			continue;
		}

		char c = (*input)[iter];
		if (c <= '9' && c >= '0' || c == '.' || c == '(')
			throw(p->code(invalid_alloc));
		string n_of_arg;
		getName(&n_of_arg, input, &iter);
		if (find(&_ArgNames, &n_of_arg))
			throw(p->code(invalid_alloc));
		_ArgNames.push_back(n_of_arg);

		goToNextSymbol(input, &iter);
		if ((*input)[iter] != ',' && (*input)[iter] != ')')
			throw(p->code(invalid_alloc));
	}

	goToNextSymbol(input, &iter);
	_name->push_back((char) (_ArgNum / 255 + 1));
	_name->push_back((char) (_ArgNum % 255 + 1));
	if ((*input)[iter] != '=')
		throw(p->code(invalid_alloc));
	reverse(_ArgNames.begin(), _ArgNames.end());
	iter++;
	makePolandList(&_list, input, &iter, &_ArgNames, p);
	delete p;
}
short Function::getNumOfArguments()
{
	return _ArgNum;
}
void Function::evaluate(AVLTree < ArgData > *_tree)
{
	AVLTree < ArgData > *tree = new AVLTree < ArgData >;
	for (int i = 0; i < _ArgNum; ++i)
		tree->insert(new ArgData(&_ArgNames[i], Stack::pop()));
	_list.evaluate(tree);
}
TKeyData* Function::GetName()
{
	string *st = new string(*_name);
	return st;
}
char Function::getPriority()
{
	return 3;
}
TypeOfQuantum Function::GetType()
{
	return func;
}
void Function::goodListFree()
{
	_list.goodFree();
}
void Function::change(Function *tmp)
{
	_list.goodFree();
	_list.change(tmp->_list);
	tmp->_list.nu();
	_ArgNames.clear();
	for (int i = 0; i < tmp->_ArgNames.size(); ++i)
		_ArgNames.push_back(tmp->_ArgNames[i]);
	_ArgNum = tmp->_ArgNum;
	delete _name;
	_name = new string(*(tmp->_name));
}
Function::~Function()
{
	if (_name) delete _name;
	_ArgNames.clear();
}
void Function::becomeConst()
{
	isConst = true;
}
Function::Function(Function *from) : _list(from->_list), _ArgNum(from->_ArgNum), _ArgNames(*(new vector < TKeyData >(from->_ArgNames))), isConst(true)
{
	_name = new string("sdf");
}
bool Function::isUnchengable()
{
	return isConst;
}


Plus::Plus()
{
	key = new string("+");
}
void Plus::evaluate(AVLTree < ArgData > *tree)
{
	Stack::push(Stack::pop() + Stack::pop());
}
char Plus::getPriority()
{
	return 1;
}
TKeyData* Plus::GetName()
{
	string* st = new string(*key);
	return st;
}

Substract::Substract()
{
	key = new string("-");
}
void Substract::evaluate(AVLTree < ArgData > *tree)
{
	TInternalData tmp = Stack::pop();
	Stack::push(Stack::pop() - tmp);
}
char Substract::getPriority()
{
	return 1;
}
TKeyData* Substract::GetName()
{
	string* st = new string(*key);
	return st;
}


Multiplication::Multiplication()
{
	key = new string("*");
}
void Multiplication::evaluate(AVLTree < ArgData > *tree)
{
	Stack::push(Stack::pop() * Stack::pop());
}
char Multiplication::getPriority()
{
	return 2;
}
TKeyData* Multiplication::GetName()
{
	string* st = new string(*key);
	return st;
}


Division::Division()
{
	key = new string("/");
}
void Division::evaluate(AVLTree < ArgData > *tree)
{
	TInternalData tmp = Stack::pop();
	long double n = 0.0;

	if (isEqu(tmp, n))
		throw(division_by_zero);
	else
		Stack::push(Stack::pop() / tmp);
}
char Division::getPriority()
{
	return 2;
}
TKeyData* Division::GetName()
{
	string* st = new string(*key);
	return st;
}


Power::Power()
{
	key = new string("^");
}
void Power::evaluate(AVLTree < ArgData > *tree)
{
	
	TInternalData tmp = Stack::pop();
	long double o_t = 1.0 / 3.0;
	if (isEqu(tmp, o_t))
	{
		Stack::push(cbrtl(Stack::pop()));
		return;
	}

	if (tmp < e)
		throw(bad_power);
	Stack::push(pow(Stack::pop(), tmp));
}
char Power::getPriority()
{
	return 2;
}
TKeyData* Power::GetName()
{
	string* st = new string(*key);
	return st;
}


StNode::StNode(TInternalData _data, StNode* p) : prev(p), data(_data){}


void Stack::clear()
{
	StNode* tmp;

	while (pointer)
	{
		tmp = pointer->prev;
		delete pointer;
		pointer = tmp;
	}
}
void Stack::push(TInternalData data)
{
	if (pointer)
		pointer = new StNode(data, pointer);
	else
		pointer = new StNode(data);
}
TInternalData Stack::pop()
{
	if (!pointer)
		throw(invalid_expression);
	TInternalData temp = pointer->data;
	StNode* tmp = pointer->prev;
	delete pointer;
	pointer = tmp;
	return temp;
}
bool Stack::isEmpty()
{
	if (pointer)
		return false;
	return true;
}
bool Stack::isNormalRezult()
{
	if (!pointer)
		return false;
	if (pointer->prev)
		return false;
	return true;
}
Stack::~Stack()
{
	while (pointer)
	{
		StNode *tmp = pointer->prev;
		delete pointer;
		pointer = tmp;
	}
}


StdFunction::StdFunction(short ArgNum) : _ArgNum(ArgNum){};
StdFunction::~StdFunction()
{
	if (key) delete key;
}
TypeOfQuantum StdFunction::GetType()
{
	return std_func;
}
TKeyData* StdFunction::GetName()
{
	TKeyData *tmp = new TKeyData(*key);
	return tmp;
}
short StdFunction::getNumOfArguments()
{
	return _ArgNum;
}
char StdFunction::getPriority()
{
	return 3;
}


short BinaryOp::getNumOfArguments()
{
	return 2;
}
TypeOfQuantum BinaryOp::GetType()
{
	return bin_op;
}
BinaryOp::~BinaryOp()
{
	if (key) delete key;
}


void OpenedBracket::evaluate(AVLTree < ArgData > *tree)
{

}
char OpenedBracket::getPriority()
{
	return 0;
}
TypeOfQuantum OpenedBracket::GetType()
{
	return op_br;
}
OpenedBracket::OpenedBracket()
{
	key = new TKeyData("(");
}
OpenedBracket::~OpenedBracket()
{
	delete key;
}
TKeyData* OpenedBracket::GetName()
{
	string* st = new string(*key);
	return st;
}


EvalList::EvalList()
{
	_root = _tail = NULL;
}
EvalList::EvalList(EvalList &l)
{
	EvalNode* p = l._root;
	_root = _tail = NULL;

	for (; p; p = p->next)
	{
		if (p->data->GetType() == var)
			add(new Variable(dynamic_cast<Variable*>(p->data)));
		else
			if (p->data->GetType() == val)
				add(new Value(dynamic_cast<Value*>(p->data)));
			else
				if (p->data->isUnchengable())
					add(newSame(p->data));
				else
					add(p->data);
	}
}
void EvalList::change(EvalList& tmp)
{
	_root = tmp._root;
	_tail = tmp._tail;
}
EvalList::~EvalList()
{
	EvalNode* tmp;

	while (_root)
	{
		tmp = _root->next;
		delete _root;
		_root = tmp;
	}
}
void EvalList::add(Quantum *input)
{
	if (_root == NULL) 
		_root = _tail = new EvalNode(input);
	else
	{
		_tail->next = new EvalNode(input);
		_tail = _tail->next;
	}
}
void EvalList::add(Quantum *input, EvalNode* place)
{
	if (!place)
	{
		EvalNode* tmp = _root;
		_root = new EvalNode(input);
		_root->next = tmp;
	}
	else
	{
		EvalNode* tmp = place->next;
		place->next = new EvalNode(input);
		place->next->next = tmp;
	}
}
void EvalList::goodFree()
{
	EvalNode* p = _root;

	for (; p; p = p->next)
		if (p->data->GetType() == var || p->data->GetType() == val || p->data->isUnchengable())
			delete p->data;
}
void EvalList::evaluate(AVLTree < ArgData > *tree)
{
	EvalNode* tmp = _root;

	while (tmp)
	{
		tmp->data->evaluate(tree);
		tmp = tmp->next;
	}
}
void EvalList::nu()
{
	_root = NULL;
}


Sine::Sine() : StdFunction(1)
{
	key = new string("sin");
	key->push_back(1);
	key->push_back(2);
}
void Sine::evaluate(AVLTree<ArgData> *tree)
{
	Stack::push(sin(Stack::pop()));
}


Cosine::Cosine() : StdFunction(1)
{
	key = new string("cos");
	key->push_back(1);
	key->push_back(2);
}
void Cosine::evaluate(AVLTree<ArgData> *tree)
{
	Stack::push(cos(Stack::pop()));
}


HyperbolicSine::HyperbolicSine() : StdFunction(1)
{
	key = new string("sinh");
	key->push_back(1);
	key->push_back(2);
}
void HyperbolicSine::evaluate(AVLTree <ArgData> *tree)
{
	Stack::push(sinh(Stack::pop()));
}


HyperbolicCosine::HyperbolicCosine() : StdFunction(1)
{
	key = new string("cosh");
	key->push_back(1);
	key->push_back(2);
}
void HyperbolicCosine::evaluate(AVLTree<ArgData> *tree)
{
	Stack::push(cosh(Stack::pop()));
}


Tangent::Tangent() : StdFunction(1)
{
	key = new string("tg");
	key->push_back(1);
	key->push_back(2);
}
void Tangent::evaluate(AVLTree<ArgData> *tree)
{
	TInternalData tmp = Stack::pop();
	TInternalData tmp1 = tmp;

	while (tmp1 > M2Pi)
		tmp -= M2Pi;
	long double k1 = M_PI_2, k2 = -M_PI_2;
	if (isEqu(tmp, k1) || isEqu(tmp, k2))
	{
		Stack::clear();
		throw invalid_value;
	}
	else Stack::push(tan(tmp));
}


Cotangent::Cotangent() : StdFunction(1)
{
	key = new string("ctg");
	key->push_back(1);
	key->push_back(2);
}
void Cotangent::evaluate(AVLTree<ArgData> *tree)
{
	TInternalData tmp = Stack::pop();
	TInternalData tmp1 = tmp;

	long double k1 = M_PI, k2 = 0.0;
	while (tmp1 > M2Pi) tmp -= M2Pi;

	if (isEqu(tmp, k1) || isEqu(tmp, k2))
	{
		Stack::clear();
		throw invalid_value;
	}
	else Stack::push(cos(tmp)/sin(tmp));
}


HyperbolicTangent::HyperbolicTangent() : StdFunction(1)
{
	key = new string("tgh");
	key->push_back(1);
	key->push_back(2);
}
void HyperbolicTangent::evaluate(AVLTree<ArgData> *tree)
{
	Stack::push(tanh(Stack::pop()));
}


Arcsine::Arcsine() : StdFunction(1)
{
	key = new string("arcsin");
	key->push_back(1);
	key->push_back(2);
}
void Arcsine::evaluate(AVLTree<ArgData> *tree)
{
	TInternalData tmp = Stack::pop();

	if (tmp < -1 || tmp > 1)
	{
		Stack::clear();
		throw invalid_value;
	}
	else Stack::push(asin(tmp));
}


Arccosine::Arccosine() : StdFunction(1)
{
	key = new string("arccos");
	key->push_back(1);
	key->push_back(2);
}
void Arccosine::evaluate(AVLTree<ArgData> *tree)
{
	TInternalData tmp = Stack::pop();

	if (tmp < -1 || tmp > 1)
	{
		Stack::clear();
		throw invalid_operand;
	}
	else Stack::push(acos(tmp));
}


HyperbolicArcsine::HyperbolicArcsine() : StdFunction(1)
{
	key = new string("arcsinh");
	key->push_back(1);
	key->push_back(2);
}
void HyperbolicArcsine::evaluate(AVLTree<ArgData> *tree)
{
	Stack::push(asinh(Stack::pop()));
}


HyperbolicArccosine::HyperbolicArccosine() : StdFunction(1)
{
	key = new string("arccosh");
	key->push_back(1);
	key->push_back(2);
}
void HyperbolicArccosine::evaluate(AVLTree<ArgData> *tree)
{
	Stack::push(acosh(Stack::pop()));
}


Arctangent::Arctangent() : StdFunction(1)
{
	key = new string("arctg");
	key->push_back(1);
	key->push_back(2);
}
void Arctangent::evaluate(AVLTree<ArgData> *tree)
{
	Stack::push(atan(Stack::pop()));
}


Arccotangent::Arccotangent() : StdFunction(1)
{
	key = new string("arcctg");
	key->push_back(1);
	key->push_back(2);
}
void Arccotangent::evaluate(AVLTree<ArgData> *tree)
{
	TInternalData tmp = Stack::pop();
	
	long double n = 0.0;
	if (isEqu(tmp, n))
		Stack::push(M_PI_2);
	else
		Stack::push(1 / atan(tmp));
}


HyperbolicArctangent::HyperbolicArctangent() : StdFunction(1)
{
	key = new string("arctgh");
	key->push_back(1);
	key->push_back(2);
}
void HyperbolicArctangent::evaluate(AVLTree<ArgData> *tree)
{
	Stack::push(atanh(Stack::pop()));
}

Logarithm::Logarithm() : StdFunction(2)
{
	key = new string("log");
	key->push_back(1);
	key->push_back(3);
}
void Logarithm::evaluate(AVLTree<ArgData> *tree)
{
	TInternalData arg = Stack::pop();
	TInternalData base = Stack::pop();


	long double ed = 1.0;
	if (isEqu(base, ed) || base <= e || arg <= e)
	{
		Stack::clear();
		throw invalid_value;
	}
	else Stack::push(log(arg)/log(base));
}


Ln::Ln() : StdFunction(1)
{
	key = new string("ln");
	key->push_back(1);
	key->push_back(2);
}
void Ln::evaluate(AVLTree<ArgData> *tree)
{
	TInternalData arg = Stack::pop();

	if (arg <= e)
	{
		Stack::clear();
		throw invalid_value;
	}
	else Stack::push(log(arg));
}


Lg::Lg() : StdFunction(1)
{
	key = new string("lg");
	key->push_back(1);
	key->push_back(2);
}
void Lg::evaluate(AVLTree<ArgData> *tree)
{
	TInternalData arg = Stack::pop();

	if (arg <= e)
	{
		Stack::clear();
		throw invalid_value;
	}
	else Stack::push(log10(arg));
}


Exp::Exp() : StdFunction(1)
{
	key = new string("exp");
	key->push_back(1);
	key->push_back(2);
}
void Exp::evaluate(AVLTree<ArgData> *tree)
{
	Stack::push(exp(Stack::pop()));
}


Drobot::Drobot()
{
	global_tree.insert(new Plus());
	global_tree.insert(new Multiplication());
	global_tree.insert(new Division());
	global_tree.insert(new Substract());
	global_tree.insert(new Power());
	global_tree.insert(new Sine());
	global_tree.insert(new Cosine());
	global_tree.insert(new HyperbolicSine());
	global_tree.insert(new HyperbolicCosine());
	global_tree.insert(new Tangent());
	global_tree.insert(new Cotangent());
	global_tree.insert(new HyperbolicTangent());
	global_tree.insert(new Arcsine());
	global_tree.insert(new Arccosine());
	global_tree.insert(new Arctangent());
	global_tree.insert(new Arccotangent());
	global_tree.insert(new HyperbolicArctangent());
	global_tree.insert(new Logarithm());
	global_tree.insert(new Ln());
	global_tree.insert(new Lg());
	global_tree.insert(new Exp());
	global_tree.insert(new HyperbolicArcsine());
	global_tree.insert(new HyperbolicArccosine());
	string *st = new string("e = 2.71828182845904523536");
	global_tree.insert(new Const(st));
	delete st;
	st = new string("pi = 3.14159265358979323846");
	global_tree.insert(new Const(st));
	delete st;
}
TInternalData* Drobot::drive(string *s)
{
	Stack::clear();

	switch (analisis(*s))
	{
	case cons:
		global_tree.insert(new Const(s));
		Drobot::input_history.push_back(*s);
		return NULL;
		break;
	case fun:
		global_tree.insert(new Function(s));
		Drobot::input_history.push_back(*s);
		return NULL;
		break;
	case eval:
		return new TInternalData(evaluateConstExpr(s));
		break;
	case err:
		throw(invalid_alloc);
	}
	return NULL;
}
vector < string >* Drobot::getHistory()
{
	return &input_history;
}
Drobot::~Drobot()
{

}

void kostyl(Quantum* tmp)
{
	if (tmp->GetType() != func)
		return;
	Function* f = dynamic_cast<Function*>(tmp);
	f->goodListFree();
}

bool isQ(Quantum *)
{
	return true;
}

bool isQ(ArgData *)
{
	return false;
}