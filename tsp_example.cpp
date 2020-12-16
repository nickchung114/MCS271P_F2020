
#include <climits>
#include <algorithm>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <string>
#include <sstream>
#include <regex>

using namespace std;
//const int N = 10;

// final_path[] stores the final solution ie, the 
// path of the salesman. 
vector<int> final_path;

// visited[] keeps track of the already visited nodes 
// in a particular path 
vector<bool> visited;

// Stores the final minimum weight of shortest tour. 
double final_res = numeric_limits<double>::max();

// Function to copy temporary solution to 
// the final solution 
void copyToFinal(vector<int> curr_path)
{
	int n = curr_path.size() - 1;
	for (int i = 0; i < curr_path.size() - 1; i++)
		final_path[i] = curr_path[i];
	final_path[n] = curr_path[0];
}

// Function to find the minimum edge cost 
// having an end at the vertex i 
double firstMin(vector<vector<double>> adj, double i)
{
	double min = numeric_limits<double>::max();
	for (int k = 0; k < adj.size(); k++)
		if (adj[i][k] < min && i != k)
			min = adj[i][k];
	return min;
}

// function to find the second minimum edge cost 
// having an end at the vertex i 
double secondMin(vector<vector<double>> adj, double i)
{
	double first = INT_MAX, second = INT_MAX;
	for (int j = 0; j < adj.size(); j++)
	{
		if (i == j)
			continue;

		if (adj[i][j] <= first)
		{
			second = first;
			first = adj[i][j];
		}
		else if (adj[i][j] <= second &&
			adj[i][j] != first)
			second = adj[i][j];
	}
	return second;
}

// function that takes as arguments: 
// curr_bound -> lower bound of the root node 
// curr_weight-> stores the weight of the path so far 
// level-> current level while moving in the search 
//		 space tree 
// curr_path[] -> where the solution is being stored which 
//			 would later be copied to final_path[] 
void TSPRec(vector<vector<double>> adj, double curr_bound, double curr_weight,
	int level, vector<int> curr_path)
{
	// base case is when we have reached level N which 
	// means we have covered all the nodes once 
	int n = adj.size();
	if (level == n)
	{
		// check if there is an edge from last vertex in 
		// path back to the first vertex 
		if (adj[curr_path[level - 1]][curr_path[0]] != 0)
		{
			// curr_res has the total weight of the 
			// solution we got 
			double curr_res = curr_weight +
				adj[curr_path[level - 1]][curr_path[0]];

			// Update final result and final path if 
			// current result is better. 
			if (curr_res < final_res)
			{
				copyToFinal(curr_path);
				final_res = curr_res;
			}
		}
		return;
	}

	// for any other level iterate for all vertices to 
	// build the search space tree recursively 
	for (int i = 0; i < adj.size(); i++)
	{
		// Consider next vertex if it is not same (diagonal 
		// entry in adjacency matrix and not visited 
		// already) 
		if (adj[curr_path[level - 1]][i] != 0 &&
			visited[i] == false)
		{
			int temp = curr_bound;
			curr_weight += adj[curr_path[level - 1]][i];

			// different computation of curr_bound for 
			// level 2 from the other levels 
			if (level == 1)
				curr_bound -= ((firstMin(adj, curr_path[level - 1]) +
					firstMin(adj, i)) / 2);
			else
				curr_bound -= ((secondMin(adj, curr_path[level - 1]) +
					firstMin(adj, i)) / 2);

			// curr_bound + curr_weight is the actual lower bound 
			// for the node that we have arrived on 
			// If current lower bound < final_res, we need to explore 
			// the node further 
			if (curr_bound + curr_weight < final_res)
			{
				curr_path[level] = i;
				visited[i] = true;

				// call TSPRec for the next level 
				TSPRec(adj, curr_bound, curr_weight, level + 1,
					curr_path);
			}

			// Else we have to prune the node by resetting 
			// all changes to curr_weight and curr_bound 
			curr_weight -= adj[curr_path[level - 1]][i];
			curr_bound = temp;

			// Also reset the visited array 
			for (int i = 0; i < adj.size(); i++) {
				visited[i] = false;
			}

			for (int j = 0; j <= level - 1; j++)
				visited[curr_path[j]] = true;
		}
	}
}

