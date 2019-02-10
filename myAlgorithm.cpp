#include "myAlgorithm.h"
#include <set>
#include <algorithm>
#include <iterator>

using namespace std;

static inline bool cmpfirst1(pair<int, int> p1, pair<int, int> p2) {
	return p1.first < p2.first;
}

// scope union:
factor::scope operator-(const factor::scope &s1, const factor::scope &s2) {
	factor::scope ret;
	set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),
		inserter(ret, ret.begin()), cmpfirst1);
	return ret;
}

bool is_in_unmarked(vector<int> unmarked_nodes, int name){
	bool flag = false;
	for (int i = 0; i < unmarked_nodes.size(); i++){
		if (unmarked_nodes[i] == name){
			flag = true;
			break;
		}
	}
	return flag;
}

int findnextunmarked(multimap<int,int> graph,vector<int> unmarked_nodes, vector<int> marked_nodes){
	vector<int> name;
	vector<int> weight;
	for (int i = 0; i < marked_nodes.size(); i++){
		int max_count = graph.count(marked_nodes[i]);
		multimap<int, int>::iterator iter = graph.find(marked_nodes[i]);
		for (int j = 0; j < max_count; j++, iter++){
			if (is_in_unmarked(unmarked_nodes, iter->second)){
				if (name.empty()){
					name.push_back(iter->second);
					weight.push_back(1);
				}
				else{
					for (int k = 0; k < name.size(); k++){
						if (name[k] == iter->second){
							weight[k]++;
						}
						else{
							name.push_back(iter->second);
							weight.push_back(1);
						}
					}
				}
			}
		}
	}
	int position = 0;
	int max = 0;
	for (int i = 0; i < weight.size(); i++){
		if (weight[i]>max){
			max = weight[i];
			position = i;
		}
	}
	
	if (!name.empty())
		return name[position];
	else
		return unmarked_nodes[0];
}

void VariableElimaination(std::map<int, factor> factors, std::vector<int> order, std::vector<factor::scope> &nodes,
		std::multimap<int, int> &adj){
	set<int> visited;			// record which factor was used, but I record positions here, not names
	set<int> visited_new;		// record which factor was used in newfactors, positions here as well
	vector<factor> newfactors;	// a new factor was produced after marginalization

	for (int i = 0; i < order.size(); i++){
		//	cout << "This is " << i << "th variable " << endl;
		factor::scope node;		// a clique
		int count = 0;
		for (map<int, factor>::iterator iter = factors.begin(); iter != factors.end(); iter++){
			if (visited.count(count) == 0){
				factor::scope sco = iter->second.getscope();
				for (factor::scope::iterator it = sco.begin(); it != sco.end(); it++){

					if (it->first == order[i]){
						node = node - iter->second.getscope();
						//cout << "original factors: ";
						//sprint(iter->second.getscope());
						visited.insert(count);
						break;
					}
				}
			}
			count++;
		}

		vector<int> add_edges;		// store all cliques that should have an edge with current clique

		for (int j = 0; j < newfactors.size(); j++){
			if (visited_new.count(j) == 0){
				factor::scope sco = newfactors[j].getscope();
				for (factor::scope::iterator it = sco.begin(); it != sco.end(); it++){

					if (it->first == order[i]){
						node = node - newfactors[j].getscope();
						//	cout << "new factors: ";
						//	sprint(newfactors[j].getscope());
						visited_new.insert(j);
						add_edges.push_back(j);
						break;
					}
				}
			}
		}

		// simulate elimination process, we neednot compute anything.
		bool is_not_a_subset = true;
		if (!node.empty()){
			// update clique tree. We use max clique here, so check whether current clique is a subset of others!
			for (int j = 0; j < nodes.size(); j++){
				factor::scope ret;
				set_intersection(nodes[j].begin(), nodes[j].end(), node.begin(), node.end(), inserter(ret, ret.begin()));
				if (ret.size() == node.size())
					is_not_a_subset = false;
			}
			if (is_not_a_subset)
				nodes.push_back(node);
			// simulate marginalization, push it into newfactor set
			node.erase(order[i]);
			factor f(node);
			newfactors.push_back(f);
		}

		if (is_not_a_subset){
			for (int j = 0; j < add_edges.size(); j++){
				adj.insert(make_pair(add_edges[j], nodes.size() - 1));
				adj.insert(make_pair(nodes.size() - 1, add_edges[j]));
			}
		}

	}
}

