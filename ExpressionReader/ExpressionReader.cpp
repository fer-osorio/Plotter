#include"ExpressionReader.hpp"
//#include"Exception.hpp"

using namespace std;

// -Default constructor.
ExpressionReader::ExpressionReader(const char* function) :
infixIndex(0), postfixIndex(0), syntaxError(NULL) {
    int len = 0, i = 0;

    // Length of the array, not of the string.
    while(function[len++] != 0);

    infix = new char[len];
    postfix = new char[len << 1]; // len * 2

    for(i = 0; i < len; i++) {
        infix[i] = function[i];
        postfix[i] = 0;
        postfix[i + len] = 0;
    }

    try {
        Expression();
    } catch(const char* e) {
        cout << e << '\n';
        throw;
    }
}

ExpressionReader::~ExpressionReader() {
    cleanMemory();
}

void ExpressionReader::printState() {
    int inflen = -1, i;
    // -Infix expression length.
    while(infix[++inflen] != 0) {}
    inflen += 10;
    // -Showing the part of the infix expression
    //  that is still not processed.
    cout << &infix[infixIndex];
    // -Padding, aesthetic purposes.
    for(i = 0; i < infixIndex; i++) cout << ' ';
    cout << " | ";
    // -Showing the processed string.
    cout << postfix;
    // -Padding, aesthetic purposes.
    if (inflen - postfixIndex > 0)
        for(i = postfixIndex; i < inflen; i++) cout << ' ';

    cout << " | ";
    // -Showing stack.
    stack.print();

    cout << '\n';
}

void ExpressionReader::cleanMemory() {
    if(postfix   != NULL) {
        delete[] postfix;
        postfix = NULL;
    }
    if(infix     != NULL) {
        delete[] infix;
        infix = NULL;
    }
    if(syntaxError != NULL) {
        delete[] syntaxError;
        syntaxError = NULL;
    }
}

void ExpressionReader::syntaxErrorMsg(const char *description, int at) {
    if(at < 0) at = infixIndex;
    int len0 = -1, infixlen = -1, deslen = -1;
    int partialLen, synErrorLoc, i;
    char msg0[] = "\nSyntax error: ";

    while(msg0[++len0] != 0);
    while(infix[++infixlen] != 0);
    while(description[++deslen] != 0);

    if(syntaxError != NULL) delete[] syntaxError;
    syntaxError = new char[(len0 + infixlen + 1) * 2 + deslen + 2];

    for(i = 0; i < len0; i++) syntaxError[i] = msg0[i];
    for(i = 0; i < infixlen; i++) syntaxError[len0 + i] = infix[i];
    syntaxError[len0 + i] = '\n';
    partialLen = len0 + infixlen + 1;
    synErrorLoc = len0 + at;
    for(i = 0; i <= synErrorLoc - 4; i++) syntaxError[partialLen + i] = ' ';
    syntaxError[partialLen + (i++)] = '~';
    syntaxError[partialLen + (i++)] = '~';
    syntaxError[partialLen + (i++)] = '^';
    syntaxError[partialLen + (i++)] = '\n';
    partialLen += i;
    for(i = 0; i < deslen; i++) syntaxError[partialLen + i] = description[i];
    syntaxError[partialLen + (i++)] = '\n';
    syntaxError[partialLen + i] = 0;
}

void ExpressionReader::printInfix() {
    if(infix != NULL) cout << infix;
}

void ExpressionReader::printInfixln() {
    if(infix != NULL) cout << infix << '\n';
}

void ExpressionReader::printPostfix() {
    if(postfix != NULL) cout << postfix;
}

void ExpressionReader::printPostfixln() {
    if(postfix != NULL) cout << postfix << '\n';
}

void ExpressionReader::operator()(const char* function) {
    int len = 0, i = 0;
    while(function[len++] != 0); // Length of the array, not of the string.

    // -In case of having an old expression.
    if(infix != NULL) delete[] infix;
    if(postfix != NULL) delete[] postfix;

    infix = new char[len];
    postfix = new char[len << 1]; // len * 2

    for(i = 0; i < len; i++) {
        infix[i] = function[i];
        postfix[i] = 0;
        postfix[i + len] = 0;
    }

    try {
        Expression();
    } catch(const char* e) {
        cout << e;
    }
}