// This function sets up final_path[] 
void TSP(vector<vector<double>> adj)
{
	const int n = adj.size();
	vector<int> curr_path;
	for (int i = 0; i <= n; i++) {
		curr_path.push_back(-1);
	}

	// Calculate initial lower bound for the root node 
	// using the formula 1/2 * (sum of first min + 
	// second min) for all edges. 
	// Also initialize the curr_path and visited array 
	double curr_bound = 0;

	// Compute initial bound 
	for (int i = 0; i < n; i++)
		curr_bound += (firstMin(adj, i) +
			secondMin(adj, i));

	// Rounding off the lower bound to an integer 
	//curr_bound = (curr_bound & 1) ? curr_bound / 2 + 1 : curr_bound / 2;
	curr_bound = curr_bound / 2;

	// We start at vertex 1 so the first vertex 
	// in curr_path[] is 0 
	visited[0] = true;
	curr_path[0] = 0;

	// Call to TSPRec for curr_weight equal to 
	// 0 and level 1 
	TSPRec(adj, curr_bound, 0, 1, curr_path);
}

//////////////////// DISTANCE MATRIX CONSTRUCTION ////////////////////

vector<double> split_nums(string line) {
	string elem;
	vector<double> row;
	istringstream ssline(line);

	while (getline(ssline, elem, ' '))
		row.push_back(stod(elem));
	return row;
}

vector<vector<double>> get_dist(string fn) {
	ifstream myfile;
	string line;
	vector<vector<double>> dist;
	vector<double> row;
	int i = 0;

	//fn = dir == "" ? fn : dir + "\\" + fn;
	myfile.open(fn);
	getline(myfile, line);
	while (getline(myfile, line)) {
		dist.push_back(split_nums(line));
	}
	myfile.close();

	/*for (int i = 0; i < dist.size(); i++)
		dist[i][i] = numeric_limits<double>::max();*/

	return dist;
}

//////////////////// FILE I/O HELPER FUNCTIONS ////////////////////

bool file_exists(string fn) {
	ifstream myfile;

	myfile.open(fn);
	if (myfile.fail()) {
		cout << "Invalid file!" << endl;
		return false;
	}
	myfile.close();
	return true;
}

string get_fn(char** argv, int i) {
	string fn = "tsp-problem-";
	for (int j = 1; j <= 4; j++) {
		fn += argv[j];
		fn += '-';
	}
	return fn + to_string(i) + ".txt";
}

bool is_number(const std::string& s)
{
	int i;
	for (i = 0; i < s.length() && isdigit(s[i]); i++);
	return !s.empty() && i == s.length();
}

//////////////////// BENCHMARK BATCH PROCESSING //////////////////////

/*
* get a list of files
* for each file, run TSP
*/

vector<string> get_fn_list(string dest_dir) {
	ifstream myfile;
	string line;
	string INPUT_LIST = "\\list.txt";
	vector<string> fn_list;

	if (file_exists(dest_dir + INPUT_LIST)) {
		myfile.open(dest_dir + INPUT_LIST);
		while (getline(myfile, line)) {
			if (line.substr(0, 11) == "tsp-problem")
				fn_list.push_back(line);
		}
		myfile.close();
	}
	return fn_list;
}

vector<int> extract_params(string tsp_fn) {
	regex rgx(R"(tsp-problem-(\d+)-(\d+)-(\d+)-(\d+).*)");
	vector<int> ans;
	smatch m;

	if (regex_search(tsp_fn, m, rgx)) {
		for (int i = 1; i < 5; i++) {
			ans.push_back(stoi(m[i]));
		}
	}
	return ans;
}

//////////////////// MAIN ////////////////////

bool valid_args(int argc, char** argv) {
	bool ans = true;
	int i = 0;
	while (++i < argc) ans = ans && is_number(argv[i]);
	return ans && argc == 6;
}

// Driver code 
int main(int argc, char** argv) {
	if (!valid_args(argc, argv)) {
		cout << "Add arguments for n, k, u, v, p" << endl;
		return 1;
	}

	vector<vector<double>> dist;

	// for each file
	for (int i = 1; i <= stoi(argv[5]); i++) {
		string fn = get_fn(argv, i);
		if (!file_exists(fn)) return 1;
		//Test get_dist
		dist = get_dist(fn);
		for (int i = 0; i < dist.size(); i++) {
			for (int j = 0; j < dist.size(); j++) {
				cout << dist[i][j] << " ";
			}
			cout << endl;
		}
	}
	for (int i = 0; i < dist.size(); i++) {
		final_path.push_back(0);
		visited.push_back(false);
	}
	final_path.push_back(0);

	TSP(dist);

	printf("Minimum cost : %f\n", final_res);
	printf("Path Taken : ");
	for (int i = 0; i <= dist.size(); i++)
		printf("%d ", final_path[i]);

	return 0;
}
