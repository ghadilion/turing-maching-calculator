#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

#define NEWSTATE 0
#define NEWSYMBOL 1
#define DIRECTION 2

#define RIGHT 1
#define LEFT 0

#define B (numTapeSymbols - 1)

#define TOINT(x) (x-'0')

class cell {
public:
    int data;
    cell *left, *right;
    cell(int);
};

cell::cell(int data) {
    this->data = data;
    this->left = this->right = NULL;
}

class turing_machine {
    cell *tape;
    char operation[64];
    int currentState, numStates, numTapeSymbols, **delta[3];
    bool displayTape;
public:
    turing_machine(const char*);
    void insert(int, bool);
    void displayDelta();
    void loadInput();
    void calculate();
    void output();
    void printTape();
};

turing_machine::turing_machine(const char* operation) {
    currentState = 0;
    displayTape = false;
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
                if(i == NEWSTATE) {
                    if(strcmp(input, "-") == 0)
                        delta[i][j][k] = -1;
                    else
                        delta[i][j][k] = atoi(input);
                }
                else if(i == NEWSYMBOL) {
                    if(strcmp(input, "B") == 0)
                        delta[i][j][k] = numTapeSymbols - 1;
                    else if(strcmp(input, "-") == 0)
                        delta[i][j][k] = -1;
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

void turing_machine::insert(int c, bool moveRight) {
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
    int a, b;
    if(strcmp(operation, "2pown") == 0) {
        cout << "\nEnter a number: ";
        cin >> a;
        insert(1, RIGHT);
        for(int i = 0; i < a; ++i)
            insert(0, RIGHT);
        insert(2, RIGHT);
    }
    else if(
        strcmp(operation, "and") == 0 ||
        strcmp(operation, "or") == 0  ||
        strcmp(operation, "xor") == 0
    ) {
        char num1[32], num2[32];
        bool isNotBinary = false;
        do {
            cout << "\nEnter 2 binary numbers of equal length: ";
            cin >> num1 >> num2;
            for(int i = 0; num1[i] != '\0'; ++i)
                if(num1[i] != '1' && num1[i] != '0') {
                    isNotBinary == true;
                    break;
                }
            if(isNotBinary)
                continue;
            for(int i = 0; num2[i] != '\0'; ++i)
                if(num2[i] != '1' && num2[i] != '0') {
                    isNotBinary == true;
                    break;
                }
        } while (strlen(num1) != strlen(num2) || isNotBinary);
        for(int i = 0; num1[i] != '\0'; ++i)
            insert(TOINT(num1[i]), RIGHT);
        insert(4, RIGHT);
        for(int i = 0; num2[i] != '\0'; ++i)
            insert(TOINT(num2[i]), RIGHT);
    }
    else {
        cout << "\nEnter 2 numbers";
        if(strcmp(operation, "div") == 0)
            cout << " (enter divisor followed by dividend)";
        cout << " : ";
        cin >> a >> b;
        for(int i = 0; i < a; ++i)
            insert(0, RIGHT);
        insert(1, RIGHT);
        for(int i = 0; i < b; ++i)
            insert(0, RIGHT);
        if(strcmp(operation, "mul") == 0 || strcmp(operation, "div") == 0)
            insert(1, RIGHT);
    }

    insert(B, LEFT);
    while(tape->left)
        tape = tape->left;
    char option;
    cout << "Would you like to view the tape after every modification? (y/n): ";
    cin >> option;
    if(option == 'y')
        displayTape = true;
    if(displayTape)
        printTape();
}

void turing_machine::calculate() {
    while(currentState != numStates) {
        if(delta[NEWSTATE][tape->data][currentState] == -1) {
            cout << "\nError in input tape!";
            break;
        }
        int newState = delta[NEWSTATE][tape->data][currentState]; 
        insert (
            delta[NEWSYMBOL][tape->data][currentState],
            delta[DIRECTION][tape->data][currentState]
        );
        currentState = newState;
        if(displayTape)
            printTape();
    }
    while(tape->left)
        tape = tape->left;
}

void turing_machine::output() {
    int answer = 0, answer2 = 0;
    cell *temp = tape;
    while(temp && temp->data == 0) {
        answer++;
        temp = temp->right;
    }
    if(strcmp(operation, "div") == 0) {
        temp = temp->right;
        while(temp && temp->data == 0) {
            answer2++;
            temp = temp->right;
        }
        cout << "Quotient: " << answer2 << "\nRemainder: " << answer << "\n\n"; 
    }
    else if(
        strcmp(operation, "and") == 0 ||
        strcmp(operation, "or") == 0  ||
        strcmp(operation, "xor") == 0
    ) {
        cell *temp = tape;
        cout << "Output on tape: ";
        while(temp) {
            cout << temp->data;
            temp = temp->right;
        }
        cout << "\n\n";
    }
    
    else
        cout << "Output on tape: " << answer << "\n\n";
}

void turing_machine::printTape() {
    cell *temp = tape;
    while(temp->left)
        temp = temp->left;
    while(temp) {
        if(temp->data == B) {
            if(temp == tape)
                cout << "[B] ";
            else
                cout << "B";
        }
        else if(temp == tape) {
            cout << "[" << temp->data << "] ";
        }
        else
            cout << temp->data << " ";
        temp = temp->right;
    }
    cout << "\nCurrent State: " << currentState;
    if(currentState == numStates)
        cout << " (final state)";
    else if(currentState == 0)
        cout << " (start state)";
    cout << endl << endl;
}

int main(int argc, char* argv[]) {
    turing_machine T(argv[1]);
    // T.displayDelta();
    T.loadInput();
    T.calculate();
    T.output();

}