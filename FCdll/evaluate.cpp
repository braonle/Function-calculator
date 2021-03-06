#include "Common.h"

///Node of History Stack
class Interpreter::NodeOfHistoryStack
{
	Interpreter::NodeOfHistoryStack *prev;
	string* name;
	bool const_mod;
	EvalNode* place;
public:
	TypeOfQuantum type;
	///For service
	short number_of_argumets;

	NodeOfHistoryStack(TypeOfQuantum _type, string* _name, EvalNode* _place, bool _const_mod, Interpreter::NodeOfHistoryStack* pointer = NULL) : prev(pointer), type(_type), number_of_argumets(0), name(_name), const_mod(_const_mod), place(_place)
	{

	}
	~NodeOfHistoryStack()
	{
		if (name)
			delete name;
	}

	friend class HistoryStack;
};

///History stack, contains types
class Interpreter::HistoryStack
{
	Interpreter::NodeOfHistoryStack* pointer;
	Interpreter *int_obj;
public:

	HistoryStack(Interpreter *obj_) : pointer(NULL), int_obj(obj_)
	{

	}

	void push(TypeOfQuantum type, string* name = NULL, EvalNode* place = NULL, bool const_mod = false)
	{
		if (pointer)
			pointer = new Interpreter::NodeOfHistoryStack(type, name, place, const_mod, pointer);
		else
			pointer = new Interpreter::NodeOfHistoryStack(type, name, place, const_mod);
	}

	bool isEmpty()
	{
		if (pointer)
			return false;
		else
			return true;
	}

	void pop()
	{
		if (isEmpty())
			return;
		Interpreter::NodeOfHistoryStack* tmp = pointer->prev;
		delete pointer;
		pointer = tmp;
	}

	void clear()
	{
		Interpreter::NodeOfHistoryStack* tmp;
		tmp = pointer->prev;
		delete pointer;
		if (tmp)
		{
			pointer = tmp;
			clear();
		}
	}

	///Returns the latest type from stack
	TypeOfQuantum getLastType()
	{
		if (!pointer)
			return nothing;
		return pointer->type;
	}

	///If next to last element if Function, returns true, otherwise false
	bool isFuncPre()
	{
		if (!pointer)
			return false;
		NodeOfHistoryStack* tmp = pointer->prev;
		if (tmp)
			if (tmp->type == func)
				return true;
		return false;
	}

	///Compares the number of given parameters to necessary one 
	void check()
	{
		if (!pointer)
			int_obj->throw_(you_are_fool);
		if (pointer->type != func)
			int_obj->throw_(you_are_fool);
	}

	///Increases the number of arguments by one
	void incLastNumberOfArgumets()
	{
		pointer->number_of_argumets++;
	}

	short getNumOfArguments()
	{
		return pointer->number_of_argumets;
	}

	EvalNode* getNode()
	{
		return pointer->place;
	}

	bool isUnchang()
	{
		return pointer->const_mod;
	}

	string* getName()
	{
		return pointer->name;
	}

	///If the last element of stack is not Value - expression is invalid
	bool isCorrect()
	{
		if (!pointer)
			return false;
		if (pointer->type != val)
			return false;
		if (pointer->prev)
			return false;
		return true;
	}

	~HistoryStack()
	{
		if (pointer)
			clear();
	}
};

///Node of Poland stack
class Interpreter::NodeOfPolStack
{
	Interpreter::NodeOfPolStack *prev;
	Quantum *obj;
public:
	NodeOfPolStack(Quantum* _obj, Interpreter::NodeOfPolStack* pointer = NULL) : prev(pointer), obj(_obj)
	{

	}
	friend class Interpreter::PolStack;

};

class Interpreter::PolStack
{
	Interpreter::NodeOfPolStack *pointer;
	///Used for dealing with memory leaks
	Interpreter *int_obj;

	///Pops the most priority operands from stack
	void del_more_priority(Quantum* obj, EvalList *list)
	{
		for (; pointer;)
			if (pointer->obj->isOpenedBracket())
				return;
			else
				if (pointer->obj->getPriority() >= obj->getPriority())
					list->add(pop());
				else
					return;
	}
public:
	PolStack(Interpreter *obj_) : pointer(NULL), int_obj(obj_)
	{

	}

	///Overriden insert
	void push(Quantum* obj, EvalList *list)
	{
		switch ((*(obj->GetName()))[0])
		{
		case '(':
			break;
		default:
			del_more_priority(obj, list);
			break;
		}

		if (pointer)
			pointer = new Interpreter::NodeOfPolStack(obj, pointer);
		else
			pointer = new Interpreter::NodeOfPolStack(obj);
	}

