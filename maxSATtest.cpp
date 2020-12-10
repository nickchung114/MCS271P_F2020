#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <map>
#include <stdlib.h>
#include <time.h>

std::vector<bool> literals;


//START DEBUG PRINTING
//

//Prints the clauses (debugging)
void print(std::vector<std::vector<int> > clauses, int n, int k, int m) {
    std::cout << "\n\nThe number of literals is: " << n << "\n";
    std::cout << "The number of clauses is: " << m << "\n";
    std::cout << "The number of literals per clause is: " << k << "\n\n";
    for (int i = 0; i < int(clauses.size()); i++) {
        for (int j = 0; j < int(clauses[i].size()); j++) {
            std::cout << clauses[i][j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n\n";
}

void printLiterals() {
    for (int i = 1; i < int(literals.size()); i++) {
        std::cout << i << " = " << literals[i] << "\n";
    }
    std::cout << "\n\n";
}

//
//END DEBUG PRINTING


//START MODEL CREATION
//

//Initialize all the literals to false to start
void initializeLiterals(int n, std::vector<bool>& literals) {
    literals.push_back(false);
    for (int i = 1; i <= n; i++) {
        literals.push_back(false);
    }
}

//Random initialization of values
void randomModelGeneration(std::vector<bool>& literals) {
    //srand(time(NULL));
    int temp = 0;
    for (int i = 0; i < int(literals.size()); i++) {
        temp = rand() % 10;
        if (temp < 5) literals[i] = true;
        else literals[i] = false;
    }
}

//
//END MODEL CREATION

//START MODEL ANALYSIS
//

//Determines the number of true clauses
int ncs(const std::vector<std::vector<int> >& clauses, const std::vector<bool>& literals) {
    int numTrue = 0;
    bool clauseValue = true;
    for (int i = 0; i < int(clauses.size()); i++) {
        clauseValue = true;
        for (int j = 0; j < int(clauses[i].size()); j++) {
            if (clauses[i][j] < 0) clauseValue = clauseValue && !literals[clauses[i][j] * (-1)];
            else clauseValue = clauseValue && literals[clauses[i][j]];
        }
        if (clauseValue) {
            numTrue++;
        }
    }
    return numTrue;
}

//
//END MODEL ANALYSIS

//START GREEDY FLIP
//

//Greedily flips a literal to obtain a better output
int greedyFlip(const std::vector<std::vector<int> >& clauses) {
    int current_max = ncs(clauses, literals);
    int temp_max = 0;
    int idx = 0;
    for (int j = 1; j < int(literals.size()); j++) {
        literals[j] = !literals[j];
        temp_max = ncs(clauses, literals);
        if (temp_max > current_max) {
            idx = j;
            current_max = temp_max;
        }
        literals[j] = !literals[j];
        if (current_max == int(clauses.size())) {
            return idx;
        }
    }
    return idx;
}

//
//END GREEDY FLIP

//START MAXSAT
//
std::vector<bool> maxSAT(float p, int maxFlips, int maxInterations, const std::vector<std::vector<int> >& clauses) {
    int m = int(clauses.size()); //number of clauses in CNF form
    int n = int(literals.size()) - 1; //number of unique variable in CNF form
    srand(time(NULL));
    std::vector<bool> result;
    initializeLiterals(n, result);
    randomModelGeneration(result);
    for (int i = 1; i <= maxInterations; i++) {
        randomModelGeneration(literals); //initialize values of literals randomly
        for (int j = 0; j <= maxFlips; j++) {
            if (ncs(clauses, literals) == m) {
                return literals;
            }
	    float x = (float) rand() / RAND_MAX;     //Random decimal between 0 and 1
            int y = 0;
            if (x < p) {
                y = rand() % n + 1; //random integer between 1 and n
                //std::cout << "Random: " << x << "\n";
            }
            else {
                y = greedyFlip(clauses);
 		//std::cout << "Greedy\n";
            }
            if (y == 0) {
                break;
            }
            literals[y] = !literals[y];
        }
        if (ncs(clauses, literals) > ncs(clauses, result)) {
            result = literals;
        }
    }
    return result;
}

//
//END MAXSAT

//START READ INPUT
//

//Processes the number on each line, returns the integer value of the string number given
int powerTen(int& i, std::string line) {
    int num = 0;
    int digits = 0;
    int neg = 1;
    for (int j = i; j < int(line.size()); j++) {
        if (line[j] == ' ') break;
        if (line[j] == '-') {
            neg = -1;
            continue;
        }
        digits++;
    }
    for (; digits > 0; digits--) {
        if (line[i] == '-') {
            i++;
            digits++;
            continue;
        }
        num += (line[i] - '0') * int(pow(10, double(digits - 1)));
        i++;
    }
    num *= neg;
    return num;
}

//Creates our clauses in the form of a 2D array, each clause is a row
std::vector<std::vector<int> > createClauses(std::ifstream* infile, int m, int k) {
    std::vector<std::vector<int> > clauses;
    std::vector<int> clause;
    std::string line;
    while (getline(*infile, line)) {
        for (int i = 0; i < int(line.size()); i++) {
            if (line[i] == ' ') continue;
            else clause.push_back(powerTen(i, line));
        }

        if (int(clause.size()) != k) {				//Checks if there is an error in the input file
            clauses.clear();
            return clauses;
        }
        clauses.push_back(clause);
        clause.clear();
    }

    if (int(clauses.size()) != m) {				//Checks if there is an error in the input file
        clauses.clear();
        return clauses;
    }

    return clauses;
}

//Obtains the number of literals as per the input file header
int getNumLiterals(std::string line) {
    int i = 0;
    int n = powerTen(i, line);
    return n;
}

//Obtains the length of the clauses as per the input file header
int getLengthClauses(std::string line) {
    int i = 0;
    int k = powerTen(i, line);
    return k;
}

//Obtains the number of clauses as per the input file header
int getNumClauses(std::string line) {
    int i = 0;
    int m = powerTen(i, line);
    return m;
}

//
//END READ INPUT

int main(int argc, char** argv) {
    std::ifstream infile;
    int n = 0, m = 0, k = 0;
    std::vector<std::vector<int> > clauses;
    std::string line;
    if (argc > 1) {
        infile.open(argv[1]);
        getline(infile, line);
        n = getNumLiterals(line);
        getline(infile, line);
        k = getLengthClauses(line);
        getline(infile, line);
        m = getNumClauses(line);

        clauses = createClauses(&infile, m, k);
        initializeLiterals(n, literals);
        if (clauses.empty()) {
            std::cout << "Error in input file! Please check the file and rerun.\n";
            return -1;
        }
        print(clauses, n, k, m);
/*
        randomModelGeneration(literals);
        printLiterals(); 
        int max = ncs(clauses, literals);
        std::cout << "Number of true clauses: " << max << "\n"; */

        //Testing the greedy flip
        /*int to_flip = greedyFlip(clauses);
        literals[to_flip] = !literals[to_flip];
        max = ncs(clauses, literals);
        std::cout << "After 1 st greedy flip\n";
        printLiterals();
        std::cout << "New number of true clauses: " << max << "\n";*/

        //Testing the maxSAT
        float p = 0.6;
        int n = 100;
        literals = maxSAT(p, 150, 100, clauses);
        int max = ncs(clauses, literals);
        std::cout << "After maxSAT\n";
        printLiterals();
        std::cout << "New number of true clauses: " << max << "\n";

    }
    else {
        std::cout << "No input given! Please rerun with a valid input.\n";
        return -1;
    }
    return 0;
}
