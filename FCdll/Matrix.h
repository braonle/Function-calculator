#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <ostream>

using namespace std;

/**
	Error codes thrown by several methods. Common for Data types and Matrix
	correct		-	returned by calculating methods if operation succeeded
	bad_ptr		-	returned by calculating methods if parameter type does not match
	zero_div	-	returned by division methods if parameter equals 0
	no_mem		-	thrown in case no memory is available; used rarely
	no_type_impl -	thrown by _argConvert() if dynamic cast returns pointer for which type conversion is not 
					implemented; in this case _argConvert() and _getCode() should be extended
	unknown_type -	thrown by _getCode() if pointer type is not considered; in this case 
					_argConvert() and _getCode() should be extended
	bad_num_format - thrown by a parser if an input text does not correspond to a number of Data types
	mMismatch	-	thrown by Matrix methods, especially when matrices have incompatible sizes
	mEmpty		-	thrown by a few Matrix methods if internal table poiner in NULL
	doubleTmpUsage - thrown by DataWrap operators if a temporary unit is used twice
	mBad		-	thrown by Matrix methods if a matrix is not a square one
	mZero		-	thrown by Matrix methods if a matrix has a zero determinant
	zero_pow	-	returned by log() if Data equals 0
	not_supported - for calculations which are not supported yet but are taken into account for development
*/
enum ErrCodes
{
	correct, bad_ptr, zero_div, no_mem, no_type_impl, unknown_type,
	bad_num_format, mMismatch, mEmpty, doubleTmpUsage, mBad, mZero, zero_pow,
	not_supported
};

typedef long double InsideType;

namespace DataNS
{
	/**
		Returned by _getCode() and then used by _argConvert() to convert types to compatible ones
	*/
	enum DataCodes
	{
		nullPtr, rCode, cCode
	};

	/**
		Common class for handling calculations. Contains DataWrap which should be used for calculations.	
	*/
	class Data
	{
	public:
		class DataWrap;
		/** 
			Adds an object of the same type to the current one. Should not be used directly
			\param	dt	-	pointer to an object of the same type; otherwise a ErrCodes::bad_ptr would be returned 
			\return			result of the operation
		*/
		virtual ErrCodes add(Data *dt) = 0;
		/**
			Substracts an object of the same type of the current one. Should not be used directly
			\param	dt	-	pointer to an object of the same type; otherwise a ErrCodes::bad_ptr would be returned
			\return			result of the operation
		*/
		virtual ErrCodes substract(Data *dt) = 0;
		/**
			Multiplies the current object by the one of the same type. Should not be used directly
			\param	dt	-	pointer to an object of the same type; otherwise a ErrCodes::bad_ptr would be returned
			\return			result of the operation
		*/
		virtual ErrCodes multiply(Data *dt) = 0;
		/**
			Divides the current object by the one of the same type. Should not be used directly
			\param	dt	-	pointer to an object of the same type; otherwise a ErrCodes::bad_ptr would be returned
			\return			result of the operation
		*/
		virtual ErrCodes divide(Data *dt) = 0;
		virtual InsideType abs(void) = 0;
		virtual Data* sin() = 0;
		virtual Data* cos() = 0;
		virtual Data* sinh() = 0;
		virtual Data* cosh() = 0;
		virtual Data* tan() = 0;
		virtual Data* tanh() = 0;
		//virtual Data* asin() = 0;
		//virtual Data* acos() = 0;
		//virtual Data* asinh() = 0;
		//virtual Data* acosh() = 0;
		//virtual Data* atan() = 0;
		//virtual Data* atanh() = 0;
		virtual Data* log() = 0;
		virtual Data* sqr() = 0;
		virtual Data* sqrt() = 0;
		virtual Data* exp() = 0;
		/**
			Used to write the contents of this object to the specified output stream.
			\param	ss	-	output stream, where data are to be written
		*/
		virtual void output(ostream& ss) = 0;
		/**
			Returns the conjugated object of this one.
			\return		conjugated object
		*/
		virtual Data* conjugate() = 0;
		/**
			Clones current object and its data. The result is independent from this object.
			\return		cloned object
		*/
		virtual Data* clone() = 0;
		/**
			Used for objects' comminication. Do not use directly.
		*/
		//Returns the encapsulated data in a specific form
		virtual void* _getData() = 0;
		/**
			Determines if this object is a neutral one for addition.
		*/
		virtual bool isZero() = 0;
		virtual ~Data() {};
		/**
			Class that handles Data calculations and offers more flexibility. Designed for the direct usage.
		*/
	private:
		//Implements type conversion for Data inheritants. Should be extended if new types and added.
		static void _argConvert(Data** arg1, Data** arg2) throw (ErrCodes);
		//Returns the code of Data inheritant. Should be extended if new types and added.
		static DataCodes _getCode(Data* code) throw (ErrCodes);
	protected:
		//Determines if the two numbers are equal with certain precision.
		static bool Data::_eq(InsideType num, InsideType data);
	};