	///Overriden insert
	void push(const char c, EvalList *list)
	{
		switch (c)
		{
		case ')':
			for (; ;)
			{
				if (!pointer)
					int_obj->throw_(need_bracket);
				if ((*(pointer->obj->GetName())) == (*(Drobot::ope_br->GetName())))
					break;
				list->add(pop());
			}
			pop();
			return;
			break;
		case ',':
			for (;;)
			{
				if (!pointer)
					int_obj->throw_(need_bracket);
				if ((*(pointer->obj->GetName())) == (*(Drobot::ope_br->GetName())))
					break;
				list->add(pop());
			}
			return;
			break;
		}
	}

	Quantum* pop()
	{
		if (!pointer)
			int_obj->throw_(invalid_expression);
		Interpreter::NodeOfPolStack* tmp = pointer->prev;
		Quantum* temp = pointer->obj;
		delete pointer;
		pointer = tmp;
		return temp;
	}

	void clear()
	{
		Interpreter::NodeOfPolStack* tmp;
		tmp = pointer->prev;
		if (pointer->obj->isUnchengable())
			delete pointer->obj;
		delete pointer;
		if (tmp)
		{
			pointer = tmp;
			clear();
		}
	}

	bool isEmpty()
	{
		if (!pointer)
			return true;
		return false;
	}

	///If the last element is an opened bracket
	bool isOpenedBr()
	{
		return (pointer->obj->isOpenedBracket());
	}

	~PolStack()
	{
		if (pointer)
			clear();
	}
};

Interpreter::Interpreter(string *s_, int *iter_, ErrStruct *p_, vector < string > *v_) : s(s_), iter(iter_), p(p_), v(v_)
{

}
void Interpreter::setVector(vector < string >* v_)
{
	v = v_;
}
///Maintains memory leaks if exception occurs
void Interpreter::throw_(ErrCode c)
{
	if (p)
	{
		p->_code = c;
		throw p;
	}
	throw c;
}
void Interpreter::getName(string *name)
{
	for (; *iter < s->size(); (*iter)++)
	{
		char temp = (*s)[*iter];
		if (isEmptySimbol(temp))
			return;
		if (temp == '(' || temp == ')' || temp == ',')
			return;
		if (temp == '=' || temp == '$')
			return;
		bool f = true;
		for (int i = 0; i < strlen(ar_operators) && f; ++i)
			if (temp == ar_operators[i])
				f = false;
		if (!f)
			return;
		name->push_back(temp);
	}
}
bool Interpreter::isConstName()
{
	int it = *iter;
	goToNextSymbol();
	if (!(it < s->size()))
		return true;
	if ((*s)[it] != '(')
		return true;
	it++;
	goToNextSymbol();
	if (!(it < s->size()))
		return false;
	if ((*s)[it] == ')')
		{
			it++;
			*iter = it;
			return true;
		}
	else
		return false;
}
/**Extracts a number from the string
	\param s		- source
	\param value	- pointer to the target location for the number
	\param iter		- current cursor position if the source
	\param p		- service use, deals with memory leaks
	*/