bool ExpressionReader::precedence(char a, char b) {
    int pri_a = 0, pri_b = 0;

    // -Priority of a.
    if(a == '+' || a == '-') pri_a = 1;
    else if(a == '*' || a == '/' || a == '%') pri_a = 2;
    // -Priority of b.
    if(b == '+' || b == '-') pri_b = 1;
    else if(b == '*' || b == '/' || b == '%') pri_b = 2;

    return pri_a < pri_b;
}

bool ExpressionReader::validChar(char c) {
    return (c >= 40 && c <= 57)  || (c >= 65 && c <= 90) ||
           (c >= 97 && c <= 122) || c == 0;
}

// -Grammar to implement -----------------------------------------------------

//  Exp --> E0                         [0 marks the end of the string]
//  E --> E + T | E - T | T
//  T --> T * F | T / F | F
//  F --> N | V | (E) | NAME
//  NAME --> CONST | FUNC(E)

// ---------------------------------------------------------------------------

// -Functions for the grammar.

void ExpressionReader::Expression() {
    // -Using E --> E + T | E - T | T and the fact that addition and
    //  subtraction are commutative.
    printState();
    try {
        Term();
    } catch(const char*) {
        throw;
    }
    char a,b;

    // -Repeating cycle till something different from '+' or '-' is
    //  found.
    while(true) {
        switch(infix[infixIndex]) {
            case '+' :
                b = '+';
                break;
            case '-' :
                b = '-';
                break;
            default:
                // -Zero case: End of string.
                if(infix[infixIndex] == 0) {
                    while(!stack.isEmpty())// -Empty the stack.
                        postfix[postfixIndex++] = stack.pop();
                    postfix[postfixIndex] = 0;// -End of string.
                    printState();
                    return ;
                }
                return ;
        }
        if(stack.isEmpty()) stack.push(b);
        else {
            a = stack.pop();
            if(precedence(a, b)) stack.push(a);
            else postfix[postfixIndex++] = a;
            stack.push(b);
        }
        infixIndex++;
        try {
            Term();
        } catch(const char*) {
            throw;
        }
    }
}

void ExpressionReader::Term() {
    // -Using T --> T * F | T / F | F and the fact that multiplication and
    //  division are commutative.
    printState();
    // -Zero case: End of string.
    if(infix[infixIndex] == 0) return ;

    try {
        Factor();
    } catch(const char*) {
        throw;
    }
    char a,b;

    // -Repeating cycle till something different from '*' or '/' is
    //  found.
    while(true) {
        switch(infix[infixIndex]) {
            case '*' :
                b = '*';
                break;
            case '/' :
                b = '/';
                break;
            default:
                if(!validChar(infix[infixIndex])) {
                    syntaxErrorMsg("Invalid character...");
                    throw syntaxError;
                }
                return ;
        }
        if(stack.isEmpty()) stack.push(b);
        else {
            a = stack.pop();
            if(precedence(a, b)) stack.push(a);
            else postfix[postfixIndex++] = a;
            stack.push(b);
        }
        infixIndex++;
        try {
            Factor();
        } catch(const char*) {
            throw ;
        }
    }
}

