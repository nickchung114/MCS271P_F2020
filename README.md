# MCS271P_F2020
## (1) SLS - maxSAT.cpp
 Max-SAT - Use SLS approachh to find a true/false (0/1) assignment to variables such that the number of clauses satisfied is maximized. (See details in `maxSAT.cpp`)
 #### usage
```bash
g++ maxSAT.cpp -o maxSAT
maxSAT [input_file]
```
The input file must have:
    - first row  -> {n} : number of variables
    - second row -> {k} : number of literals per clause
    - third row  -> {m} : number of clauses
    - all other (m) rows have one clause per row,
    (each clause is a disjunction of literals,
    each variable is represented by its index (1~N), e.g, X5 is written as "5" (without quotes)
    if a variable is negated, it has a "-" symbol before it, e.g., ~X5 is written as "-5" (without quotes)
    all literals are delimited by a whitespace,
    e.g., if the clause is X1 v ~X5 v X35, the row is "1 -5 35" (without quotes))
The output: Print the true/false (0/1) assignment of variables and the number of satisfied clauses.

## (2) BnB-DFS - do_TSP.cpp
 Travelling Sales Person - Use BnB-DFS to find an ordered list of the locations (each location appears on the list exactly once), such that the total distance travelled is minimized. (See details in `do_TSP.cpp`)
