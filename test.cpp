#include "factor.h"
#include "myAlgorithm.h"
#include <algorithm>
#include <iterator>
#include <set>
#include <time.h>

using namespace std;

static inline bool cmpfirst(pair<int, int> p1, pair<int, int> p2) {
	return p1.first < p2.first;
}

// scope union:
factor::scope operator+(const factor::scope &s1, const factor::scope &s2) {
	factor::scope ret;
	set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),
		inserter(ret, ret.begin()), cmpfirst);
	return ret;
}

void sprint(const factor::scope s1){
	for (factor::scope::const_iterator it1 = s1.begin(); it1 != s1.end(); it1++)
		cout << it1->first << " ";
	cout << endl;
}

// The following parts are my samples.
void sample1(std::map<int, factor> &factors, std::vector<int> &order){
	enum { C = 0, D = 1, I = 2, T = 3, G = 4, S = 5, L = 6, J = 7 };

	factor::scope cvar; cvar[C] = 2;
	factor::scope dvar; dvar[D] = 2;
	factor::scope ivar; ivar[I] = 2;
	factor::scope tvar; tvar[T] = 2;
	factor::scope gvar; gvar[G] = 3;
	factor::scope svar; svar[S] = 2;
	factor::scope lvar; lvar[L] = 2;
	factor::scope jvar; jvar[J] = 2;

	factor::scope c = cvar;
	factor::scope d = cvar + dvar;
	factor::scope i = ivar;
	factor::scope t = ivar + tvar;
	factor::scope g = dvar + tvar + gvar;
	factor::scope s = tvar + svar;
	factor::scope l = gvar + lvar;
	factor::scope j = svar + lvar + jvar;

	factor pc(c), pd(d), pi(i), pt(t), pg(g), ps(s), pl(l), pj(j);

	factors.insert(pair<int, factor>(C, pc));
	factors.insert(pair<int, factor>(D, pd));
	factors.insert(pair<int, factor>(I, pi));
	factors.insert(pair<int, factor>(T, pt));
	factors.insert(pair<int, factor>(G, pg));
	factors.insert(pair<int, factor>(S, ps));
	factors.insert(pair<int, factor>(L, pl));
	factors.insert(pair<int, factor>(J, pj));

	order.push_back(C);
	order.push_back(I);
	order.push_back(D);
	order.push_back(T);
	
	order.push_back(G);	//if we marginalize S firstly, then we will get a mode with less cliques but larger clique size
	order.push_back(S);
	order.push_back(L);
	order.push_back(J);
}

void sample2(std::map<int, factor> &factors, std::vector<int> &order){
	enum { C = 0, D = 1, I = 2, T = 3, G = 4, S = 5, L = 6, J = 7 };

	factor::scope cvar; cvar[C] = 2;
	factor::scope dvar; dvar[D] = 2;
	factor::scope ivar; ivar[I] = 2;
	factor::scope tvar; tvar[T] = 2;
	factor::scope gvar; gvar[G] = 3;
	factor::scope svar; svar[S] = 2;
	factor::scope lvar; lvar[L] = 2;
	factor::scope jvar; jvar[J] = 2;

	factor::scope c = cvar;
	factor::scope d = cvar + dvar;
	factor::scope i = ivar;
	factor::scope t = tvar + gvar + jvar;
	factor::scope g = dvar + ivar + gvar;
	factor::scope s = ivar + svar;
	factor::scope l = gvar + lvar;
	factor::scope j = svar + lvar + jvar;

	factor pc(c), pd(d), pi(i), pt(t), pg(g), ps(s), pl(l), pj(j);

	factors.insert(pair<int, factor>(C, pc));
	factors.insert(pair<int, factor>(D, pd));
	factors.insert(pair<int, factor>(I, pi));
	factors.insert(pair<int, factor>(T, pt));
	factors.insert(pair<int, factor>(G, pg));
	factors.insert(pair<int, factor>(S, ps));
	factors.insert(pair<int, factor>(L, pl));
	factors.insert(pair<int, factor>(J, pj));

	order.push_back(C);
	order.push_back(D);
	order.push_back(I);
	order.push_back(T);
	order.push_back(G);
	order.push_back(S);
	order.push_back(L);
	order.push_back(J);
}

void sample3(std::map<int, factor> &factors, std::vector<int> &order){
	enum { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5 };

	factor::scope avar; avar[A] = 2;
	factor::scope bvar; bvar[B] = 2;
	factor::scope cvar; cvar[C] = 2;
	factor::scope dvar; dvar[D] = 2;
	factor::scope evar; evar[E] = 3;
	factor::scope fvar; fvar[F] = 2;


	factor::scope a = avar;
	factor::scope b = bvar;
	factor::scope c = cvar;
	factor::scope d = dvar;
	factor::scope e = evar;
	factor::scope f = fvar + avar + bvar + cvar + dvar + evar;


	factor pa(a), pb(b), pc(c), pd(d), pe(e), pf(f);
	factors.insert(pair<int, factor>(A, pa));
	factors.insert(pair<int, factor>(B, pb));
	factors.insert(pair<int, factor>(C, pc));
	factors.insert(pair<int, factor>(D, pd));
	factors.insert(pair<int, factor>(E, pe));
	factors.insert(pair<int, factor>(F, pf));

	order.push_back(A);
	order.push_back(B);
	order.push_back(C);
	order.push_back(D);
	order.push_back(E);
	order.push_back(F);

}


int main(){
	std::map<int, factor> factors;
	std::vector<int> order;
	//sample1(factors, order);
	sample2(factors, order);
	//sample3(factors, order);
	// sample 4:
	// sample 5:

	std::vector<factor::scope> nodes; 
	std::multimap<int, int> adj;

	clock_t begin, end;
	double time;
	begin = clock();	

	//for (int i = 0; i < 1; i++){
	//	std::vector<factor::scope> nodes1;
	//	std::multimap<int, int> adj1;
	//	VariableElimaination(factors,order,nodes, adj);

	//	//ChordalGraph(factors, order, nodes1, adj1);
	//	nodes = nodes1;
	//	adj = adj1;
	//}
	//VariableElimaination(factors, order, nodes, adj);

	ChordalGraph(factors, order, nodes, adj);



	end = clock();
	time = (double)(end - begin) / CLOCKS_PER_SEC;
	cout << "running time is: " << time << " s" << endl;
	cout << "clique nodes" << endl;
	for (int i = 0; i < nodes.size(); i++){
		cout << i <<": ";
		for (factor::scope::iterator iter = nodes[i].begin(); iter != nodes[i].end(); iter++){
			cout << iter->first<<" ";
		}
		cout << endl;
	}
	cout << "edges" << endl;
	if (adj.empty())
		cout << "empty" << endl;
	for (multimap<int, int>::iterator iter = adj.begin(); iter != adj.end(); iter++){
		cout << "<" << iter->first << "," << iter->second << ">" << endl;
	}
	system("pause");
}