#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include <tdzdd/DdSpecOp.hpp>
#include <tdzdd/DdStructure.hpp>
#include <tdzdd/DdEval.hpp>


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

class Knapsack: public tdzdd::DdSpec<Knapsack, int, 2> {
  int const n;
  int const *w;
  int const W;
public:
  Knapsack(int n, int *w, int W)
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

struct MaxGainData {
	int val;
	int sets;
	};

class MaxGain: public tdzdd::DdEval<MaxGain,MaxGainData> {
	int const n;
	int const *g;
public:
	MaxGain(int n, int *g) : n(n), g(g) {
	}
	void evalTerminal(MaxGainData& data, bool one) const {
		data.val = one ? 0 : INT_MIN;
		data.sets = 0;
	}
	void evalNode(MaxGainData& data, int level, tdzdd::DdValues<MaxGainData,2> const& values) const {
		const MaxGainData& data0 = values.get(0);
		const MaxGainData& data1 = values.get(1);
		if(data0.val >= data1.val + g[n-level]){
			data.val = data0.val;
			data.sets = data0.sets;
		}else{
			data.val = data1.val + g[n-level];
			data.sets = data1.sets | (1<<level);
		}				
	}
};

int main(int argc, char **argv){

int n=7, W=18, k=3;
int w[]={4, 3, 7, 5, 6, 8, 10};
int g[]={7, 2, 8, 3, 6, 9, 5};

	Knapsack knapsack(n, w, W);
	Combination combination(n, k);
	tdzdd::ZddIntersection<Knapsack, Combination> intersection = tdzdd::zddIntersection(knapsack, combination);
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

        	std::cout << "Node Count = " << dd.size() << std::endl;
        	std::cout << "Cardinality = " << dd.evaluate(ZddCardinality<>()) << std::endl;
			MaxGain gain = MaxGain(n, g);
			MaxGainData ans = dd.evaluate(gain);
			std::cout << "Maximum Gain = " << ans.val << std::endl;
			std::cout << "Yielding Set = {";
			int items = ans.sets;
			for(int i=n, j=0; i>0 && j<k; i--){
				if( (items & (1<<i)) != 0 ){
					if(j != 0) std::cout << ", ";
					std::cout << "(" << w[n-i] << ", " << g[n-i] << ")";
					j++;
				}
			}
			std::cout << "}" << std::endl;
	}
	return 0;
}
