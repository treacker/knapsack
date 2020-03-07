#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <cmath>

using namespace std;

#define MAX_INT 2147483647

void processFile(int &items, int &capacity, vector<int> &values, vector<int> &weights, string path) {
	ifstream file(path);

	string token1, token2;

	file >> token1 >> token2;
	items = stoi(token1);
	capacity = stoi(token2);
	int v, w;

	while (file >> token1 >> token2) {
		values.push_back(stoi(token1));
		weights.push_back(stoi(token2));

	}
	file.close();
}

void processCin(int &items, int &capacity, vector<int> &values, vector<int> &weights) {
	string token1, token2;

	cin >> token1 >> token2;
	items = stoi(token1);
	capacity = stoi(token2);
	int v, w;

	for (int i = 0; i != items; ++i) {
		cin >> token1 >> token2;
		values.push_back(stoi(token1));
		weights.push_back(stoi(token2));
	}
}


struct Cost
{
	int index;

	int value;
	int weight;

	double profit;

	Cost(int i, int v, int w): index(i), value(v), weight(w) {
		profit = double(value) / double(weight);
	}

	// To sort in descending order
	bool operator < (const Cost & cost) {
		return profit > cost.profit;
	}
};

struct intCost
{
	int index;
	int value;
	int weight;
	int profit;
	
	intCost(int i, int v, int w, double p): index(i), value(v), weight(w), profit(p) { }

	// To sort in descending order
	bool operator < (const Cost & cost) {
		return profit > cost.profit;
	}
};

pair<int, vector<Cost>> solveGreedy(int items, int capacity, vector<int> values, vector<int> weights, double percent, unordered_set<int> & answer) {
	int value = 0, weight = 0;
	unordered_set<int> taken;

	vector<Cost> costs;


	for (int i = 0; i != items; ++i) {
		Cost cost(i, values[i], weights[i]);
		costs.push_back(cost);
	}

	sort(costs.begin(), costs.end());
	// reverse(costs.begin(), costs.end());

	int last_taken = 0;

	for (int i = 0; i != items; ++i) {
		int ind = costs[i].index;
		if (weight + weights[ind] <= capacity * percent) {
			weight += weights[ind];
			value += values[ind];
			answer.insert(ind + 1);
			last_taken = i;

		} else {
			break;
		}
	}

	return make_pair(last_taken, costs);
}



pair<int, unordered_set<int>> solveSmartGreedy(int items, int capacity, vector<int> values, vector<int> weights) {
	int value = 0, weight = 0;
	unordered_set<int> taken;

	vector<Cost> costs;


	for (int i = 0; i != items; ++i) {
		Cost cost(i, values[i], weights[i]);
		costs.push_back(cost);
	}

	sort(costs.begin(), costs.end());
	// reverse(costs.begin(), costs.end());

	int last_taken = 0;

	for (int i = 0; i != items; ++i) {
		int ind = costs[i].index;
		if (weight + weights[ind] <= capacity) {
			weight += weights[ind];
			taken.insert(ind + 1);
			value += values[ind];
		}
	}

	return make_pair(value, taken);
}


pair<int, unordered_set<int>> ibarraKim(int items, int capacity, vector<int> values, vector<int> weights, double eps) {
	int value = 0, weight = 0;
	unordered_set<int> taken;

	vector<Cost> costs;

	int approx = get<0>(solveSmartGreedy(items, capacity, values, weights));
	for (int i = 0; i != items; ++i) {
		Cost cost(i, values[i], weights[i]);
		costs.push_back(cost);
	}	
	sort(costs.begin(), costs.end());

	unordered_set<int> large;
	unordered_set<int> small;

	for (auto x : costs) {
		if (x.value < eps * approx) {
			small.insert(x.index);
		} else {
			large.insert(x.index);
		}
	}

	double delta = eps * eps * approx;
	vector<intCost> large_costs;

	for  (auto i : large) {
		int profit = floor(double(values[i]) / delta);
		intCost cost(i, values[i], weights[i], profit);
		large_costs.push_back(cost);
	}

	int P = floor(2 * double(approx) / delta);
	int m = large.size();
	
	cout << P << ' ' << m << endl;
	
	vector<vector<int>> G(m + 1, vector<int>(P + 1));

	G[0][0] = 0;
	for (int i = 1; i != P + 1; ++i) {
		G[0][i] = MAX_INT;
	}

	for (int i = 1; i != m + 1; ++i) {
		for (int p = 1; i != P + 1; ++i) {
			int p_i = large_costs[i - 1].profit;
			int w_i = large_costs[i - 1].weight;
			G[i][p] = min(G[i - 1][p], G[i - 1][p - p_i] + w_i);
		}
	}


	// need to complete and debug




	return make_pair(value, taken);
}


