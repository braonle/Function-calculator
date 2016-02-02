#include "Matrix.h"
#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>



using namespace MatrixNS;
using namespace DataNS;

Data::DataWrap::~DataWrap()
{
	if (!_tmpFlag)
		delete _ptr;
}
string Data::DataWrap::toString()
{
	stringstream ss;
	this->_ptr->output(ss);
	return ss.str();
}
bool Data::_eq(InsideType num, InsideType data)
{
	const long double e = 0.00000000000001;
	if (::abs(data - num) < e)
		return true;
	else
		return false;
}


DataCodes Data::_getCode(Data* code)
{
	if (NULL == code)
		return nullPtr;
	if (dynamic_cast<RealData*>(code))
		return rCode;
	if (dynamic_cast<ComplexData*>(code))
		return cCode;

	throw unknown_type;
}
void Data::_argConvert(Data** arg1, Data** arg2)
{
	long double *tmp;

	switch (_getCode(*arg1))
	{
	case rCode:
		switch (_getCode(*arg2))
		{
		case rCode:
			*arg1 = new RealData((RealData&)**arg1);
			break;

		case cCode:
			tmp = (long double*) (*arg2)->_getData();
			if (Data::_eq(tmp[1], 0))
			{
				*arg2 = new RealData(tmp[0]);
				*arg1 = new RealData((RealData&)**arg1);
			}
			else
				*arg1 = new ComplexData((RealData*)*arg1);
			delete[] tmp;
			break;

		default:
			throw no_type_impl;
			break;
		}
		break;

	case cCode:
		switch (_getCode(*arg2))
		{
		case rCode:

			tmp = (long double*)(*arg1)->_getData();
			if (Data::_eq(tmp[1], 0))
				*arg1 = new RealData((long double)tmp[0]);
			else
			{
				*arg2 = new ComplexData((RealData*)*arg2);
				*arg1 = new ComplexData((ComplexData&)**arg1);
			}
			delete[] tmp;
			break;

			break;

		case cCode:
			*arg1 = new ComplexData((ComplexData&)**arg1);
			break;

		default:
			throw no_type_impl;
			break;
		}
		break;

	default:
		throw no_type_impl;
		break;
	}
	if (*arg1 == NULL || *arg2 == NULL)
		throw no_mem;
}
Data::DataWrap::DataWrap(Data::DataWrap& src)
{
	if (src._ptr == nullptr)
		throw doubleTmpUsage;

	if (src._tmpFlag)
	{
		this->_ptr = src._ptr;
		src._ptr = nullptr;
	}
	else
		this->_ptr = src._ptr->clone();
	if (src._opFlag)
		_tmpFlag = true;
	else
		_tmpFlag = false;

	_opFlag = false;
}
Data::DataWrap Data::DataWrap::operator + (Data::DataWrap& param2)
{
	Data *arg1 = this->_ptr;
	Data *arg2 = param2._ptr;
	Data::_argConvert(&arg1, &arg2);

	ErrCodes err = arg1->add(arg2);
	if (err != correct)
		throw err;
	DataWrap wr(arg1);
	wr._tmpFlag = true;
	wr._opFlag = true;
	if (arg2 != param2._ptr)
		delete arg2;
	param2._tmpFlag = false;
	this->_tmpFlag = false;
	return wr;
}
Data::DataWrap Data::DataWrap::operator - (Data::DataWrap& deduction)
{
	Data *arg1 = this->_ptr;
	Data *arg2 = deduction._ptr;
	Data::_argConvert(&arg1, &arg2);

	ErrCodes err = arg1->substract(arg2);
	if (err != correct)
		throw err;
	DataWrap wr(arg1);
	wr._tmpFlag = true;
	wr._opFlag = true;
	if (arg2 != deduction._ptr)
		delete arg2;
	deduction._tmpFlag = false;
	this->_tmpFlag = false;
	return wr;
}
Data::DataWrap Data::DataWrap::operator * (Data::DataWrap& param2)
{
	Data *arg1 = this->_ptr;
	Data *arg2 = param2._ptr;
	Data::_argConvert(&arg1, &arg2);

	ErrCodes err = arg1->multiply(arg2);
	if (err != correct) throw err;
	DataWrap wr(arg1);
	wr._tmpFlag = true;
	wr._opFlag = true;
	if (arg2 != param2._ptr) delete arg2;
	param2._tmpFlag = false;
	this->_tmpFlag = false;
	return wr;
}
Data::DataWrap Data::DataWrap::operator / (Data::DataWrap& divisor)
{
	Data *arg1 = this->_ptr;
	Data *arg2 = divisor._ptr;
	Data::_argConvert(&arg1, &arg2);

	ErrCodes err = arg1->divide(arg2);
	if (err != correct)
		throw err;
	DataWrap wr(arg1);
	wr._tmpFlag = true;
	wr._opFlag = true;
	if (arg2 != divisor._ptr)
		delete arg2;
	divisor._tmpFlag = false;
	this->_tmpFlag = false;
	return wr;
}
Data::DataWrap& Data::DataWrap::operator += (Data::DataWrap &param) throw(ErrCodes)
{
	Data *arg1 = this->_ptr;
	Data *arg2 = param._ptr;
	Data::_argConvert(&arg1, &arg2);

	ErrCodes err = arg1->add(arg2);
	if (err != correct)
		throw err;

	if (this->_ptr != param._ptr)
	{
		delete this->_ptr;		
	}
	this->_ptr = arg1;
	this->_tmpFlag = false;
	if (arg2 != param._ptr)
		delete arg2;
	param._tmpFlag = false;
	return *this;
}
Data::DataWrap& Data::DataWrap::operator -= (Data::DataWrap &deduction) throw(ErrCodes)
{
	Data *arg1 = this->_ptr;
	Data *arg2 = deduction._ptr;
	Data::_argConvert(&arg1, &arg2);

	ErrCodes err = arg1->substract(arg2);
	if (err != correct)
		throw err;

	if (this->_ptr != deduction._ptr)
	{
		delete this->_ptr;
	}
	this->_ptr = arg1;
	this->_tmpFlag = false;
	if (arg2 != deduction._ptr)
		delete arg2;
	deduction._tmpFlag = false;
	return *this;
}
Data::DataWrap& Data::DataWrap::operator *= (Data::DataWrap &param) throw(ErrCodes)
{
	Data *arg1 = this->_ptr;
	Data *arg2 = param._ptr;
	Data::_argConvert(&arg1, &arg2);

	ErrCodes err = arg1->multiply(arg2);
	if (err != correct)
		throw err;

	if (this->_ptr != param._ptr)
	{
		delete this->_ptr;
	}
	this->_ptr = arg1;
	this->_tmpFlag = false;
	if (arg2 != param._ptr)
		delete arg2;
	param._tmpFlag = false;
	return *this;
}
Data::DataWrap& Data::DataWrap::operator /= (Data::DataWrap& divisor) throw(ErrCodes)
{
	Data *arg1 = this->_ptr;
	Data *arg2 = divisor._ptr;
	Data::_argConvert(&arg1, &arg2);

	ErrCodes err = arg1->divide(arg2);
	if (err != correct)
		throw err;

	if (this->_ptr != divisor._ptr)
	{
		delete this->_ptr;
	}
	this->_ptr = arg1;
	this->_tmpFlag = false;
	if (arg2 != divisor._ptr)
		delete arg2;
	divisor._tmpFlag = false;
	return *this;
}
Data::DataWrap& Data::DataWrap::operator = (Data::DataWrap& src) throw(ErrCodes)
{
	if (src._ptr == nullptr)
		throw doubleTmpUsage;

	if (this->_ptr != NULL) 
		delete this->_ptr;

	if (src._tmpFlag)
	{
		this->_ptr = src._ptr;
		src._ptr = nullptr;
	}
	else
		this->_ptr = src._ptr->clone();
	this->_tmpFlag = false;
	return *this;
}
Data::DataWrap DataNS::Data::DataWrap::conjugate()
{
	return Data::DataWrap::getInstance(_ptr->conjugate()).restrictDelete();
}
Data::DataWrap* Data::DataWrap::clone()
{
	return new Data::DataWrap(_ptr->clone());
}
Data::DataWrap& Data::DataWrap::set(Data* src)
{
	if (_ptr != nullptr)
		delete _ptr;
	_ptr = src;
	_tmpFlag = FALSE;
	return *this;
}
Data::DataWrap& Data::DataWrap::forceDelete()
{
	this->_tmpFlag = FALSE;
	return *this;
}
Data::DataWrap& Data::DataWrap::restrictDelete()
{
	this->_tmpFlag = TRUE;
	return *this;
}
bool Data::DataWrap::isZero()
{
	return _ptr->isZero();
}
void Data::DataWrap::output(ostream& ss)
{
	this->_ptr->output(ss);
}
Data::DataWrap Data::DataWrap::getInstance(Data* src)
{
	Data::DataWrap ret(src);
	ret.restrictDelete();
	ret._opFlag = true;
	return ret;
}
InsideType DataNS::Data::DataWrap::abs()
{
	return this->_ptr->abs();
}
Data::DataWrap DataNS::Data::DataWrap::cos()
{
	return Data::DataWrap(this->_ptr->cos()).restrictDelete();
}
Data::DataWrap DataNS::Data::DataWrap::sin()
{
	return Data::DataWrap(this->_ptr->sin()).restrictDelete();
}
Data::DataWrap DataNS::Data::DataWrap::exp()
{
	return Data::DataWrap(this->_ptr->exp()).restrictDelete();
}
Data::DataWrap DataNS::Data::DataWrap::sqr()
{
	return Data::DataWrap(this->_ptr->sqr()).restrictDelete();
}
Data::DataWrap DataNS::Data::DataWrap::sqrt()
{
	return Data::DataWrap(this->_ptr->sqrt()).restrictDelete();
}
Data::DataWrap DataNS::Data::DataWrap::tan()
{
	return Data::DataWrap(this->_ptr->tan()).restrictDelete();
}
Data::DataWrap DataNS::Data::DataWrap::sinh()
{
	return Data::DataWrap(this->_ptr->sinh()).restrictDelete();
}
Data::DataWrap DataNS::Data::DataWrap::cosh()
{
	return Data::DataWrap(this->_ptr->cosh()).restrictDelete();
}
Data::DataWrap DataNS::Data::DataWrap::tanh()
{
	return Data::DataWrap(this->_ptr->tanh()).restrictDelete();
}
Data::DataWrap DataNS::Data::DataWrap::log()
{
	return Data::DataWrap(this->_ptr->log()).restrictDelete();
}
Data::DataWrap DataNS::Data::DataWrap::pow(Data::DataWrap& param)
{
	return (this->log() * param).exp().restrictDelete();
}
Data* DataNS::Data::DataWrap::getContent()
{
	return _ptr;
}
ostream& DataNS::operator << (ostream& out, Data::DataWrap& src)
{
	src.output(out);
	return out;
}