void Interpreter::getValue(TInternalData *value_)
{
	float st = 10;
	//value->setValue(0);
	long double ld;
	long double* value = &ld;
	*value = 0;
	long double osn = 1;
	for (; *iter < s->size(); (*iter)++)
	{
		char temp = (*s)[*iter];
		if (temp == '.')
		{
			//If the secong comma occurs, the number is invalid
			if (st == 0.1)
				throw_(invalid_value);
			st = 0.1;
			osn = 1;
		}
		else
		{
			if (temp <= '9' && temp >= '0')
				if (st == 10)
					(*value) = (*value) * st + (temp - '0');
				else
					(*value) = (*value) + (temp - '0') * osn;
			else
			{
				value_->setValue(ld);
				return;
			}
		}
		osn *= st;
	}
	value_->setValue(ld);
}
void Interpreter::goToNextSymbol()
{
	for (; *iter < s->size(); ++(*iter))
		if (!isEmptySimbol((*s)[*iter]))
			return;
}
bool Interpreter::isConstMod()
{
	goToNextSymbol();
	if (!(*iter < s->size()))
		return false;
	if ((*s)[*iter] != '$')
		return false;
	++(*iter);
	return true;
}
bool Interpreter::find(string *s)
{
	return ::find(v, s);
}
bool Interpreter::find(Quantum* q)
{
	TKeyData* s = q->GetName();
	s->pop_back();
	s->pop_back();
	bool f = find(s);
	delete s;
	return f;
}
void Interpreter::makePolandList(EvalList *list)
{
	bool is_function = (v) ? true : false;

	goToNextSymbol();
	if (*iter >= s->size())
		throw_(invalid_expression);

	Interpreter::HistoryStack h_stack(this);
	Interpreter::PolStack p_stack(this);
	for (; (*iter) < s->size(); goToNextSymbol())
	{
		goToNextSymbol();
		if (!(*iter < s->size()))
			break;
		switch (h_stack.getLastType())
		{
		case val:
			switch ((*s)[*iter])
			{
			case '(':
			{
				{
					string* pr = new string("*");
					p_stack.push(Drobot::global_tree.find(pr), list);
					delete pr;
					p_stack.push(Drobot::ope_br, list);

					h_stack.pop();
					h_stack.push(bin_op);
					h_stack.incLastNumberOfArgumets();
					h_stack.push(op_br);
					++(*iter);
					continue;
				}
			}
			case ')':
				p_stack.push(')', list);

				h_stack.pop();
				if (h_stack.getLastType() != op_br)
					throw_(invalid_expression);

				h_stack.pop();

				switch (h_stack.getLastType())
				{
				case bin_op:
					h_stack.pop();
					h_stack.push(val);
					++(*iter);
					continue;
				case nothing:
					h_stack.push(val);
					++(*iter);
					continue;
				case val:
					throw(you_are_fool, p);
					continue;
				case func:
				{
					h_stack.check();
					h_stack.incLastNumberOfArgumets();
					short num = h_stack.getNumOfArguments();
					Quantum* tmp = Drobot::global_tree.find(h_stack.getName(), num);
					if (!tmp)
					{
						if (num == 1)
						{
							if (find(h_stack.getName()) && !h_stack.isUnchang())
							{
								list->add(new Variable(h_stack.getName()), h_stack.getNode());
								string* pr = new string("*");
								p_stack.push(Drobot::global_tree.find(pr), list);
								delete pr;
							}
							else
							{
								tmp = Drobot::global_tree.find(h_stack.getName(), 0);
								if (!tmp)
									throw_(unknown_name);
								if (h_stack.isUnchang())
									tmp = new Const(dynamic_cast<Const*>(tmp));
								list->add(tmp, h_stack.getNode());
								string* pr = new string("*");
								p_stack.push(Drobot::global_tree.find(pr), list);
								delete pr;
							}
						}
						else
							throw_(unknown_name);
					}
					else
					{
						if (h_stack.isUnchang())
							tmp = new Function(dynamic_cast<Function*>(tmp));
						p_stack.push(tmp, list);
					}
					h_stack.pop();
					if (h_stack.getLastType() == bin_op)
						h_stack.pop();
					h_stack.push(val);
					++(*iter);
					continue;
				}
				case op_br:
					h_stack.push(val);
					++(*iter);
					continue;
				}

				continue;
			case ',':
				p_stack.push(',', list);

				h_stack.pop();
				if (h_stack.getLastType() != op_br)
					throw_(invalid_expression);

				if (!h_stack.isFuncPre())
					throw_(invalid_expression);

				h_stack.pop();
				h_stack.incLastNumberOfArgumets();
				h_stack.push(op_br);
				++(*iter);
				continue;
			case '$':
				throw_(bad_position_of_reserved_symbol);
				continue;
			default:
			{
				char c = (*s)[*iter];
				if (c <= '9' && c >= '0')
				{
					TInternalData* tmp = new TInternalData(0.0);
					getValue(tmp);
					string* pr = new string("*");
					p_stack.push(Drobot::global_tree.find(pr), list);

					list->add(new Value(tmp));

					delete pr;
					continue;
				}

				bool found_in_ar_operators = false;
				for (int i = 0; i < strlen(ar_operators) && !found_in_ar_operators; ++i)
				{
					if (c == ar_operators[i])
					{
						found_in_ar_operators = true;
						string* st = new string;
						st->push_back(ar_operators[i]);
						p_stack.push(Drobot::global_tree.find(st), list);
						delete st;

						h_stack.pop();
						h_stack.push(bin_op);
						h_stack.incLastNumberOfArgumets();
						++(*iter);
						continue;
					}
				}

				if (!found_in_ar_operators)
				{
					string* st = new string;
					getName(st);

					bool uncheng = isConstMod();
					goToNextSymbol();

					string* pr = new string("*");
					p_stack.push(Drobot::global_tree.find(pr), list);
					delete pr;
					if (!(*iter < s->size()) || isConstName())
					{
						if (find(st))
						{
							if (uncheng)
							{
								Quantum* f = Drobot::global_tree.find(st, 0);
								if (!f)
								{
									delete st;
									throw_(invalid_expression);
								}
								else
								{
									delete st;
									list->add(new Const(dynamic_cast<Const*>(f)));
								}
							}
							else
							{
								list->add(new Variable(st));
								delete st;
							}
						}
						else
						{
							Quantum* f = Drobot::global_tree.find(st, 0);
							if (uncheng)
							{
								delete st;
								list->add(new Const(dynamic_cast<Const*>(f)));
							}
							else
							{
								delete st;
								list->add(f);
							}
						}
					}
					else
					{
						h_stack.pop();
						h_stack.push(bin_op);
						h_stack.push(func, st, list->_tail, uncheng);

						p_stack.push(Drobot::ope_br, list);
						h_stack.push(op_br);
						++(*iter);
					}
				}
				continue;
			}
			}
			continue;
		case bin_op:
			switch ((*s)[*iter])
			{
			case '(':
				p_stack.push(Drobot::ope_br, list);
				h_stack.push(op_br);
				++(*iter);
				continue;
			case ')':
				throw_(invalid_expression);
				continue;
			case ',':
				throw_(invalid_expression);
				continue;
			case '$':
				throw_(bad_position_of_reserved_symbol);
				continue;
			default:
			{
				char c = (*s)[*iter];

				if (c <= '9' && c >= '0')
				{
					TInternalData* tmp = new TInternalData(0.0);
					getValue(tmp);
					list->add(new Value(tmp));
					h_stack.pop();
					h_stack.push(val);
					continue;
				}

				for (int i = 0; i < strlen(ar_operators); ++i)
					if (c == ar_operators[i])
						throw_(invalid_expression);

				string* st = new string;
				getName(st);

				bool uncheng = isConstMod();
				goToNextSymbol();

				if (!(*iter < s->size()) || isConstName())
				{
					h_stack.pop();
					h_stack.push(val);
					if (find(st))
					{
						if (uncheng)
						{
							Quantum* f = Drobot::global_tree.find(st, 0);
							if (!f)
							{
								delete st;
								throw_(invalid_expression);
							}
							else
							{
								delete st;
								list->add(new Const(dynamic_cast<Const*>(f)));
							}
						}
						else
						{
							list->add(new Variable(st));
							delete st;
						}
					}
					else
					{
						Quantum* f = Drobot::global_tree.find(st, 0);
						if (uncheng)
						{
							delete st;
							list->add(new Const(dynamic_cast<Const*>(f)));
						}
						else
						{
							delete st;
							list->add(f);
						}
					}
				}
				else
				{
					h_stack.push(func, st, list->_tail, uncheng);

					p_stack.push(Drobot::ope_br, list);
					h_stack.push(op_br);
					++(*iter);
				}
			}
			}
			continue;
		case nothing:
			switch ((*s)[*iter])
			{
			case '(':
				p_stack.push(Drobot::ope_br, list);
				h_stack.push(op_br);
				++(*iter);
				continue;
			case ')':
				throw_(invalid_bracket_position);
				continue;
			case ',':
				throw_(invalid_expression);
				continue;
			case '$':
				throw_(bad_position_of_reserved_symbol);
				continue;
			default:
			{
				char c = (*s)[*iter];

				if (c <= '9' && c >= '0')
				{
					TInternalData* tmp = new TInternalData(0.0);
					getValue(tmp);
					list->add(new Value(tmp));

					h_stack.push(val);
					continue;
				}

				if (c == '-')
				{
					string* pr = new string("*");
					TInternalData* ch = new TInternalData(-1);
					list->add(new Value(ch));
					p_stack.push(Drobot::global_tree.find(pr), list);
					h_stack.push(bin_op);
					delete pr;
					++(*iter);
					continue;
				}

				if (c == '+')
				{
					++(*iter);
					continue;
				}

				for (int i = 0; i < strlen(ar_operators); ++i)
					if (c == ar_operators[i])
						throw_(invalid_expression);

				string* st = new string;
				getName(st);

				bool uncheng = isConstMod();
				goToNextSymbol();

				if (!(*iter < s->size()) || isConstName())
				{
					h_stack.push(val);
					if (find(st))
					{
						if (uncheng)
						{
							Quantum* f = Drobot::global_tree.find(st, 0);
							if (!f)
							{
								delete st;
								throw_(invalid_expression);
							}
							else
							{
								delete st;
								list->add(new Const(dynamic_cast<Const*>(f)));
							}
						}
						else
						{
							list->add(new Variable(st));
							delete st;
						}
					}
					else
					{
						Quantum* f = Drobot::global_tree.find(st, 0);
						if (uncheng)
						{
							delete st;
							list->add(new Const(dynamic_cast<Const*>(f)));
						}
						else
						{
							delete st;
							list->add(f);
						}
					}
				}
				else
				{
					h_stack.push(func, st, list->_tail, uncheng);

					p_stack.push(Drobot::ope_br, list);
					h_stack.push(op_br);
					++(*iter);
				}
			}
			}
			continue;
		case op_br:
			switch ((*s)[*iter])
			{
			case '(':
				p_stack.push(Drobot::ope_br, list);
				h_stack.push(op_br);
				(*iter)++;
				continue;
			case ')':
				p_stack.push(')', list);
				h_stack.pop();
				++(*iter);
				continue;
			case ',':
				throw_(invalid_expression);
				continue;
			case '$':
				throw_(bad_position_of_reserved_symbol);
				continue;
			default:
			{
				char c = (*s)[*iter];

				if (c <= '9' && c >= '0')
				{
					TInternalData* tmp = new TInternalData(0.0);
					getValue(tmp);
					list->add(new Value(tmp));

					h_stack.push(val);
					continue;
				}

				if (c == '-')
				{
					string* pr = new string("*");
					TInternalData* ch = new TInternalData(-1);
					list->add(new Value(ch));
					p_stack.push(Drobot::global_tree.find(pr), list);
					h_stack.push(bin_op);
					delete pr;
					++(*iter);
					continue;
				}

				if (c == '+')
				{
					++(*iter);
					continue;
				}

				for (int i = 0; i < strlen(ar_operators); ++i)
					if (c == ar_operators[i])
						throw_(invalid_expression);

				string* st = new string;
				getName(st);

				bool uncheng = isConstMod();
				goToNextSymbol();

				if (!(*iter < s->size()) || isConstName())
				{
					h_stack.push(val);
					if (find(st))
					{
						if (uncheng)
						{
							Quantum* f = Drobot::global_tree.find(st, 0);
							if (!f)
							{
								delete st;
								throw_(invalid_expression);
							}
							else
							{
								delete st;
								list->add(new Const(dynamic_cast<Const*>(f)));
							}
						}
						else
						{
							list->add(new Variable(st));
							delete st;
						}
					}
					else
					{
						Quantum* f = Drobot::global_tree.find(st, 0);
						if (uncheng)
						{
							delete st;
							list->add(new Const(dynamic_cast<Const*>(f)));
						}
						else
						{
							delete st;
							list->add(f);
						}
					}
				}
				else
				{
					h_stack.push(func, st, list->_tail, uncheng);

					p_stack.push(Drobot::ope_br, list);
					h_stack.push(op_br);
					++(*iter);
				}
				/*Quantum* f = Drobot::global_tree.find(st);
				if (!f)
				{
				if (!is_function)
				throw_(unknown_name, p);
				if (!find(v, st))
				throw_(unknown_name, p);
				list->add(new Variable(st));
				h_stack.push(val);

				delete st;
				continue;
				}

				if (f->GetType() == con)
				{
				if (isConstMod(s, iter))
				list->add(new Const(dynamic_cast<Const*>(f)));
				else
				list->add(f);
				delete st;
				h_stack.push(val);
				continue;
				}
				bool uncheng = isConstMod(s, iter);

				goToNextSymbol(s, iter);
				if (!(*iter < s->size()))
				throw_(wrong_number_of_argument, p);
				if ((*s)[*iter] != '(')
				throw_(invalid_operand, p);

				if (uncheng)
				p_stack.push(new Function(dynamic_cast<Function*>(f)), list);
				else
				p_stack.push(f, list);

				p_stack.push(Drobot::ope_br, list);
				h_stack.push(func, f);
				h_stack.push(op_br);
				(*iter)++;
				continue;*/
			}
			}
		}
	}

	if (!h_stack.isCorrect())
		throw_(invalid_expression);

	for (; !p_stack.isEmpty();)
		if (p_stack.isOpenedBr())
			throw_(invalid_expression);
		else
			list->add(p_stack.pop());
}
/**Calculates the expression
\param	s	 -	source
*/
TInternalData Interpreter::evaluateConstExpr()
{
	iter = new int(0);
	p = new ErrStruct;
	p->add(iter);
	EvalList pol_list;
	if (s->find('=') != -1)
		for (int i = s->size() - 1; i >= 0; --i)
			if ((*s)[i] == '=')
			{
				s->pop_back();
				break;
			}
			else
				s->pop_back();

	makePolandList(&pol_list);
	delete iter;
	delete p;
	pol_list.evaluate();

	if (!Stack::isNormalRezult())
		throw(invalid_expression);
	s->push_back(' ');
	s->push_back('=');
	s->push_back(' ');
	return Stack::pop();
}