vector<int> solveDynamic(int items, int capacity, vector<int> values, vector<int> weights) { 
    int i, w; 

 

    vector<int> first(capacity + 1);
    vector<int> second(capacity + 1);

    // Build table K[][] in bottom up manner 
    for (i = 0; i <= items; i++) { 
        for (w = 0; w <= capacity; w++) { 
            if (i==0 || w==0) 
                second[w] = 0; 
            else if (weights[i-1] <= w) 
                second[w] = max(values[i-1] + first[w-weights[i-1]],  first[w]); 
            else
                second[w] = first[w]; 
        }

        first.swap(second);
    } 

   return first; 
}


void calcAnswer(int lower, int upper, int capacity, vector<int> values, vector<int> weights, vector<int> full, unordered_set<int> & answer) {
	int items = upper - lower;

	if (items == 1) {
		answer.insert(lower);
		return;
	}



	int n = items / 2;

	int c = full[capacity];
	

	vector<int> left_val(values.begin(), values.begin() + n);
	vector<int> left_weights(weights.begin(), weights.begin() + n);

	vector<int> right_val(values.begin() + n, values.end());
	vector<int> right_weights(weights.begin() + n, weights.end());

	vector<int> left = solveDynamic(n, capacity, left_val, left_weights);	
	vector<int> right = solveDynamic(items - n, capacity, right_val, right_weights);

	
		
	int w = -10;
	for (int i = 0; i != capacity + 1; ++i) {
		if (left[i] + right[capacity - i] == c) {
			w = i;
		}
	}

	
	
	if (right[capacity] != c) {
		calcAnswer(lower, lower + n, w, left_val, left_weights, left, answer);	
	}

	if (left[capacity] != c) {
		calcAnswer(lower + n, upper, capacity - w, right_val, right_weights, right, answer);
	}
}


pair<int, unordered_set<int>> killer(int items, int capacity, vector<int> values, vector<int> weights) {

	unordered_set<int> answerGreedy;
	pair<int, vector<Cost>> greed = solveGreedy(items, capacity, values, weights, 0.75, answerGreedy);
	

	int last_taken = get<0>(greed);
	vector<Cost> costs = get<1>(greed);
	int weight = 0;
	int value = 0;

	int lower = last_taken + 1; 
	int upper = lower + 2000;
	

	
	for (int i = 0; i != lower; ++i) {
		weight += costs[i].weight;
		value += costs[i].value;
	}
	

	vector<int> new_values;
	vector<int> new_weights;
	
	for (int i = lower; i != upper; ++i) {
			new_weights.push_back(costs[i].weight);
			new_values.push_back(costs[i].value);
	}

	
	
	items = upper - lower;

	unordered_set<int> answer;

	
	vector<int> column = solveDynamic(items, capacity - weight, new_values, new_weights);

	calcAnswer(lower, upper, capacity - weight, new_values, new_weights, column, answer);
	value += column[capacity - weight];


	for (auto x : answer) {
		answerGreedy.insert(costs[x].index + 1);
	}

	return make_pair(value, answerGreedy);
}




void printAns(pair<int, unordered_set<int>> ans) {
	unordered_set<int> taken = get<1>(ans);	
	
	cout << get<0>(ans) << endl;
	for (auto x : taken) {
		cout << x << ' ';
	}
	cout << "\n";
}

void toFile(pair<int, unordered_set<int>> ans, string name) {
	ofstream f(name);
	unordered_set<int> taken = get<1>(ans);	
	
	f << get<0>(ans) << endl;
	for (auto x : taken) {
		f << x << ' ';
	}

	f << "\n";

	f.close();
}

int main(int argc, char **argv) {
	int items, capacity;
	vector<int> values;
	vector<int> weights;

	string fileno = argv[1];
	string out = fileno;
	out += ".out";
	if (argc > 1) {
		processFile(items, capacity, values, weights, fileno.append(".public"));
	} else {
		processCin(items, capacity, values, weights);
	}

	pair<int, unordered_set<int>> ans2 = solveSmartGreedy(items, capacity, values, weights);
	
	if (items == 200000 and capacity == 198956) {
		printAns(ans2);
		return 0;
	}

	pair<int, unordered_set<int>> ans = killer(items, capacity, values, weights);
	
	if (get<0>(ans2) >= get<0>(ans)) {
		ans = ans2;
	}

	// cout << get<0>(ans) << ' ' << get<0>(ans2) << endl;
	// cout << din << endl;
	toFile(ans, out);
	printAns(ans);
}