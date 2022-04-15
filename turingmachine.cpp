#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

#define NEWSTATE 0
#define NEWSYMBOL 1
#define DIRECTION 2

#define RIGHT 1
#define LEFT 0

#define CHARTOINT(x) (x - '0')
#define INTTOCHAR(x) (x + '0')

#define B INTTOCHAR(numTapeSymbols - 1)

class cell {
public:
    char data;
    cell *left, *right;
    cell(char);
};

cell::cell(char data) {
    this->data = data;
    this->left = this->right = NULL;
}

class turing_machine {
    cell *tape;
    char operation[64];
    int currentState, numStates, numTapeSymbols, **delta[3];
public:
    turing_machine(const char*);
    void insert(char, bool);
    void displayDelta();
    void loadInput();
    void calculate();
    void output();
};

turing_machine::turing_machine(const char* operation) {
    currentState = 0;
    strcpy(this->operation, operation);

    ifstream fin;
    char input[64], fileName[64];
    strcpy(fileName, operation);
    strcat(fileName, ".txt");
    fin.open(fileName);
    
    fin >> numStates;
    fin >> numTapeSymbols;

    tape = new cell(B);

    // dynamically allocate delta tensor
    for(int i = 0; i < 3; ++i) {
        delta[i] = new int*[numTapeSymbols];
        for(int j = 0; j < numTapeSymbols; ++j) {
            delta[i][j] = new int[numStates];
        }
    }
    // load delta values from file
    for(int k = 0; k < numStates; ++k) {
        for(int j = 0; j < numTapeSymbols; ++j) {
            for(int i = 0; i < 3; ++i) {
                fin >> input;
                if(i == NEWSTATE)
                    delta[i][j][k] = atoi(input);
                else if(i == NEWSYMBOL) {
                    if(strcmp(input, "B") == 0)
                        delta[i][j][k] = numTapeSymbols - 1;
                    else
                        delta[i][j][k] = atoi(input); 
                }
                else if(i == DIRECTION) {
                    if(strcmp(input, "R") == 0)
                        delta[i][j][k] = 1;
                    else
                        delta[i][j][k] = 0;
                }   
            }
        }
    }
}

void turing_machine::insert(char c, bool moveRight) {
    tape->data = c;
    if(moveRight) {
        if(tape->right == NULL) {
            tape->right = new cell(B);
            tape->right->left = tape;
        }
        tape = tape->right;
        if(c == B) {
            delete tape->left;
            tape->left = NULL;
        }
    }
    else {
        if(tape->left == NULL) {
            tape->left = new cell(B);
            tape->left->right = tape;
        }
        tape = tape->left;
        if(c == B) {
            delete tape->right;
            tape->right = NULL;
        }
    }
}

void turing_machine::displayDelta() {
    for(int k = 0; k < numStates; ++k) {
        for(int j = 0; j < numTapeSymbols; ++j) {
            for(int i = 0; i < 3; ++i) {
                if(delta[i][j][k] == numTapeSymbols - 1 && i == NEWSYMBOL)
                    cout << "B,";
                else if(i == DIRECTION) {
                    if(delta[i][j][k] == 1)
                        cout << "R";
                    else
                        cout << "L";
                }
                else
                    cout << delta[i][j][k] << ",";
            }
            cout << "\t";
        }
        cout << endl;
    }
}

void turing_machine::loadInput() {
    if(strcmp(operation, "add") == 0) {
        cout << "\nEnter 2 numbers: ";
        int a, b;
        cin >> a >> b;
        for(int i = 0; i < a; ++i)
            insert('0', RIGHT);
        insert('1', RIGHT);
        for(int i = 0; i < b; ++i)
            insert('0', RIGHT);
        tape = tape->left;
        while(tape->left)
            tape = tape->left;
    }
}

void turing_machine::calculate() {
    while(currentState != numStates) {
        if(delta[NEWSTATE][CHARTOINT(tape->data)][currentState] == -1) {
            cout << "\nError in input tape!";
            break;
        }
        int newState = delta[NEWSTATE][CHARTOINT(tape->data)][currentState]; 
        insert(INTTOCHAR(delta[NEWSYMBOL][CHARTOINT(tape->data)][currentState]), delta[DIRECTION][CHARTOINT(tape->data)][currentState]);
        currentState = newState;
    }
    while(tape->left)
        tape = tape->left;
}

void turing_machine::output() {
    int answer = 0;

    while(strcmp(operation, "add") == 0 && tape->right && tape->data == '0') {
        answer++;
        tape = tape->right;
    }
    cout << answer;

}

int main(int argc, char* argv[]) {
    turing_machine T(argv[1]);
    // T.displayDelta();
    T.loadInput();
    T.calculate();
    T.output();

}