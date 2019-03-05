#include <iostream>
#include <string>
#include <vector>
#include <set>

#include<tdzdd/DdSpecOp.hpp>
#include<tdzdd/DdStructure.hpp>

using namespace tdzdd;


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

int main(int argc, char **argv) {

int n=7, W=18;
int w[]={4, 3, 7, 5, 6, 8, 10};

	Knapsack knapsack(n, w, W);
	DdStructure<2> dd(knapsack); // construct ZDD

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