void ExpressionReader::Factor() {
    printState();
    bool pointFound = false, noIntegerPart;
    int mark = 0;

    // -Zero case: End of string.
    if(infix[infixIndex] == 0) return ;

    // -First case: Number.
    if((infix[infixIndex] > 47 && infix[infixIndex] < 58) ||
    infix[infixIndex] == '.' || infix[infixIndex] == ','  ||
    infix[infixIndex] == '+' || infix[infixIndex] == '-'  ) {
        postfix[postfixIndex++] = '<'; // -Beginning of multicharacter token.

        // -Sign found, passed to the output.
        if(infix[infixIndex] == '+' || infix[infixIndex] == '-') {
            postfix[postfixIndex++] = infix[infixIndex++];
        }

        // -Case of number formed just by digits to the right of
        //  decimal point.
        if(infix[infixIndex] == '.' || infix[infixIndex] == ',') {
            noIntegerPart = true;
            pointFound = true;
        }

        while((infix[infixIndex] > 47 && infix[infixIndex] < 58) ||
        infix[infixIndex] == '.' || infix[infixIndex] == ',') {
            // -Send number to output.
            postfix[postfixIndex++] = infix[infixIndex++];
            if((infix[infixIndex] == '.' || infix[infixIndex] == ',')
            && pointFound) {
                syntaxErrorMsg("A number can't have two decimal points...");
                throw syntaxError;
            }
            if(infix[infixIndex] == '.' || infix[infixIndex] == ',')
                pointFound = true;
        }
        if(infix[infixIndex - 1] == '+' || infix[infixIndex - 1] == '-') {
            infixIndex--;
            syntaxErrorMsg("Expected a digit or a point...");
            throw syntaxError;
        }

        if(postfix[postfixIndex-1] == '.' || postfix[postfixIndex-1] == ',') {
            if(noIntegerPart) {
                infixIndex--;
                syntaxErrorMsg("A single point doesn't form a valid number...");
                throw syntaxError;
            }
            else postfixIndex--;
        }
        postfix[postfixIndex++] = '>'; // -End of a multi-character token.
        return ;
    }
    // -Second case: Variable.
    if(infix[infixIndex] == 'x' || infix[infixIndex] == 'y') {
        postfix[postfixIndex++] = infix[infixIndex++];
        return ;
    }
    // -Third case: Expression in parenthesis.
    if(infix[infixIndex] == '(') {
        // -Mark; we will use it in the case of not finding the ')'.
        // -Since we'll not use the parenthesis in the postfix expression,
        //  we can discard it just after reading it.
        mark = infixIndex++;
        stack.push('(');
        try {
            Expression();
        } catch(const char*) {
            throw;
        }
        if(infix[infixIndex++] != ')') {
            infixIndex--;
            syntaxErrorMsg("Corresponding closing parenthesis never found...",
            mark);
            throw syntaxError;
        }
        // -Sending the stack elements to the output till a '(' is
        //  found.
        while((postfix[postfixIndex++] = stack.pop()) != '(') {}
        // -Ignoring '('.
        postfixIndex--;
        return ;
    }

    // -Four case: Functions and constants.
    if( (infix[infixIndex] > 64 && infix[infixIndex] < 91) ||
        (infix[infixIndex] > 96 && infix[infixIndex] < 123) ) {
        Name(); return;
    }
    syntaxErrorMsg("Invalid character...");
    throw syntaxError;
}

void ExpressionReader::Name() {
    printState();
    int _case;

    // -Functions
    if((_case = isFunction()) != -1) {
        int i, mark = infixIndex - 1;
        try{ Expression(); }
        catch(const char*) { throw; }

        if(infix[infixIndex++] != ')') {
            syntaxErrorMsg("Corresponding closing parenthesis never found...",
            mark);
            throw  syntaxError;
        }

        // -Sending the stack elements to the output till a '(' is
        //  found.
        while((postfix[postfixIndex++] = stack.pop()) != '(') ;
        // -Ignoring '('.
        postfixIndex--;

        // -Identifying functions with '['.
        postfix[postfixIndex++] = '[';
        // -It can be either 'log' or 'ln'.

        overwrite(funcNames[_case], postfix, postfixIndex);
        for(i = 0; funcNames[_case][i] != 0; i++);
        postfixIndex += i;

        postfix[postfixIndex++] = ']';
        return;
    } else {
        // -Constants
        postfix[postfixIndex++] = '{';
        while( (infix[infixIndex] > 64 && infix[infixIndex] < 91) ||
        (infix[infixIndex] > 96 && infix[infixIndex] < 123) ) {
            postfix[postfixIndex++] = infix[infixIndex++];
        }
        postfix[postfixIndex++] = '}';
    }
}

int ExpressionReader::isFunction() {
    printState();
    int i, j;
    for(i = 0; i < numFunc; i++) {
        for(j = 0; infix[infixIndex + j] == funcNames[i][j] &&
        funcNames[i][j] != 0; j++);
        if(funcNames[i][j] == 0 && infix[infixIndex + j] == '(') {
            infixIndex += j + 1;
            stack.push('(');
            return i;
        }
    }

    return -1;
}

