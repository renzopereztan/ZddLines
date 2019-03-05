
#include <iostream>
#include <string>
#include <vector>
#include <set>

#include <DdSpecOp.hpp>
#include <DdStructure.hpp>
#include <util/Graph.hpp>
#include <util/IntSubset.hpp>
#include <spec/DegreeConstraint.hpp>
#include <spec/FrontierBasedSearch.hpp>

//#include "Graph.hpp"

using namespace tdzdd;
using namespace std;

class Lines: public tdzdd::DdSpec<Lines, int, 2> {
	// n is the number of edges
	// L is the number of lines
	int const n, L;
	// l is the line the edge is part of
	int const *l;
public:
  Lines(int n, int L, int *l)
    : n(n), L(L), l(l){
  }
  int getRoot(int& state) const{
    state = 0;
    return n;
  }
  int getChild(int& state, int level, int value)const{
    if(value == 1) state |= (1<<l[n-level]);
    level--;
    if(level == 0){
		if(state == ((1<<L)-1)<<1){
			return -1;
		}else{
			return 0;
		}
	}
    return level;
  }
};

int main(int argc, char *argv[]) {
   if (argc <= 1) {
        std::cout << "usage: " << argv[0] << " <graph_filename>" << std::endl;
        return 0;
    }
    tdzdd::Graph graph;
    graph.readEdges(std::string(argv[1]));

    tdzdd::IntRange zeroOrTwo(0, 2, 2);
    tdzdd::IntRange justOne(1, 1);
    tdzdd::DegreeConstraint dc(graph);

    std::string oneStr("1");
    char ss[256];
    sprintf(ss, "%d", graph.vertexSize());
    std::string nStr(ss);

    // assume s = 1, and t = (maximum vertex number)
    for (int v = 1; v <= graph.vertexSize(); ++v) {
        if (graph.vertexName(v) == oneStr || graph.vertexName(v) == string("41")) {
            dc.setConstraint(v, &justOne);
        }
        else {
            dc.setConstraint(v, &zeroOrTwo);
        }
    }

    tdzdd::FrontierBasedSearch fbs(graph, 1, false, false);

    // This is a ZDD representing all the s-t paths
    int n=125, L=9;
    int l[]={7,7,7,7,5,5,1,1,3,3,5,5,5,4,1,3,3,4,4,4,4,4,9,9,9,9,9,9,9,9,9,3,3,3,3,3,7,5,6,1,6,6,6,5,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,4,6,7,7,2,4,2,6,6,2,2,3,3,1,1,1,1,1,1,1,2,7,7,4,5,5,5,5,5,5,8,4,4,4,4,6,6,2,2,7,7,7,7,7,7,7,8,8,2,2,2,2,2,8,8,8,2,2,8,8,8,8};
    
    Lines lines(n, L, l);
    DdStructure<2> dd(tdzdd::zddIntersection(tdzdd::zddIntersection(dc, fbs), lines));
    if (argc >= 3 && std::string(argv[2]) == std::string("dump")) {

        dd.dumpDot(std::cout, "ZDD");

    } else if (argc >= 3 && std::string(argv[2]) == std::string("solutions")) {

        for (tdzdd::DdStructure<2>::const_iterator itor1 = dd.begin(); itor1 != dd.end(); ++itor1) {
            const std::set<int>& s = *itor1;
            for (std::set<int>::const_iterator itor2 = s.begin(); itor2 != s.end(); ++itor2) {
                std::cout << 125-*itor2 << " ";
            }
            std::cout << std::endl;
        }

    } else {

        std::cout << "Number of ZDD nodes = " << dd.size() << std::endl;
        std::cout << "Number of elements = " << dd.evaluate(tdzdd::ZddCardinality<>()) << std::endl;
    }
    return 0;
}