	class Data::DataWrap
	{
		///Data type to be wrapped
		Data* _ptr;
		/// A flag which shows if this instance is to free Data on _ptr
		bool _tmpFlag;
		/// A flag to indicate if the value is returned from operators or alike operations; saves temporary status
		bool _opFlag;
	public:
		/**
			Usual constructor
			\param ptr	-	pointer to Data instance
			\param isTmp -	indicates if this instance is temporary; mostly for optimization
		*/
		DataWrap(Data *ptr = nullptr, bool isTmp = false) :_ptr(ptr), _tmpFlag(isTmp) { _opFlag = false; };
		/**
			Used by return and when passing DataWrap as a parameter; do not modify
			\param	src	-	instance which is copied if it is permanent; otherwise contents are taken
		*/
		DataWrap(DataWrap& src);
		/**
			Addition operator; makes its parameters permanent; 
			wrappes Data methods and handles type conversion
			
			\param param2	-	number to add
			\return				result which is a temporary instance
		*/
		Data::DataWrap operator + (Data::DataWrap& param2) throw (ErrCodes);
		/**
			Substraction operator; makes its parameters permanent;
			wrappes Data methods and handles type conversion

			\param param2	-	number to subctract
			\return				result which is a temporary instance
		*/
		Data::DataWrap operator - (Data::DataWrap& deduction) throw (ErrCodes);
		/**
			Multiplication operator; makes its parameters permanent;
			wrappes Data methods and handles type conversion

			\param param2	-	number to multiply by
			\return				result which is a temporary instance
		*/
		Data::DataWrap operator * (Data::DataWrap& param2) throw (ErrCodes);
		/**
			Division operator; makes its parameters permanent;
			wrappes Data methods and handles type conversion

			\param param2	-	number to divide by
			\return				result which is a temporary instance
		*/
		Data::DataWrap operator / (Data::DataWrap& divisor) throw (ErrCodes);
		/**
			Addition operator; makes its parameter permanent;
			wrappes Data methods and handles type conversion;
			if parameter is temporary, takes it content

			\param param2	-	number to add
			\return				result which is a reference to a permanent instance
		*/
		Data::DataWrap& operator += (Data::DataWrap& param2) throw (ErrCodes);
		/**
			Substraction operator; makes its parameter permanent;
			wrappes Data methods and handles type conversion;
			if parameter is temporary, takes it content

			\param param2	-	number to substract
			\return				result which is a reference to a permanent instance
		*/
		Data::DataWrap& operator -= (Data::DataWrap& deduction) throw (ErrCodes);
		/**
			Multiplication operator; makes its parameter permanent;
			wrappes Data methods and handles type conversion;
			if parameter is temporary, takes it content

			\param param2	-	number to add
			\return				result which is a reference to a permanent instance
		*/
		Data::DataWrap& operator *= (Data::DataWrap& param2) throw (ErrCodes);
		/**
			Division operator; makes its parameter permanent;
			wrappes Data methods and handles type conversion;
			if parameter is temporary, takes it content

			\param param2	-	number to add
			\return				result which is a reference to a permanent instance
		*/
		Data::DataWrap& operator /= (Data::DataWrap& divisor) throw (ErrCodes);
		/**
			Assigns the value; if it temporary, takes its contents; othewise copies it
			\param src	-	value to aasign
			\return			result which is a reference to a permanent instance
		*/
		Data::DataWrap& operator = (Data::DataWrap& src) throw (ErrCodes);
		/*
			Returns the copy for RealData and a conjugate for ComplexData
			\return		a permanent instance
		*/
		Data::DataWrap conjugate();
		/**
			Returns a separate clone of the instance, no changes will affect parent object
			\return		a separate copy
		*/
		Data::DataWrap* clone();
		/**
			Frees current contents and assigns a new one; use with caution
			\param	src	-	a new instance for the contents
			\return			a reference to this object
		*/
		Data::DataWrap& set(Data* src);
		/**
			Makes this instance to free contents when deleted; use with caution
			\return			a reference to this permanent object
		*/
		Data::DataWrap& forceDelete();
		/**
			Releases this instance from freeing its contents when deleted; use with caution
			\return			a reference to this permanent object
		*/
		Data::DataWrap& restrictDelete();
		/**
			Indicates if the instance of equal to a neutral element for addition
			\return		obvious
		*/
		bool isZero();
		/**
			Converts the contents to std::string representation
			\return		an instance of std::string
		*/
		string toString();
		/**
			Writes the contents in text format to the output stream
		*/
		void output(ostream& ss);
		/**
			Allows to create on object with defined contents; 
			useful in calculations for creating a constant in expressions;
			similar to result returned by operators
			
			\return		temporary instance of DataWrap with defined contents
		*/
		static Data::DataWrap getInstance(Data* src);
		Data* getContent();


