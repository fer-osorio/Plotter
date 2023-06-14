// -Class to manage a stack. The name is in Spanish in order to not have
//  conflicts with the standard Stack class.
#include<iostream>

template<class T> class Pila {
	struct element {
		T data;
		element *previous;
	} *p ;

public:
	Pila() { // -Constructor: Initializes the stack with NULL.
    	p = NULL;
	}

	void push(T data) { // -Push element on the stack.
    	element* q = new element();
    	q->data = data;
    	q->previous = p;
    	p = q;
	}

	T pop() { // -Pop element.
    	element *q = p;
    	T data;
    	if(p == NULL) throw "\nEmpty stack...\n";
    	p = q->previous;
    	data = q->data;
    	delete q;
    	return data;
	}

	bool isEmpty() { // -Tells if the stack is empty.
    	if(p == NULL) return true;
    	return false;
	}

	void print() {
		element *q = p;
		std::cout << "[ ";
		while(q != NULL) {
			std::cout << q->data;
			q = q->previous;
			if(q != NULL) std::cout << ", ";
			else break;
		}
		std::cout << ']';
	}
};