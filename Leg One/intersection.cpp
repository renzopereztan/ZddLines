#include <iostream>
#include <string>
#include <vector>
#include <set>

#include <tdzdd/DdSpecOp.hpp>
#include <tdzdd/DdStructure.hpp>

using namespace tdzdd;

class Combination: public tdzdd::DdSpec<Combination,int,2> {
    int const n;
    int const k;
public:
    Combination(int n, int k)
            : n(n), k(k) {
    }
    int getRoot(int& state) const {
        state = 0;
        return n;
    }
    int getChild(int& state, int level, int value) const {
        state += value;
        if (--level == 0) return (state == k) ? -1 : 0;
        if (state > k) return 0;
        if (state + level < k) return 0;
        return level;
    }
};

class KnapsackZdd: public tdzdd::DdSpec<KnapsackZdd, int, 2> {
  int const n;
  int const *w;
  int const W;
public:
  KnapsackZdd(int n, int *w, int W)
    : n(n), w(w), W(W){
  }
  int getRoot(int& state) const{
    state = 0;
    return n;
  }
  int getChild(int& state, int level, int value)const{
    if(value == 1) state += w[n-level];
    level--;
    if(state > W) return 0;
    if(level == 0) return -1;
    return level;
  }
};

int main(int argc, char **argv){

int n=7, W=18, k=3;
int w[]={4, 3, 7, 5, 6, 8, 10};

	KnapsackZdd knapsack(n, w, W);
	Combination combination(n, k);
	tdzdd::ZddIntersection<KnapsackZdd, Combination> intersection = tdzdd::zddIntersection(knapsack, combination);
	DdStructure<2> dd(intersection); // construct ZDD

	if (argc >= 2 && std::string(argv[1]) == std::string("dump")) {

        	dd.dumpDot(std::cout, "ZDD");

    	} else if (argc >= 2 && std::string(argv[1]) == std::string("solutions")) {

       		for (DdStructure<2>::const_iterator itor1 = dd.begin(); itor1 != dd.end(); ++itor1) {
        	    const std::set<int>& s = *itor1;
        	    for (std::set<int>::const_iterator itor2 = s.begin(); itor2 != s.end(); ++itor2) {
        		    std::cout << *itor2 << " ";
        	    }
        	std::cout << std::endl;
        	}

   	} else {

        	std::cout << "Number of ZDD nodes = " << dd.size() << std::endl;
        	std::cout << "Number of elements = " << dd.evaluate(ZddCardinality<>()) << std::endl;

    	}
	return 0;
}