// This is the second algorithm. It has four steps
// Step 1: construct a undirected graph. use moral graph here
// Step 2: do Heuristic triangulation (VE) to get Chrodal graph 
// Step 3: find maximal cliques: maximum cardinality search
// Step 4: build clique tree using maximum spanning tree algorithm
void ChordalGraph(std::map<int, factor> factors, std::vector<int> order,
	std::vector<factor::scope> &nodes, std::multimap<int, int> &adj){
	
	// step 1: get moral graph
	// I use std::multimap to stroe edge info of graph. For ease, I store i->j && j->i for <i,j>
	multimap<int, int> graph;
	set<pair<int, int>> appear_graph;
	for (map<int, factor>::iterator iter = factors.begin(); iter != factors.end(); iter++){
		factor::scope current_scope = iter->second.getscope();
		vector<int> parents;
		for (factor::scope::iterator it = current_scope.begin(); it != current_scope.end(); it++){
			if (it->first != iter->first){
				parents.push_back(it->first);
				if (appear_graph.count(make_pair(iter->first, it->first)) == 0){
					graph.insert(make_pair(iter->first, it->first));
					appear_graph.insert(make_pair(iter->first, it->first));
				}
				if (appear_graph.count(make_pair(it->first, iter->first)) == 0){
					graph.insert(make_pair(it->first, iter->first));
					appear_graph.insert(make_pair(it->first, iter->first));
				}
			}
		}
		while (!parents.empty()){
			int last_parent = parents[parents.size() - 1];
			parents.pop_back();
			for (int i = 0; i < parents.size(); i++){
				if (appear_graph.count(make_pair(last_parent, parents[i])) == 0){
					graph.insert(make_pair(last_parent, parents[i]));
					appear_graph.insert(make_pair(last_parent, parents[i]));
				}
				if (appear_graph.count(make_pair(parents[i], last_parent)) == 0){
					graph.insert(make_pair(parents[i], last_parent));
					appear_graph.insert(make_pair(parents[i], last_parent));
				}
			}
		}
	}

	// Step 2: Heuristic Triangulation, using VE order
	set<int> visited;			// record which factor was used, but I record positions here, not names
	set<int> visited_new;		// record which factor was used in newfactors, positions here as well
	vector<factor> newfactors;	// a new factor was produced after marginalization
	for (int i = 0; i < order.size(); i++){
		factor::scope node;		// a clique
		int count = 0;
		for (map<int, factor>::iterator iter = factors.begin(); iter != factors.end(); iter++){
			if (visited.count(count) == 0){
				factor::scope sco = iter->second.getscope();
				for (factor::scope::iterator it = sco.begin(); it != sco.end(); it++){

					if (it->first == order[i]){
						node = node - iter->second.getscope();
						//cout << "original factors: ";
						//sprint(iter->second.getscope());
						visited.insert(count);
						break;
					}
				}
			}
			count++;
		}
		for (int j = 0; j < newfactors.size(); j++){
			if (visited_new.count(j) == 0){
				factor::scope sco = newfactors[j].getscope();
				for (factor::scope::iterator it = sco.begin(); it != sco.end(); it++){
					if (it->first == order[i]){
						node = node - newfactors[j].getscope();
						//	cout << "new factors: ";
						//	sprint(newfactors[j].getscope());
						visited_new.insert(j);
						break;
					}
				}
			}
		}
		if (!node.empty()){
			//  triangulartion: should be a complete graph
			vector<int> inte_scopes;
			for (factor::scope::iterator iter = node.begin(); iter != node.end(); iter++)
				inte_scopes.push_back(iter->first);
			while (!inte_scopes.empty()){
				int last_element = inte_scopes[inte_scopes.size() - 1];
				inte_scopes.pop_back();
				for (int i = 0; i < inte_scopes.size(); i++){
					if (appear_graph.count(make_pair(last_element, inte_scopes[i])) == 0){
						graph.insert(make_pair(last_element, inte_scopes[i]));
						appear_graph.insert(make_pair(last_element, inte_scopes[i]));
					}
					if (appear_graph.count(make_pair(inte_scopes[i], last_element)) == 0){
						graph.insert(make_pair(inte_scopes[i], last_element));
						appear_graph.insert(make_pair(inte_scopes[i], last_element));
					}
				}
			}
			// simulate VE
			node.erase(order[i]);
			factor f(node);
			newfactors.push_back(f);
		}
	}



	//for (multimap<int, int>::iterator it = graph.begin(); it != graph.end(); it++){
	//	cout << it->first << " " << it->second << endl;
	//}



	//cout << "Step3" << endl;
	//Step 3: find maximal cliques:  maximum cardinality search
	vector<vector<int>> clique_list;	// clique set
	vector<int> current_clique;			// to be a clique
	vector<int> unmarked_nodes = order;
	vector<int> marked_nodes;

	while (!unmarked_nodes.empty()){
		int next_unmarked = findnextunmarked(graph,unmarked_nodes,marked_nodes);
		//cout <<endl<< next_unmarked << " one "<<endl;

		// check whether fully connected
		// idea: current_clique must be fully connected, 
		// so we just test whether there is an edge between new node and all nodes in current_clique
		vector<int> current_clique_new;
		int count = 0;
		int max_count = graph.count(next_unmarked);
		multimap<int, int>::iterator iter = graph.find(next_unmarked);
		for (int i = 0; i < max_count; i++, iter++){
			for (int j = 0; j < current_clique.size(); j++){
				if (iter->second == current_clique[j])
					count++;
			}
			for (int j = 0; j < marked_nodes.size(); j++){
				if (iter->second == marked_nodes[j])
					current_clique_new.push_back(marked_nodes[j]);
			}
		}
		if (count == current_clique.size()){
			current_clique.push_back(next_unmarked);
		}else{
			clique_list.push_back(current_clique);
			current_clique_new.push_back(next_unmarked);
			current_clique = current_clique_new;
		}

		// update unmarked set and marked set
		for (int i = 0; i < unmarked_nodes.size(); i++){
			if (unmarked_nodes[i] == next_unmarked){
				swap(unmarked_nodes[i], unmarked_nodes[unmarked_nodes.size() - 1]);
				unmarked_nodes.pop_back();
			}
		}
		marked_nodes.push_back(next_unmarked);
	}

	// The lastcurrent_clique should also be pushed into list
	clique_list.push_back(current_clique);

	//cout << endl << "clique_list" << endl;
	//for (int i = 0; i < clique_list.size(); i++){
	//	for (int j = 0; j < clique_list[i].size(); j++){
	//		cout << clique_list[i][j] << "";
	//	}
	//	cout << endl;
	//}


	//cout << "Step4" << endl;
	// Step 4: maximum spanning tree:
	// initialize weight
	vector<vector<int>> weight;
	for (int i = 0; i < clique_list.size(); i++){
		vector<int> v_column;
		for (int j = 0; j < clique_list.size(); j++)
			v_column.push_back(0);
		weight.push_back(v_column);
	}
	for (int i = 0; i < clique_list.size(); i++){
		for (int j = 0; j < clique_list.size(); j++){
			if (i == j){
				weight[i][j] = 0;
			}else{
				vector<int> clique1 = clique_list[i];
				vector<int> clique2 = clique_list[j];
				vector<int> ret;
				sort(clique1.begin(), clique1.end());
				sort(clique2.begin(), clique2.end());
				set_intersection(clique1.begin(), clique1.end(), clique2.begin(), clique2.end(), back_inserter(ret));
				weight[i][j] = ret.size();
			}
		}
	}
	//cout << "init finished" << endl;
	set<int> in_a_tree;
	set<int> not_in_a_tree;
	for (int i = 0; i < clique_list.size(); i++)
		not_in_a_tree.insert(i);
	//cout << "set" << endl;
	if (clique_list.size()>1){
		while (!not_in_a_tree.empty()){
			// find next clique
			int max_weight = 0;
			int clique1;	// node1 && node2 has an edge in the tree
			int clique2;
			if (in_a_tree.empty()){
				for (int i = 0; i < clique_list.size(); i++){
					for (int j = 0; j < clique_list.size(); j++){
						if (weight[i][j]>max_weight){
							max_weight = weight[i][j];
							clique1 = i; clique2 = j;
						}
					}
				}
			}
			else{
				for (int i = 0; i < clique_list.size(); i++){
					if (in_a_tree.count(i) == 1){
						for (int j = 0; j < clique_list.size(); j++){
							if (in_a_tree.count(j) == 0 && weight[i][j]>max_weight){
								max_weight = weight[i][j];
								clique1 = i; clique2 = j;
							}
						}
					}
				}
			}
			in_a_tree.insert(clique1);
			in_a_tree.insert(clique2);
			adj.insert(make_pair(clique1, clique2));
			adj.insert(make_pair(clique2, clique1));
			not_in_a_tree.erase(clique1);
			not_in_a_tree.erase(clique2);
		}
	}
	
	//cout << "update clique tree" << endl;
	// update clique tree nodes
	for (int i = 0; i < clique_list.size(); i++){
		factor::scope scope_ret;
		for (int j = 0; j < clique_list[i].size(); j++){
			for (map<int, factor>::iterator it = factors.begin(); it != factors.end(); it++){
				factor::scope sco = it->second.getscope();
				for (factor::scope::iterator sit = sco.begin(); sit != sco.end(); sit++){
					if (clique_list[i][j] == sit->first){
						factor::scope tem; tem[sit->first] = sit->second;
						scope_ret = scope_ret - tem;
					}
				}
			}
		}
		nodes.push_back(scope_ret);
	}

}