		InsideType abs();

		//Needs to be tested if right mathematical results are returned, especially using
		//ComplexData inner object
		Data::DataWrap cos();
		Data::DataWrap sin();
		Data::DataWrap exp();
		Data::DataWrap sqr();
		Data::DataWrap sqrt();
		Data::DataWrap tan();
		Data::DataWrap sinh();
		Data::DataWrap cosh();
		Data::DataWrap tanh();
		///Gives the main branch of logarithm
		Data::DataWrap log();
		Data::DataWrap pow(Data::DataWrap&);
		//Data::DataWrap asin();
		//Data::DataWrap acos();
		//Data::DataWrap asinh();
		//Data::DataWrap acosh();
		//Data::DataWrap atan();
		//Data::DataWrap atanh();
		~DataWrap();
	};

	ostream& operator<<(ostream& out, Data::DataWrap& src);
		
	class RealData : public Data
	{
		InsideType _data;
	public:
		RealData(InsideType dt = 0);
		virtual ErrCodes add(Data *dt);
		virtual ErrCodes substract(Data *dt);
		virtual ErrCodes multiply(Data *dt);
		virtual ErrCodes divide(Data *dt);
		virtual Data* sin();
		virtual Data* cos();
		virtual Data* sinh();
		virtual Data* cosh();
		virtual Data* tan() throw(ErrCodes);
		virtual Data* tanh();
		//virtual Data* asin();
		//virtual Data* acos();
		//virtual Data* asinh();
		//virtual Data* acosh();
		//virtual Data* atan();
		//virtual Data* atanh();
		virtual Data* log();
		virtual Data* sqr();
		virtual Data* sqrt();
		virtual Data* exp();
		virtual void output(ostream& ss);
		virtual Data* conjugate();
		virtual InsideType abs(void);
		virtual void* _getData();
		virtual bool isZero();
		virtual Data* clone();
		virtual ~RealData();
	};
	class ComplexData : public Data
	{
		InsideType _re;
		InsideType _im;
	public:
		ComplexData(InsideType re = 0, InsideType im = 0);
		ComplexData(RealData* src);
		virtual ErrCodes add(Data *dt);
		virtual ErrCodes substract(Data *dt);
		virtual ErrCodes multiply(Data *dt);
		virtual ErrCodes divide(Data *dt);
		virtual Data* sin();
		virtual Data* cos();
		virtual Data* sinh();
		virtual Data* cosh();
		virtual Data* tan();
		virtual Data* tanh();
		//virtual Data* asin();
		//virtual Data* acos();
		//virtual Data* asinh();
		//virtual Data* acosh();
		//virtual Data* atan();
		//virtual Data* atanh();
		virtual Data* log();
		virtual Data* sqr();
		virtual Data* sqrt();
		virtual Data* exp();
		virtual Data* conjugate();
		virtual void output(ostream& ss);
		virtual InsideType abs(void);
		virtual void* _getData();
		virtual bool isZero();
		virtual Data* clone();
		virtual ~ComplexData();
	};

}

