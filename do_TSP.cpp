#include <iostream>
#include <fstream>
#include <limits>

using namespace std;

//////////////////// HELPER ALGORITHM FUNCTIONS ////////////////////

void reduce(vector<vector<double>>& D, double& path_len) {
	int i, j, min, n = D.size();
	
	// reduce rows
	for(i = 0; i < n; i++) {
		min = numeric_limits<double>::max();
		for(j = 0; j < n && min != 0; j++)
			min = D[i][j] < min ? D[i][j] : min;
		for(j = 0; j < n; j++)
			D[i][j] -= min;
		path_len += min;
	}
	
	// reduce columns
	for(j = 0; j < D.size(); j++) {
		min = numeric_limits<double>::max();
		for(i = 0; i < n && min != 0; i++)
			min = D[i][j] < min ? D[i][j] : min;
		for(i = 0; i < n; i++)
			D[i][j] -= min;
		path_len += min;
	}
}

vector<vector<double>> visit(vector<vector<double>> D, int x) {
	int i, n = D.size();
	for(i = 0; i < n; i++) {
		D[x][i] = numeric_limits<double>::max(); 
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
	ans.sort(ans.begin(),ans.end(),less_than_key());
	
	return ans;
}

vector<int> combine(vector<int> path, int i) {
	path.push_back(i);
	return path;
}

//////////////////// MAIN ALGORITHM ////////////////////

void TSP(vector<int> path, 
         vector<vector<double>> D,
		 double path_len,
		 vector<int>& best_path,
		 double& best_len) {
	if(path.size() == D.size()) {
		int tot_len = path_len + D[*path.back()][*path.front()]; 
		
		if(tot_len < best_len) {
			best_path.swap(path);
			best_len = tot_len;
		}
	}
	else {
		// reduce current distance matrix
		reduce(D,path_len); 
		// prune branch if min len exceeds curr soln
		if(path_len >= best_len) return; 
		// node ordering
		vector<pair<int,double>> snodes = sort_dist_from(D,*path.back());
		for(int i = 0; i < snodes.size(); i++)
			// curr node hasn't been visited and doesn't exceed curr soln's length
			if(D[*path.back()][snodes[i].first] != numeric_limits<double>::max()
				&& D[*path.back()][snodes[i].first] + path_len < best_len)
				// add curr node to curr path & "delete" all distances to curr node
				TSP(combine(path,snodes[i].first), 
				    visit(D,snodes[i].first), 
					D[*path.back()][snodes[i].first] + path_len,
					best_path,
					best_len);
	}
}

void print_ans(vector<int> best_path, double best_len) {
	cout << "A best path is the following sequence of nodes:" << endl;
	for(size_t i = 0; i < best_path.size(); i++) {
		cout << best_path[i];
		if(i + 1 != best_path.size()) cout << ", ";
	}
	cout << endl << "and has a length = " << best_len << endl;
}

void do_alg(vector<vector<double>> D) {
	vector<int> curr_path;
	vector<int> best_path;
	double best_len = numeric_limits<double>::max();
	
	curr_path.push_back(0);
	
	TSP(curr_path,D,0,best_path,best_len);
	
	if(best_path.size() != D.size())
		cout << "Failed to find a best path!" << endl;
	else
		print_ans(best_path,best_len);
}

//////////////////// DISTANCE MATRIX CONSTRUCTION ////////////////////

vector<double> split_nums(string line) {
	string elem;
	vector<double> row;
	stringstream ssline(line);
	
	while(getline(ssline,elem,' '))
		row.push_back(stod(elem));
	return row;
}

vector<vector<double>> get_dist(string fn) {
	ofstream myfile;
	string line;
	vector<vector<double>> dist;
	vector<double> row;
	int i = 0;
	
	myfile.open(fn);
	while(getline(myfile,line)) {
		if(i++>0) dist.push_back(split_nums(line));
	}
	myfile.close();
	
	for(int i = 0; i < D.size(); i++)
		D[i][i] = numeric_limits<double>::max();
	
	return dist;
}

//////////////////// FILE I/O HELPER FUNCTIONS ////////////////////

bool file_exists(string fn) {
	ofstream myfile;
	
	myfile.open(fn);
	if(myfile.fail()) {
		cout << "Invalid file!" << endl;
		return false;
	}
	myfile.close();
	return true;
}

string get_fn(char ** argv, int i) {
	return "tsp-problem-" + str(argv[1]) +
	     + "-" + str(argv[2]) 
		 + "-" + str(argv[3])
		 + "-" + str(argv[4])
		 + "-" + to_string(i);
}

bool is_number(const std::string& s)
{
    string::iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

//////////////////// MAIN ////////////////////

bool valid_args(int argc, char ** argv) {
	bool ans = true;
	int i = 0; 
	while(++i < argc) ans = ans && is_number(argv[i]);
	return ans && argc == 6;
}

int main(int argc, char ** argv) {	
	if(valid_args(argc,argv)) {
		cout << "Add arguments for n, k, u, v, p" << endl;
		return 1;
	}
	
	// for each file
	for(int i = 1; i <= stoi(argv[5]); i++) {
		if(!file_exists(get_fn(argv,i)) return 1;
		do_alg(get_dist(get_fn(argv,i)));
	}
	
	return 0;
}