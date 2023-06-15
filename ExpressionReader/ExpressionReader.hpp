#include"Pila.hpp"
#include<cmath>

// -Define type pointer to function double (double).
typedef double (*FUNCTION)(double);

class ExpressionReader {
	// -Expression in post-fix notation.
	char *infix, *postfix;

	// -Functions accepted by our program.
	const char* funcNames[6] = {"sin", "cos", "tan", "log",  "ln", "sqrt"};
	FUNCTION    functions[6] = {&sin,  &cos,  &tan,  &log10, &log, &sqrt};

	int numFunc = 6;

	// -When analyzing the strings, well need to know where we are.
	int infixIndex, postfixIndex;
	Pila<char> stack;

	// -Exception message.
	char *syntaxError;

	// -Function of one or two variables.
	bool twoVar;

public:
	// -Default constructor; initializes infix and postfix as NULL.
	ExpressionReader(const char* function);

	~ExpressionReader();

	// -Printing functions.
	void printInfix();
	void printInfixln();
	void printPostfix();
	void printPostfixln();

	// -Taking an expression and saving it together with its postfix form.
	void operator()(const char* function);

	// -Tells us if the function has two variables or just one.
	bool hasTwoVar();

	// -Evaluating expression on (x,y)
	double evaluate(double x = 0, double y = 0);

	// -Evaluates the function just on x, leaving y as 0.
	double evalOn_x(double x);

private:

	// -Prints the part of the infix expression that is still not processed,
	//  the postfix expression of the processed part and the content of the
	//  stack.
	void printState();

	// -Intended for clean the memory when trowing an exception.
	void cleanMemory();

	// -Generates the adequate syntax error message
	void syntaxErrorMsg(const char* description, int at = -1);

	// -Supposing that the characters 'a' and 'b' are binary operations,
	//  this function tell us if the operation represented by 'b' is higher
	//  in the hierarchy than 'a'.
	bool precedence(char a, char b);

	// -Determines is the 'c' belongs to the input alphabet of our grammar.
	bool validChar(char c);

	// -Grammar to implement -----------------------------------------------------

	//  Exp --> E0                         [0 marks the end of the string]
	//  E --> E + T | E - T | T
	//  T --> T * F | T / F | F
	//  F --> N | V | (E) | NAME
	//  NAME --> CONST | FUNC(E)

	// ---------------------------------------------------------------------------

	// -Functions for the grammar.

	// -Reads expression.
	void Expression();
	// -Reads term.
	void Term();
	// -Reads factor.
	void Factor();
	// -Get name of a constant of a function.
	void Name();

	// -Discriminating between constants and
	//  allowed functions inside the post-fix
	//  expression building process. If found,
	//  returns the index of the function in-
	//  side of the array funcNames; if not,
	//  returns -1.
	int isFunction();

	// -Look inside the FUNCTION array and returns
	//  the index of the first coincidence; if not
	//  found any, returns -1.
	int lookFunction(const char* func);

	// -Writes the content of the origin string over the destination
	//  string starting from index. No range is checked.
	void overwrite(const char* origin, char* destination, int index = 0);
};