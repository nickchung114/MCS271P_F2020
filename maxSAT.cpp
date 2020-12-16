#include "timer.c"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <set>
    
std::vector<bool> literals;
std::multimap<int,int> lit_to_clause;

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
int ncs(const std::vector<std::vector<int> >& clauses, const std::vector<bool>& literals, std::set<int>& false_clauses) {
    int numTrue = 0;
    bool clauseValue = true;
    //printf ("Time at start of ncs = %f\n", elapsed_time ());

    //Standard evaluation of clauses
    for (int i = 0; i < int(clauses.size()); i++) {
        for (int j = 0; j < int(clauses[i].size()); j++) {
            //Negative literal
            if (clauses[i][j] < 0) clauseValue = !literals[clauses[i][j] * (-1)];
            //Positive literal
            else clauseValue = literals[clauses[i][j]];

            //True literal found
	          if (clauseValue) {
                //Remove from false clauses if exists there
                if (!false_clauses.empty()) {
                  std::set<int>::iterator it = false_clauses.find(i);
                      if (it != false_clauses.end()) {
                          false_clauses.erase(it);
                      }
                  }
		              numTrue++;
		              j = clauses[i].size();
    	        }
          }
          //Insert into false clauses if no true literal found
          if (false_clauses.empty() && !clauseValue) false_clauses.insert(i);
          if (!false_clauses.empty() && !clauseValue && false_clauses.find(i) == false_clauses.end()) false_clauses.insert(i);
    }
    return numTrue;
}

//
//END MODEL ANALYSIS

//START GREEDY FLIP
//

//Greedily flips a literal to obtain a better output
int greedyFlip(const std::vector<std::vector<int> >& clauses, std::set<int>& false_clauses, int cur_max, int clause) {
    int current_max = 0, temp_max = 0, idx = 0, c = 0;
    bool clauseValue = false;
    for (int j = 0; j < clauses[clause].size(); j++) {
        //Get the number of true clauses containing this literal
        std::pair<std::multimap<int,int>::iterator, std::multimap<int,int>::iterator> ret;
        if (clauses[clause][j] < 0) c = clauses[clause][j] * -1;
        else c = clauses[clause][j];
        ret = lit_to_clause.equal_range(c);

        //Iterate through all the clauses containing the literal and see the inital count of true ones with it
        for (std::multimap<int,int>::iterator it = ret.first; it != ret.second; ++it) {
            for (int i = 0; i < clauses[it->second].size(); i++) {
                if (clauses[it->second][i] < 0) clauseValue = !literals[clauses[it->second][i] * (-1)];
                else clauseValue = literals[clauses[it->second][i]];

              //True clause
              if (clauseValue) current_max++;
            }
        }

        //Flipping the literal
        literals[c] = !literals[c];

        //Evaluate our choice
        for (std::multimap<int,int>::iterator it = ret.first; it != ret.second; ++it) {
            for (int i = 0; i < clauses[it->second].size(); i++) {
              if (clauses[it->second][i] < 0) clauseValue = literals[clauses[it->second][i] * (-1)];
              else clauseValue = literals[clauses[it->second][i]];

              //True clause
              if (clauseValue) temp_max++;
            }
        }
        //Better than the current
        if (temp_max > current_max) {
            idx = c;
            current_max = temp_max;
        }

        //Flip the literal back to let the walkSAT do it
        literals[c] = !literals[c];

        //If somehow we have satisfied every possible clause
        if (cur_max + current_max == int(clauses.size())) {
            return idx;
        }
    }
    return idx;
} 

//
//END GREEDY FLIP

//START MAXSAT
//
std::vector<bool> maxSAT(float p, int maxFlips, int maxInterations, const std::vector<std::vector<int> >& clauses, std::set<int>& false_clauses) {
    int m = int(clauses.size()); //number of clauses in CNF form
    int n = int(literals.size()) - 1; //number of unique variable in CNF form
    int curr_max = 0, res_max = 0, clause_pick = 0, temp_max = 0;
    std::set<int>::iterator it;
    srand(time(NULL));
    std::vector<bool> result;

    //Initialize a random set for comparison
    initializeLiterals(n, result);
    printf ("Time at start of full ncs = %f\n", elapsed_time ());
    randomModelGeneration(result);
    printf ("Time at end of full ncs = %f\n", elapsed_time ());
    res_max = ncs(clauses, result, false_clauses);
    printf ("Time at start of maxSAT = %f\n", elapsed_time ());

    //The WalkSAT
    for (int i = 0; i < maxInterations; i++) {
        randomModelGeneration(literals); //initialize values of literals randomly
        if (!false_clauses.empty()) false_clauses.clear();
        curr_max = ncs(clauses, literals, false_clauses);
        temp_max = curr_max;
        for (int j = 0; j < maxFlips; j++) {

            //We have satisfied every clause
            if (curr_max == m) {
                return literals;
            } 

            //Choose a random unsatisfied clause
            if (!false_clauses.empty()) {
                clause_pick = rand() % false_clauses.size();
                it = false_clauses.begin();
                for (int k = 0; k < clause_pick; k++) {
                    it++;
                }
            }
      	    float x = (float) rand() / RAND_MAX;     //Random decimal between 0 and 1
            int y = 0;

            //Random flip
            if (x < p) {
                int r = rand() % clauses[0].size(); //random integer between 1 and n
                y = clauses[*it][r];
                if (y < 0) y *= -1;
            }

            //Greedy flip
            else {
                y = greedyFlip(clauses, false_clauses, curr_max, *it);
            }

            //Error occurred
            if (y == 0) {
                break;
            }

            //Flip the literal
            literals[y] = !literals[y];
        //}
        curr_max = ncs(clauses, literals, false_clauses);
        //std::cout << "Iteration: " << i << " Improvement: " << curr_max - temp_max << "\n";
        if (curr_max > res_max) {
            result = literals;
            res_max = curr_max;
        }
        }
    }
    printf ("Time at end of maxSAT = %f\n", elapsed_time ());
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
    int count = 0;
    printf ("Time at start of input gathering = %f\n", elapsed_time ());
    while (getline(*infile, line)) {
        for (int i = 0; i < int(line.size()); i++) {
            if (line[i] == ' ') continue;
            else clause.push_back(powerTen(i, line));
            lit_to_clause.insert(std::pair<int,int>(count, i));
        }

        if (int(clause.size()) != k) {				//Checks if there is an error in the input file
            clauses.clear();
            return clauses;
        }
        clauses.push_back(clause);
        count++;
        clause.clear();
    }

    if (int(clauses.size()) != m) {				//Checks if there is an error in the input file
        clauses.clear();
        return clauses;
    }

    printf ("Time at end of input gathering = %f\n", elapsed_time ());
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
    std::set<int> false_clauses;
    std::string line;
    if (argc > 1) {
        start_timer();
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
//        print(clauses, n, k, m);
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
        float p = 0.5;
        int n = 50;
        literals = maxSAT(p, 1000000, 50, clauses, false_clauses);
        int max = ncs(clauses, literals, false_clauses);
        std::cout << "After maxSAT\n";
        //printLiterals();
        std::cout << "New number of true clauses: " << max << "\n";

    }
    else {
        std::cout << "No input given! Please rerun with a valid input.\n";
        return -1;
    }
    return 0;
}