RealData::RealData(InsideType data)
{
	this->_data = data;
}
ErrCodes RealData::add(Data *dt)
{
	RealData *ptr;
	if (!(ptr = dynamic_cast<RealData*>(dt)))
		return bad_ptr;
	_data += ptr->_data;
	return correct;
}
ErrCodes RealData::substract(Data *dt)
{
	RealData *ptr;
	if (!(ptr = dynamic_cast<RealData*>(dt)))
		return bad_ptr;
	_data -= ptr->_data;
	return correct;
}
ErrCodes RealData::multiply(Data *dt)
{
	RealData *ptr;
	if (!(ptr = dynamic_cast<RealData*>(dt)))
		return bad_ptr;
	_data *= ptr->_data;
	return correct;
}
ErrCodes RealData::divide(Data *dt)
{
	RealData *ptr;
	if (!(ptr = dynamic_cast<RealData*>(dt)))
		return bad_ptr;
	if (ptr->_eq(0, ptr->_data))
		return zero_div;
	_data /= ptr->_data;
	return correct;
}
Data * DataNS::RealData::sin()
{
	return new DataNS::RealData(std::sin(this->_data));
}
Data * DataNS::RealData::cos()
{
	return new DataNS::RealData(std::cos(this->_data));
}
Data * DataNS::RealData::tan()
{
	return new DataNS::RealData(std::tan(this->_data));
}
Data * DataNS::RealData::sqr()
{
	return new DataNS::RealData((this->_data) * this->_data);
}
Data * DataNS::RealData::sqrt()
{
	if (_data >= 0)
	{
		return new DataNS::RealData(std::sqrt(this->_data));
	}
	else
	{
		return new DataNS::ComplexData(0, std::sqrt(_data));
	}
}
Data * DataNS::RealData::exp()
{
	return new DataNS::RealData(std::exp(this->_data));
}
Data * DataNS::RealData::sinh()
{
	return new DataNS::RealData((std::exp(_data) - std::exp(_data)) / 2);
}
Data * DataNS::RealData::cosh()
{
	return new DataNS::RealData((std::exp(_data) + std::exp(_data)) / 2);
}
Data * DataNS::RealData::tanh()
{
	return new DataNS::RealData((std::exp(_data) + std::exp(_data)) / (std::exp(_data) - std::exp(_data)));
}
Data * DataNS::RealData::log()
{
	if (_eq(_data, 0))
	{
		throw zero_pow;
	}
	else if (_data > 0)
	{
		return new RealData(std::log(_data));
	}
	else
	{
		return new ComplexData(std::log(std::abs(_data)), M_PI);
	}
}
void RealData::output(ostream& ss)
{
	ss << _data;
}
Data * DataNS::RealData::conjugate()
{
	return new DataNS::RealData(*this);
}
InsideType RealData::abs(void)
{
	return ::abs(_data);
}
void* RealData::_getData()
{
	return new long double(this->_data);
}
bool DataNS::RealData::isZero()
{
	return _eq(0, _data);
}
Data * DataNS::RealData::clone()
{
	return new DataNS::RealData(*this);
}
RealData::~RealData() {}


