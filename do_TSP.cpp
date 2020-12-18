#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <regex>
#include <time.h>

using namespace std;

//////////////////// HELPER PRINT FUNCTIONS ////////////////////

void print_v(vector<int> v, string delim=" ") {
	for (int i = 0; i < v.size(); i++) {
		cout << v[i];
		if (i + 1 != v.size()) cout << delim;
	}
	cout << endl;
}

//////////////////// HELPER ALGORITHM FUNCTIONS ////////////////////

void reduce(vector<vector<double>>& D, double& path_len) {
	int i, j, n = D.size();
	double min;
	
	// reduce rows
	for(i = 0; i < n; i++) {
		min = numeric_limits<double>::max();
		for(j = 0; j < n && min != 0; j++)
			min = D[i][j] < min ? D[i][j] : min;
		if (min != numeric_limits<double>::max()) {
			for (j = 0; j < n; j++)
				if(D[i][j] != numeric_limits<double>::max())
					D[i][j] -= min;
			path_len += min;
		}
	}
	
	// reduce columns
	for(j = 0; j < D.size(); j++) {
		min = numeric_limits<double>::max();
		for(i = 0; i < n && min != 0; i++)
			min = D[i][j] < min ? D[i][j] : min;
		if (min != numeric_limits<double>::max()) {
			for (i = 0; i < n; i++)
				if(D[i][j] != numeric_limits<double>::max())
					D[i][j] -= min;
			path_len += min;
		}
	}
}

vector<vector<double>> visit(vector<vector<double>> D, int x) {
	int i, n = D.size();
	for(i = 0; i < n; i++) {
		//D[x][i] = numeric_limits<double>::max(); 
		D[i][x] = numeric_limits<double>::max();
	}
	return D;
}

// for node ordering: sort nodes by distance to the given node
struct less_than_key{
    bool operator() (const pair<int,double>& left, const pair<int,double>& right) {
        return (left.second < right.second);
    }
};

// return a list of nodes sorted in ascending order by distance from node x
vector<pair<int,double>> sort_dist_from(vector<vector<double>>& D, int x) {
	vector<pair<int,double>> ans;
	
	for(int i = 0; i < D.size(); i++)
		ans.push_back({i,D[x][i]});
	sort(ans.begin(),ans.end(),less_than_key());
	
	return ans;
}

vector<int> combine(vector<int> path, int i) {
	path.push_back(i);
	return path;
}

//////////////////// MAIN ALGORITHM ////////////////////
//#define DEBUG
int TSP_COUNT = 0;
clock_t G_TIMER;
int TIMEOUT = 15*30;
/*
vector<int> best_path;
double best_len = numeric_limits<double>::max();
vector<vector<double>> D;
vector<double> DZ;
*/
void TSP(vector<int> path, 
         vector<vector<double>> D,
		 double path_len,
		 vector<int>& best_path,
		 double& best_len,
		 vector<double>& DZ) {
#ifdef DEBUG
	if (++TSP_COUNT % 1000000 == 0) cout << (int)(TSP_COUNT/1000000) << " ";
#endif
	++TSP_COUNT;
	if (((float)(clock() - G_TIMER)) / CLOCKS_PER_SEC > TIMEOUT) {
		//cout << "TIMEOUT" << endl;
		return;
	}
	if(path.size() == D.size()) {
		double tot_len = path_len + DZ[path.back()]; 
		
		if(tot_len < best_len) {
			best_path.swap(path);
			best_len = tot_len;
			cout << "New best path: " << tot_len << endl;
			cout << TSP_COUNT << endl;
			//print_v(path);
		}
#ifdef DEBUG
		else {
			//cout << "X ";
			//print_v(path);
		}
#endif
	}
	else {
		// reduce current distance matrix
		reduce(D,path_len); 
		// prune branch if min len exceeds curr soln
		if (path_len >= best_len) {
#ifdef DEBUG
			//cout << path.size() << " ";
			//print_v(path);
#endif
			return;
		}
		// node ordering
		vector<pair<int,double>> snodes = sort_dist_from(D,path.back());
		for(int i = 0; i < snodes.size(); i++)
			// curr node hasn't been visited and doesn't exceed curr soln's length
			if (D[path.back()][snodes[i].first] != numeric_limits<double>::max()
				&& D[path.back()][snodes[i].first] + path_len < best_len) {
#ifdef DEBUG
				//cout << "just added " << snodes[i].first << endl;
#endif
				// add curr node to curr path & "delete" all distances to curr node
				TSP(combine(path, snodes[i].first),
					visit(D, path.back()),//snodes[i].first),
					D[path.back()][snodes[i].first] + path_len,
					best_path,
					best_len,
					DZ);
			}
	}
}