int ExpressionReader::lookFunction(const char *func) {
    int i, j;
    for(i = 0; i < numFunc; i++) {
        for(j = 0; func[j] == funcNames[i][j] && funcNames[i][j] != 0; j++);
        if(funcNames[i][j] == 0 && func[j] == 0) {
            return i;
        }
    }

    return -1;
}

void ExpressionReader::overwrite(const char *origin, char *destination,
int index) {
    for(int i = 0; origin[i] != 0; i++, index++)
        destination[index] = origin[i];
}

double ExpressionReader::evaluate(double x, double y, bool showProcess) {
    Pila<double> values;
    double tmp = 0;

    int poslen = -1;
    while(postfix[++poslen] != 0) {}

    auto evaluationState = [this, &values] (int k) {
        cout << &postfix[k];
        while(0 < k--) cout << ' ';
        cout << " | ";
        values.print();
        cout << '\n';
    };

    if(showProcess) evaluationState(0);
    for(int i = 0; postfix[i] != 0;) {
        // -First case: Number.
        if(postfix[i] == '<') { i++;
	        bool isNegative = false;
	        double integer = 0, fractional = 0;
	        int last;
	        if(postfix[i] == '-') {
		        isNegative = true;
		        i++;
	        }
	        // Positive sign ignored.
	        if(postfix[i] == '+') i++;
	        // -Extracting (if any) integer part.
	        for(; postfix[i] != '.' && postfix[i] != ',' && postfix[i] != '>';
	        i++) integer = integer * 10.0 + (postfix[i] - 48);

	        // -Point found, ignored. Extracting fractional part
	        if(postfix[i] == '.' || postfix[i] == ',') {
	            // -Same algorithm that the one used to extract the
	            //  integer part, but in reverse and with '/'.
	            while(postfix[++i] != '>') {} last = i;
	            for(i--; postfix[i] != '.' && postfix[i] != ','; i--)
	                fractional = fractional / 10.0 + (postfix[i] - 48);
	            // -One last division is necessary.
	            fractional /= 10.0;
	            i = last; // -Taking i to the end of the number.
	        }
            tmp = integer + fractional;
            // -Debugging purposes.
            // cout << tmp << '\n';
	        if(isNegative) tmp = -tmp;
        	values.push(tmp); i++;
        	if(showProcess) evaluationState(i);
        }

        // -Second case: Variables.
        if(postfix[i] == 'x') {
            values.push(x); i++;
            if(showProcess) evaluationState(i);
        }
        if(postfix[i] == 'y') {
            values.push(y); i++;
            if(showProcess) evaluationState(i);
        }

        // -Third case : Constants
        if(postfix[i] == '{') {
            cout << "\n Define the value of: ";
            while(postfix[i++] != '}') cout << postfix[i];
            cout << " = ";
            cin >> tmp;
            values.push(tmp);
            if(showProcess) evaluationState(i);
        }

        // -Fourth case: functions
        if(postfix[i] == '[') {
            char fname[6]; int j;

            // -Getting name of the function.
            for(j = 0, i++; postfix[i] != ']';j++, i++)
                fname[j] = postfix[i];

            fname[j] = 0;
            j = lookFunction(fname);
            tmp = values.pop();
            values.push(functions[j](tmp));
            i++; if(showProcess) evaluationState(i);
        }

        // -Fifth case: Operations.
        double a,b;
        switch(postfix[i]) {
            case '+':
                a = values.pop();
                b = values.pop();
                values.push(a + b); ++i;
                if(showProcess) evaluationState(i);
                break;
            case '-':
                a = values.pop();
                b = values.pop();
                values.push(a - b); ++i;
                if(showProcess) evaluationState(i);
                break;
            case '*':
                a = values.pop();
                b = values.pop();
                values.push(a * b); ++i;
                if(showProcess) evaluationState(i);
                break;
            case '/':
                a = values.pop();
                b = values.pop();
                if(b == 0) throw "Division by zero...";
                values.push(a / b); ++i;
                if(showProcess) evaluationState(i);
                break;
        }
    }
    return values.pop();
}