namespace MatrixNS
{
	class Matrix
	{
		///Size of internal table
		int _width, _height;
		///Table of DataWrap instances
		DataNS::Data::DataWrap** _table;
	public:
		/**
			Common constructor

			\param	input	-	a pointer to the table of DataWrap's
			\param	height	-	the first index for input table
			\param	width	-	the second index for input table
		*/
		Matrix(DataNS::Data::DataWrap** input, int width = 1, int height = 1);
		/**
			Copy constructor; makes a separate copy independent from parent instance
			\param	m	-	matrix to be copied
		*/
		Matrix(Matrix& m);
		/**
			Adds the input matrix to itself; might use several threads

			\param	input	-	matrix to be added
			\return				reference to this instance
		*/
		Matrix& add(Matrix& input) throw (ErrCodes);
		/**
			Substracts the input matrix from itself; might use several threads

			\param	input	-	matrix to be substracted
			\return				reference to this instance
		*/
		Matrix& substract(Matrix& input) throw (ErrCodes);
		/**
			Multiplies the contents by input

			\param	input	-	reference to a number to multiply by
			\return				reference to this instance
		*/
		Matrix& multiply(DataNS::Data::DataWrap& input) throw (ErrCodes);
		/**
			Multiplies the contents by input; input is then deleted

			\param	input	-	pointer to a number to multiply by
			\return				reference to this instance
		*/
		Matrix& multiply(DataNS::Data::DataWrap* input) throw (ErrCodes);
		/**
			Multiplies this matrix by input; input is put to the left

			\param	input	-	matrix to multiply by
			\return				reference to this instance
		*/
		Matrix& multiplyL(Matrix& input) throw (ErrCodes);
		/**
			Multiplies this matrix by input; input is put to the right

			\param	input	-	matrix to multiply by
			\return				reference to this instance
		*/
		Matrix& multiplyR(Matrix& input) throw (ErrCodes);
		/**
			Finds an inverse matrix if possible; the instance is independent

			\return		pointer to an inverse matrix
		*/
		Matrix* inverse() throw (ErrCodes);
		/**
			Calculates the determinant if possible

			\return			determinant
		*/
		DataNS::Data::DataWrap determinant() throw (ErrCodes);
		/**
			Calculates the trace (product of diagonal elements) if possible

			\return			trace
		*/
		DataNS::Data::DataWrap trace() throw (ErrCodes);
		/**
			Gives a pointer to internal contents; use with caution

			\return		a pointer to _table
		*/
		DataNS::Data::DataWrap*** getContent();
		/**
			Returns a transpose matrix if this instance contains only RealData

			\return		instance of conjugate matrix
		*/
		Matrix* conjugate();
		/**
			Positive if detetminant exists and equal to neutral element for addition
		*/
		bool isZero();
		/**
			Determaines if the matrix is symmetric
			\return		-1 - if the matrix is not square
						 0 - if it is square and symmetric
						 1 - if it is square but not symmetric
		*/
		int isSymmetric();
		/**
			Determaines if the matrix is unitary (conjugate equals inverse) 
			\return		-1 - if the matrix is not square
						 0 - if it is square and unitary
						 1 - if it is square but not unitary
		*/
		int isUnitary();
		/**
			Determines if this matrix have the same contents
			\param	src		-	matrix to compare to
		*/
		bool equals(Matrix& src);
		/**
			Determines if this matrix is square (width = height)
		*/
		bool isSquare();
		/**
			Writes this instance to the output stream as a text (std::string)
		*/
		void output(ostream& ss);
		int getHeight();
		int getWidth();
		~Matrix();
	private:

		///Used by determinant(); gives a diagonal representation of the table
		DataNS::Data::DataWrap** _diagUp(bool* sign);
		///Structure used to hand over parameters to thread functions
		struct ThreadInfo
		{
			ThreadInfo(Matrix *tg, Matrix * sr, int s, DataNS::Data::DataWrap*** sr1 = nullptr) : src(sr), target(tg), seq(s), src1(sr1){};
			Matrix *target, *src;
			DataNS::Data::DataWrap*** src1;
			int seq;
		};
		///Number of threads to use
		static const int _threadNumber = 4;
		///Actual adding function; does not use threads
		void _simpleAdd(Matrix& input) throw (ErrCodes);
		///Actual adding function; uses threads
		static DWORD WINAPI _threadAdd(LPVOID data);
		///Actual substracting function; does not use threads
		void _simpleSubstract(Matrix& input) throw (ErrCodes);
		///Actual substracting function; uses threads
		static DWORD WINAPI _threadSubstract(LPVOID);

		///Actual multuplyL function; does not use threads
		void _simpleLMultiply(Matrix& input) throw (ErrCodes);
		///Actual multuplyR function; does not use threads
		void _simpleRMultiply(Matrix& input) throw (ErrCodes);
		///Actual multiplying function; uses threads; target is the left matrix, src - the right
		static DWORD WINAPI _threadMultiply(LPVOID data);
	};

	ostream& operator<< (ostream& ss, Matrix& m);
}