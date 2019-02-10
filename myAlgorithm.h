#ifndef MYALGORITHM_H
#define MYALGORITHM_H

#include "factor.h"


using namespace std;

void VariableElimaination(std::map<int, factor> factors, std::vector<int> order, std::vector<factor::scope> &nodes,
		std::multimap<int, int> &adj);
void ChordalGraph(std::map<int, factor> factors, std::vector<int> order,
	std::vector<factor::scope> &nodes, std::multimap<int, int> &adj);

#endif // !MYALGORITHM_H