void print_ans(vector<int> best_path, double best_len) {
	cout << endl << "A best path is the following sequence of nodes:" << endl;
	for(size_t i = 0; i < best_path.size(); i++) {
		cout << best_path[i];
		if(i + 1 != best_path.size()) cout << ", ";
	}
	cout << endl << "and has a length = " << best_len << endl;
}

vector<double> get_DZ(vector<vector<double>>& D) {
	vector<double> ans;
	for (int i = 0; i < D.size(); i++)
		ans.push_back(D[i][0]);
	return ans;
}

double do_alg(vector<vector<double>> D) {
	vector<int> curr_path;
	vector<int> best_path;
	double best_len = numeric_limits<double>::max();
	vector<double> DZ = get_DZ(D); // distance from node i to node 0

	curr_path.push_back(0);
	
	TSP(curr_path,D,0,best_path,best_len,DZ);
	
	if (best_path.size() != D.size()) {
		cout << "Failed to find a best path!" << endl;
		return -1;
	}
	else {
		//print_ans(best_path, best_len);
		return best_len;
	}
}

//////////////////// DISTANCE MATRIX CONSTRUCTION ////////////////////

vector<double> split_nums(string line) {
	string elem;
	vector<double> row;
	istringstream ssline(line);
	
	while(getline(ssline,elem,' '))
		row.push_back(stod(elem));
	return row;
}

vector<vector<double>> get_dist(string fn, string dir = "") {
	ifstream myfile;
	string line;
	vector<vector<double>> dist;
	vector<double> row;
	int i = 0;
	
	fn = dir == "" ? fn : dir + "\\" + fn;
	myfile.open(fn);
	getline(myfile, line);
	while(getline(myfile,line)) {
		dist.push_back(split_nums(line));
	}
	myfile.close();
	
	for (int i = 0; i < dist.size(); i++)
		dist[i][i] = numeric_limits<double>::max();
	
	return dist;
}

//////////////////// FILE I/O HELPER FUNCTIONS ////////////////////

bool file_exists(string fn) {
	ifstream myfile;
	
	myfile.open(fn);
	if(myfile.fail()) {
		cout << "Invalid file!" << endl;
		return false;
	}
	myfile.close();
	return true;
}

string get_fn(char ** argv, int i) {
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
	string INPUT_LIST = dest_dir == "" ? "list.txt" : "\\list.txt";
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

bool valid_args(int argc, char ** argv) {
	bool ans = true;
	int i = 0; 
	while(++i < argc) ans = ans && is_number(argv[i]);
	return ans && argc == 6;
}

#define BATCH
int main(int argc, char ** argv) {	
#ifdef BATCH
	//string t_dir = "C:\\Users\\Nicholas\\Documents\\MCS Program\\Q1\\CS271P\\project\\cs271p\\fall20-benchmark-tsp-problems";
	//string t_dir = "C:\\Users\\Nicholas\\Documents\\MCS Program\\Q1\\CS271P\\project\\cs271p\\tb3";
	string t_dir = "";
	vector<string> fn_list = get_fn_list(t_dir);
	//print_v(fn_list, "\n");
	for (auto a : fn_list) {
		//cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
		for (auto p : extract_params(a)) cout << p << " ";
		//cout << endl << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

		G_TIMER = clock();
		TSP_COUNT = 0;
		
		double t = do_alg(get_dist(a,t_dir));
		
		G_TIMER = clock() - G_TIMER;
		//cout << "Total time taken = " << ((float)G_TIMER)/CLOCKS_PER_SEC << endl;
		//cout << "# TSP calls = " << TSP_COUNT << endl;
		cout << t << " "; 
		cout << ((float)G_TIMER) / CLOCKS_PER_SEC << " ";
		cout << TSP_COUNT << endl;
	}
#else
	if(!valid_args(argc,argv)) {
		cout << "Add arguments for n, k, u, v, p" << endl;
		return 1;
	}
	
	// for each file
	for(int i = 1; i <= stoi(argv[5]); i++) {
		string fn = get_fn(argv, i);
		if(!file_exists(fn)) return 1;
		//Test get_dist
		vector<vector<double>> dist = get_dist(fn);
		for (int i = 0; i < dist.size(); i++) {
			for (int j = 0; j < dist.size(); j++) {
				cout << dist[i][j] << " ";
			}
			cout << endl;
		}
		do_alg(get_dist(fn));
	}
#endif
	return 0;
}