ComplexData::ComplexData(InsideType re, InsideType im)
{
	this->_re = re;
	this->_im = im;
}
ComplexData::ComplexData(RealData* src)
{
	long double *ptr = (long double*) src->_getData();
	this->_re = *ptr;
	this->_im = 0;
	delete ptr;
}
ErrCodes ComplexData::add(Data * dt)
{
	ComplexData *ptr;
	if (!(ptr = dynamic_cast<ComplexData*>(dt)))
		return bad_ptr;
	this->_re += ptr->_re;
	this->_im += ptr->_im;
	return correct;
}
ErrCodes ComplexData::substract(Data * dt)
{
	ComplexData *ptr;
	if (!(ptr = dynamic_cast<ComplexData*>(dt)))
		return bad_ptr;
	this->_re -= ptr->_re;
	this->_im -= ptr->_im;
	return correct;
}
ErrCodes ComplexData::multiply(Data * dt)
{
	ComplexData *ptr;
	if (!(ptr = dynamic_cast<ComplexData*>(dt)))
		return bad_ptr;

	long double tmp = this->_re * ptr->_re - this->_im * ptr->_im;
	this->_im = this->_re * ptr->_im + this->_im * ptr->_re;
	this->_re = tmp;
	return correct;
}
ErrCodes ComplexData::divide(Data * dt)
{
	ComplexData *ptr;
	if (!(ptr = dynamic_cast<ComplexData*>(dt)))
		return bad_ptr;

	if (_eq(ptr->_re, 0) && _eq(ptr->_im, 0))
		return zero_div;
	
	ptr = (ComplexData*)ptr->conjugate();
	this->multiply(ptr);
	long double divisor = ptr->_re * ptr->_re + ptr->_im * ptr->_im;
	delete ptr;
	this->_re /= divisor;
	this->_im /= divisor;

	return correct;
}
Data * DataNS::ComplexData::sin()
{
	return new DataNS::ComplexData(std::sin(_re) * std::cosh(_im), std::sinh(_im) * std::cos(_re));
}
Data * DataNS::ComplexData::cos()
{
	return new DataNS::ComplexData(std::cos(_re) * std::cosh(_im), (-1) * std::sinh(_im) * std::sin(_re));;
}
Data * DataNS::ComplexData::tan()
{
	Data * ptr = this->cos();
	Data * ret = this->sin();
	ErrCodes err = ret->divide(ptr);
	delete ptr;
	if (err != correct)
	{
		delete ptr;
		throw err;
	}
	return ret;
}
Data * DataNS::ComplexData::sqr()
{
	return new DataNS::ComplexData(_re * _re - _im * _im, 2 * _re * _im);
}
Data * DataNS::ComplexData::sqrt()
{
	InsideType module = std::sqrt(std::sqrt(_re * _re + _im * _im));
	if (!Data::_eq(_re, 0))
	{
		InsideType angle = std::atan(_im / _re) * 0.5;
		return new DataNS::ComplexData(module * std::cos(angle), module * std::sin(angle));
	}
	else if (_im > 0)
	{
		return new DataNS::ComplexData(module * std::sqrt(2) / 2, module * std::sqrt(2) / 2);
	}
	else
	{
		return new DataNS::ComplexData((-1) * module * std::sqrt(2) / 2, module * std::sqrt(2) / 2);
	}
}
Data * DataNS::ComplexData::exp()
{
	InsideType module = std::exp(_im);
	return new DataNS::ComplexData(module * std::cos(_re), module * std::sin(_re));
}
Data * DataNS::ComplexData::cosh()
{
	return new DataNS::ComplexData(std::cos(_im) * std::cosh(_re), std::sin(_im) * std::sinh(_re));
}
Data * DataNS::ComplexData::sinh()
{
	return new DataNS::ComplexData(std::cos(_im) * std::sinh(_re), std::sin(_im) * std::cosh(_re));
}
Data * DataNS::ComplexData::tanh()
{
	Data * ptr = this->cosh();
	Data * ret = this->sinh();
	ErrCodes err = ret->divide(ptr);
	delete ptr;
	if (err != correct)
	{
		delete ptr;
		throw err;
	}
	return ret;
}
Data * DataNS::ComplexData::log()
{
	if (_eq(_re, 0) && _eq(_im, 0))
	{
		throw zero_pow;
	}
	else if (_eq(_im, 0))
	{
		return new RealData(_re);
	}
	else if (!_eq(_re, 0))
	{
		return new ComplexData(0.5 * std::log(_re*_re + _im*_im), std::atan(_im / _re));
	}
	else if (_im > 0)
	{
		return new ComplexData(std::log(_im), M_PI_2);
	}
	else
	{
		return new ComplexData(std::log(std::abs(_im)), 3 * M_PI_2);
	}
}
Data* ComplexData::conjugate()
{
	return new ComplexData(this->_re, -(this->_im));
}
void ComplexData::output(ostream& ss)
{
	
	if (_eq(_im, 0))
		ss << _re;
	else if (_eq(_re, 0))
	{
		if (!_eq(1, ::abs(_im)))
			ss << _im << "i";
		else
			if (_eq(_im, 1))
				ss << "i";
			else
				ss << "-i";
	}
	else
	{
		ss << _re;
		if (_im > 0)
			ss << "+";
		if (!_eq(1, ::abs(_im)))
			ss << _im << "i";
		else
			if (_eq(_im, 1))
				ss << "i";
			else
				ss << "-i";
	}
}
InsideType ComplexData::abs(void)
{
	return std::sqrt(_re*_re + _im*_im);
}
void* ComplexData::_getData()
{
	long double *ptr = new long double[2];
	ptr[0] = this->_re;
	ptr[1] = this->_im;	
	return ptr;
}
bool ComplexData::isZero()
{
	return _eq(0, this->abs());
}
ComplexData::~ComplexData() {}
Data * ComplexData::clone()
{
	return new DataNS::ComplexData(*this);
}

