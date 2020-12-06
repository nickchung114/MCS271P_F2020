#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <map>
#include <stdlib.h>
#include <time.h>
using namespace std;

vector<bool> literals;

//START MODEL CREATION
//

//Initialize all the literals to false to start
void initializeLiterals(int n) {
  literals.push_back(false);
  for (int i = 1; i <= n; i++) {
    literals.push_back(false);
  }
}

//Random initialization of values
void randomModelGeneration() {
  srand(time(NULL));
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
int ncs(vector<vector<int> > clauses) {
  int numTrue = 0;
  bool clauseValue = true;
  for (int i = 0; i < int(clauses.size()); i++) {
    clauseValue = true;
    for (int j = 0; j < int(clauses[i].size()); j++) {
      if (clauses[i][j] < 0) clauseValue = (clauseValue && !literals[(clauses[i][j] * -1)]);
      else clauseValue = (clauseValue && literals[clauses[i][j]]);
    }
    if (clauseValue) numTrue++;
  }
  return numTrue;
}

//
//END MODEL ANALYSIS

//START GREEDY FLIP
//

//Greedily flips a literal to obtain a better output
int greedyFlip(vector<vector<int> > clauses) {
  int current_max = ncs(clauses);
  int temp_max = 0;
  int idx = 0;
  for (int j = 1; j < int(literals.size()); j++) {
    literals[j] = !literals[j];
    temp_max = ncs(clauses);
    if (temp_max > current_max) {
      idx = j;
      current_max = temp_max;
    }
    literals[j] = !literals[j];
  }
  return idx;
}

//
//END GREEDY FLIP

//START READ INPUT
//

//Processes the number on each line, returns the integer value of the string number given
int powerTen(int& i, string line) {
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
vector<vector<int> > createClauses(ifstream* infile, int m, int k) {
  vector<vector<int> > clauses;
  vector<int> clause;
  string line;
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
int getNumLiterals(string line) {
  int i = 0;
  int n = powerTen(i, line);
  return n;
}

//Obtains the length of the clauses as per the input file header
int getLengthClauses(string line) {
  int i = 0;
  int k = powerTen(i, line);
  return k;
}

//Obtains the number of clauses as per the input file header
int getNumClauses(string line) {
  int i = 0;
  int m = powerTen(i, line);
  return m;
}

//
//END READ INPUT

//START DEBUG PRINTING
//

//Prints the clauses (debugging)
void print(vector<vector<int> > clauses, int n, int k, int m) {
  cout << "\n\nThe number of literals is: " << n << "\n";
  cout << "The number of clauses is: " << m << "\n";
  cout << "The number of literals per clause is: " << k << "\n\n";
  for (int i = 0; i < int(clauses.size()); i++) {
    for (int j = 0; j < int(clauses[i].size()); j++) {
      cout << clauses[i][j] << " ";
    }
    cout << "\n";
  }
  cout << "\n\n";
}

void printLiterals() {
  for (int i = 1; i < int(literals.size()); i++) {
    cout << i << " = " << literals[i] << "\n";
  }
  cout << "\n\n";
}

//
//END DEBUG PRINTING

int main(int argc, char** argv) {
  ifstream infile;
  int n = 0, m = 0, k = 0;
  vector<vector<int> > clauses;
  string line;
  if (argc > 1) {
    infile.open(argv[1]);
    getline(infile, line);		
    n = getNumLiterals(line);
    getline(infile, line);
    k = getLengthClauses(line);
    getline(infile, line);
    m = getNumClauses(line);

    clauses = createClauses(&infile, m, k);
    initializeLiterals(n);
    if (clauses.empty()) {
      cout << "Error in input file! Please check the file and rerun.\n";
      return -1;
    }
    print(clauses, n, k, m);

    randomModelGeneration();
    printLiterals();
    int max = ncs(clauses);
    cout << "Number of true clauses: " << max << "\n";

    //Testing the greedy flip
    int to_flip = greedyFlip(clauses);
    literals[to_flip] = !literals[to_flip];
    max = ncs(clauses);
    cout << "After greedy flip\n";
    printLiterals();
    cout << "New number of true clauses: " << max << "\n";

  } else {
    cout << "No input given! Please rerun with a valid input.\n";
    return -1;
  }
  return 0;
}