Matrix::Matrix(Data::DataWrap ** input, int width, int height)
{
	this->_table = input;
	this->_width = width;
	this->_height = height;
}
Matrix::Matrix(Matrix & m)
{
	_width = m._width;
	_height = m._height;
	_table = new DataNS::Data::DataWrap*[_height];
	for (int i = 0; i < _height; i++)
	{
		_table[i] = new DataNS::Data::DataWrap[_height];
		for (int j = 0; j < _width; j++)
		{
			_table[i][j] = m._table[i][j];
		}
	}
}
Matrix::~Matrix()
{
	for (int i = 0; i < _height; i++)
	{
		delete[] _table[i];
	}
	delete[] _table;
}
Matrix& Matrix::add(Matrix & input) throw(ErrCodes)
{
	const int tNumToStart = 40;

	if (_width != input._width || _height != input._height) throw mMismatch;
	if (_table == nullptr || input._table == nullptr) throw mEmpty;
	
	if (_height < tNumToStart) _simpleAdd(input);
	else
	{
		ThreadInfo* info;
		HANDLE threads[_threadNumber];
		for (int i = 0; i < _threadNumber; i++)
		{
			info = new ThreadInfo(this, &input, i);
			threads[i] = CreateThread(NULL, 0, _threadAdd, info, 0, NULL);
		}
		WaitForMultipleObjects(_threadNumber, threads, true, INFINITE);
		for (int i = 0; i < _threadNumber; i++) CloseHandle(threads[i]);
	} 
	return *this;
}
void Matrix::_simpleAdd(Matrix& input)
{
	for (int i = 0; i < _height; i++)
		for (int j = 0; j < _width; j++)
		{
			this->_table[i][j] += input._table[i][j];
		}
	return;
}
DWORD Matrix::_threadAdd(LPVOID data)
{
	ThreadInfo* dt = (ThreadInfo*)data;

	for (int i = dt->seq; i < dt->target->_height; i += _threadNumber)
		for (int j = 0; j < dt->target->_width; j++)
		{
			dt->target->_table[i][j] += dt->src->_table[i][j];
		}
	dt->target = dt->src = nullptr;
	delete dt;
	return 0;
}
Matrix& Matrix::substract(Matrix & input) throw(ErrCodes)
{
	const int tNumToStart = 40;

	if (_width != input._width || _height != input._height) throw mMismatch;
	if (_table == nullptr || input._table == nullptr) throw mEmpty;

	if (_height < tNumToStart) _simpleSubstract(input);
	else
	{
		ThreadInfo* info;
		HANDLE threads[_threadNumber];
		for (int i = 0; i < _threadNumber; i++)
		{
			info = new ThreadInfo(this, &input, i);
			threads[i] = CreateThread(NULL, 0, _threadSubstract, info, 0, NULL);
		}
		WaitForMultipleObjects(_threadNumber, threads, true, INFINITE);
		for (int i = 0; i < _threadNumber; i++) CloseHandle(threads[i]);
	}
	return *this;
}
void Matrix::_simpleSubstract(Matrix& input)
{
	for (int i = 0; i < _height; i++)
		for (int j = 0; j < _width; j++)
		{
			this->_table[i][j] -= input._table[i][j];
		}
	return;
}
DWORD Matrix::_threadSubstract(LPVOID data)
{
	ThreadInfo* dt = (ThreadInfo*)data;

	for (int i = dt->seq; i < dt->target->_height; i += _threadNumber)
		for (int j = 0; j < dt->target->_width; j++)
		{
			dt->target->_table[i][j] -= dt->src->_table[i][j];
		}
	dt->target = dt->src = nullptr;
	delete dt;
	return 0;
}
Matrix& Matrix::multiply(Data::DataWrap& input)
{
	for (int i = 0; i < _height; i++)
		for (int j = 0; j < _width; j++)
			_table[i][j] *= input;
		
	return *this;
}
Matrix* Matrix::conjugate()
{
	Data::DataWrap **arr = new Data::DataWrap*[_width];

	for (int i = 0; i < _width; i++)
	{
		arr[i] = new Data::DataWrap[_height];
		for (int j = 0; j < _height; j++)
		{
			arr[i][j] = _table[j][i].conjugate().restrictDelete();
		}
	}
	return new Matrix(arr, _height, _width);
}
Matrix& Matrix::multiply(Data::DataWrap* input)
{
	multiply(*input);
	delete input;
	return *this;
}
Matrix& MatrixNS::Matrix::multiplyL(Matrix & input) throw(ErrCodes)
{
	const int tNumToStart = 20;
	if (input._width != _height) throw mMismatch;
	if (_table == nullptr || input._table == nullptr) throw mEmpty;

	if (_height < tNumToStart) _simpleLMultiply(input);
	else
	{
		ThreadInfo* info;
		HANDLE threads[_threadNumber];

		Data::DataWrap** arr = new Data::DataWrap*[input._height];
		for (int i = 0; i < input._height; i++)
			arr[i] = new Data::DataWrap[_width];
		
		for (int i = 0; i < _threadNumber; i++)
		{
			info = new ThreadInfo(&input, this, i, &arr);
			threads[i] = CreateThread(NULL, 0, _threadMultiply, info, 0, NULL);
		}
		WaitForMultipleObjects(_threadNumber, threads, true, INFINITE);
		for (int i = 0; i < _threadNumber; i++) 
			CloseHandle(threads[i]);
		for (int i = 0; i < _height; i++)
			delete[] _table[i];
		delete[] _table;
		_table = arr;
		_height = input._height;
	}
	return *this;
}
Matrix& MatrixNS::Matrix::multiplyR(Matrix & input) throw(ErrCodes)
{
	const int tNumToStart = 20;
	if (_width != input._height) throw mMismatch;
	if (_table == nullptr || input._table == nullptr) throw mEmpty;

	if (_height < tNumToStart) _simpleRMultiply(input);
	else
	{
		ThreadInfo* info;
		HANDLE threads[_threadNumber];

		Data::DataWrap** arr = new Data::DataWrap*[_height];
		for (int i = 0; i < _height; i++)
			arr[i] = new Data::DataWrap[input._width];

		for (int i = 0; i < _threadNumber; i++)
		{
			info = new ThreadInfo(this, &input, i, &arr);
			threads[i] = CreateThread(NULL, 0, _threadMultiply, info, 0, NULL);
		}
		WaitForMultipleObjects(_threadNumber, threads, true, INFINITE);
		for (int i = 0; i < _threadNumber; i++) 
			CloseHandle(threads[i]);
		for (int i = 0; i < _height; i++)
			delete[] _table[i];
		delete[] _table;
		_table = arr;
		_width = input._width;
	}
	return *this;
}
void MatrixNS::Matrix::_simpleLMultiply(Matrix & input) throw(ErrCodes)
{
	Data::DataWrap** arr = new Data::DataWrap*[input._height];

	for (int i = 0; i < input._height; i++)
	{
		arr[i] = new Data::DataWrap[_width];
		for (int j = 0; j < _width; j++)
		{
			arr[i][j].set(new RealData(0));
			for (int k = 0; k < _height; k++)
				arr[i][j] += (input._table[i][k] * _table[k][j]);
		}
	}
	for (int i = 0; i < _height; i++)
		delete[] _table[i];
	delete[] _table;
	_table = arr;
	_height = input._height;
}
void MatrixNS::Matrix::_simpleRMultiply(Matrix & input) throw(ErrCodes)
{
	Data::DataWrap** arr = new Data::DataWrap*[_height];
		
	for (int i = 0; i < _height; i++)
	{
		arr[i] = new Data::DataWrap[input._width];
		for (int j = 0; j < input._width; j++)
		{
			arr[i][j].set(new RealData(0));
			for (int k = 0; k < _width; k++)
				arr[i][j] += (_table[i][k] * input._table[k][j]);
		}
	}
	for (int i = 0; i < _height; i++)
		delete[] _table[i];
	delete[] _table;
	_table = arr;
	_width = input._width;
}
DWORD MatrixNS::Matrix::_threadMultiply(LPVOID data)
{
	Matrix *lm = static_cast<ThreadInfo*>(data)->target,
		*rm = static_cast<ThreadInfo*>(data)->src;
	int seq = static_cast<ThreadInfo*>(data)->seq;
	Data::DataWrap** arr = *static_cast<ThreadInfo*>(data)->src1;

	for (int i = seq; i < lm->_height; i += _threadNumber)
	{
		for (int j = 0; j < rm->_width; j++)
		{
			arr[i][j].set(new RealData(0));
			for (int k = 0; k < lm->_width; k++)
				arr[i][j] += lm->_table[i][k] * rm->_table[k][j];
		}
	}

	static_cast<ThreadInfo*>(data)->target = nullptr;
	static_cast<ThreadInfo*>(data)->src = nullptr;
	static_cast<ThreadInfo*>(data)->src1 = nullptr;
	delete data;

	return 0;
}
int Matrix::isSymmetric()
{
	if (!isSquare()) return -1;

	for (int i = 0; i < _height; i++)
		for (int j = i + 1; j < _width; j++)
			if (!(_table[i][j] - _table[j][i]).forceDelete().isZero())
				return 1;

	return 0;
}
int MatrixNS::Matrix::isUnitary()
{
	if (!isSquare()) return -1;

	Matrix *a = this->conjugate(), *b = this->inverse();
	bool res = a->equals(*b);
	delete a;
	delete b;
	return res;
}
Data::DataWrap** Matrix::_diagUp(bool* sign)
{
	Data::DataWrap **arr = new Data::DataWrap*[_height];

	for (int i = 0; i < _height; i++)
	{
		arr[i] = new Data::DataWrap[_width];
		for (int j = 0; j < _width; j++)
			arr[i][j] = _table[i][j];
	}

	*sign = true;
	for (int i = 0; i < _width; i++)
	{
		bool zFlag = true;
		for (int j = i; j < _height && zFlag; j++)
			if (!arr[j][i].isZero())
			{
				zFlag = false;
				if (i != j)
				{
					Data::DataWrap *tmp = arr[i];
					arr[i] = arr[j];
					arr[j] = tmp;
					*sign = !*sign;
				}
			}
		if (zFlag)
		{
			for (int j = 0; j < _width; j++)
				delete[] arr[j];
			delete arr;
			return nullptr;
		}
		for (int j = i + 1; j < _height; j++)
		{
			for (int k = i + 1; k < _width && !arr[j][i].isZero(); k++)
				arr[j][k] -= arr[i][k] * arr[j][i] / arr[i][i];
			arr[j][i].set(new RealData(0));
		}
	}

	return arr;
}
Matrix * MatrixNS::Matrix::inverse() throw(ErrCodes)
{
	if (!isSquare()) throw mBad;

	Data::DataWrap **arr = new Data::DataWrap*[_height];
	Data::DataWrap **ret = new Data::DataWrap*[_height];

	for (int i = 0; i < _height; i++)
	{
		arr[i] = new Data::DataWrap[_width];
		ret[i] = new Data::DataWrap[_width];
		for (int j = 0; j < _width; j++)
		{
			arr[i][j] = _table[i][j];
			if (j == i)
				ret[i][j] = Data::DataWrap::getInstance(new RealData(1));
			else
				ret[i][j] = Data::DataWrap::getInstance(new RealData(0));
		}
	}

	for (int i = 0; i < _width; i++)
	{
		bool zFlag = true;
		for (int j = i; j < _height && zFlag; j++)
			if (!arr[j][i].isZero())
			{
				zFlag = false;
				if (i != j)
				{
					Data::DataWrap *tmp = arr[i];
					arr[i] = arr[j];
					arr[j] = tmp;

					tmp = ret[i];
					ret[i] = ret[j];
					ret[j] = tmp;
				}
			}
		if (zFlag)
		{
			for (int j = 0; j < _width; j++)
			{
				delete[] arr[j];
				delete[] ret[j];
			}
			delete arr;
			delete ret;
			throw mZero;
		}
		for (int j = i + 1; j < _height; j++)
		{
			for (int k = i + 1; k < _width && !arr[j][i].isZero(); k++)
				arr[j][k] -= arr[i][k] * arr[j][i] / arr[i][i];
			for (int k = 0; k < _width && !arr[j][i].isZero(); k++)
				ret[j][k] -= ret[i][k] * arr[j][i] / arr[i][i];
			arr[j][i].set(new RealData(0));
		}
	}

	for (int i = _height - 1; i >= 0; i--)
	{
		for (int j = 0; j < _width; j++)
			ret[i][j] /= arr[i][i];
		for (int k = i - 1; k >= 0; k--)
			for (int j = 0; j < _width; j++)
				ret[k][j] -= ret[i][j] * arr[k][i];
	}

	Matrix* X = new Matrix(ret, _width, _width);
	for (int i = 0; i < _width; i++)
		delete[] arr[i];
	delete[] arr;
	return X;
}
Data::DataWrap Matrix::determinant() throw(ErrCodes)
{
	if (!isSquare()) throw mBad;

	bool sign;

	if (_width == 2)
	{
		return _table[0][0] * _table[1][1] - _table[1][0] * _table[0][1];
	} 
	else if (_width == 3)
	{
		return _table[0][0] * (_table[1][1] * _table[2][2] - _table[1][2] * _table[2][1])
			- _table[0][1] * (_table[1][0] * _table[2][2] - _table[2][0] * _table[1][2])
			+ _table[0][2] * (_table[1][0] * _table[2][1] - _table[2][0] * _table[1][1]);
	}

	Data::DataWrap **arr = _diagUp(&sign);
	if (arr == nullptr)
	{
		return Data::DataWrap::getInstance(new RealData(0));
	}

	Data::DataWrap res = arr[0][0].restrictDelete();
	if (!sign)
		res *= Data::DataWrap::getInstance(new RealData(-1));
	for (int i = 1; i < _width && !res.isZero(); i++)
		res *= arr[i][i];

	for (int i = 0; i < _height; i++)
		delete[] arr[i];
	delete[] arr;

	return res.restrictDelete();
}
Data::DataWrap Matrix::trace()
{
	if (!isSquare()) throw mBad;

	Data::DataWrap res = _table[0][0];
	for (int i = 1; i < _width; i++)
		res *= _table[i][i];

	res.restrictDelete();
	return res;
}
DataNS::Data::DataWrap *** MatrixNS::Matrix::getContent()
{
	return &_table;
}
bool Matrix::isZero()
{
	bool res = true;
	try
	{
		res = determinant().isZero();
	}
	catch (ErrCodes)
	{
		res = false;
	}
	return res;
}
bool Matrix::equals(Matrix& src)
{
	if (_width != src._width || _height != src._height) 
		return false;

	for (int i = 0; i < _width; i++)
		for (int j = 0; j < _height; j++)
			if (!(_table[i][j] - src._table[i][j]).forceDelete().isZero()) 
				return false;

	return true;
}
bool Matrix::isSquare()
{
	return _width == _height;
}
void Matrix::output(ostream& ss)
{
	const char separator = ' ';
	for (int i = 0; i < _height; i++)
	{
		for (int j = 0; j < _width; j++)
		{
			ss << _table[i][j];
			if (j != _width - 1) ss << separator;
		}
		ss << endl;
	}
}
int MatrixNS::Matrix::getHeight()
{
	return _height;
}
int MatrixNS::Matrix::getWidth()
{
	return _width;
}
ostream& MatrixNS::operator<<(ostream& ss, Matrix& m)
{
	m.output(ss);
	return ss;
}



/*
long long MatrixNS::timetest(Matrix& A, Matrix& B)
{
	int init0 = __rdtsc();
	A._simpleRMultiply(B);
	init0 -= __rdtsc();

	int init1 = __rdtsc();
	Matrix::ThreadInfo* info;
	HANDLE threads[Matrix::_threadNumber];
	Data::DataWrap** arr = new Data::DataWrap*[A._height];
	for (int i = 0; i < A._height; i++)
		arr[i] = new Data::DataWrap[B._width];

	for (int i = 0; i < Matrix::_threadNumber; i++)
	{
		info = new Matrix::ThreadInfo(&A, &B, i, &arr);
		threads[i] = CreateThread(NULL, 0, Matrix::_threadMultiply, info, 0, NULL);
	}
	WaitForMultipleObjects(Matrix::_threadNumber, threads, true, INFINITE);
	for (int i = 0; i < Matrix::_threadNumber; i++) 
		CloseHandle(threads[i]);
	for (int i = 0; i < A._height; i++)
		delete[] A._table[i];
	delete[] A._table;
	A._table = arr;
	A._width = B._width;
	init1 -= __rdtsc();

	return init0 - init1;
}
*/