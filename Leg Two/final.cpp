// general preamble
#include <iostream>
#include <string>
#include <vector>
#include <set>
// TdZdd-specific preamble
#include <DdSpecOp.hpp>
#include <DdStructure.hpp>
#include <util/Graph.hpp>
#include <util/IntSubset.hpp>
#include <spec/DegreeConstraint.hpp>
#include <spec/FrontierBasedSearch.hpp>

using namespace tdzdd;
using namespace std;

// finding the path with maximum total distance
struct MaxDistData {
	// storing two data types for each node
	int val1;
	// both below are to accommodate the 125 edges	
	unsigned long long sets1;
	unsigned long long sets2;
	};

class MaxDist: public tdzdd::DdEval<MaxDist,MaxDistData> {
	// n is the number of edges	
	int const n;
	// d is the distance vector	
	int const *d;
public:
	MaxDist(int n, int *d) : n(n), d(d) {
	}
	void evalTerminal(MaxDistData& data, bool one) const {
		// initializing
		data.val1 = one ? 0 : INT_MIN;
		data.sets1 = 0;
		data.sets2 = 0;
	}
	void evalNode(MaxDistData& data, int level, tdzdd::DdValues<MaxDistData,2> const& values) const {
		// principal declaration
		const MaxDistData& data0 = values.get(0);
		const MaxDistData& data1 = values.get(1);
		// general logic for finding the maximum
		if(data0.val1 >= data1.val1 + d[n-level]){
			data.val1 = data0.val1;
			data.sets1 = data0.sets1;
			data.sets2 = data0.sets2;
		}else{
			if(level <= 64){
				// in this case, the first data set with capacity 64 is altered
				data.val1 = data1.val1 + d[n-level];
				data.sets1 = data1.sets1 | (1llu <<(level-1));
				data.sets2 = data1.sets2;
			}else{
				// in this case, the second data set is altered
				data.val1 = data1.val1 + d[n-level];
				data.sets1 = data1.sets1;
				data.sets2 = data1.sets2 | (1llu <<(level-65));
			}
		}				
	}
};

// finding the path with minimum total distance
struct MinDistData {
	// storing two data types for each node
	int val2;
	// both below are to accommodate the 125 edges
	unsigned long long sets3;
	unsigned long long sets4;
	};

class MinDist: public tdzdd::DdEval<MinDist,MinDistData> {
	// n is the number of edges	
	int const n;
	// d is the distance vector
	int const *d;
public:
	MinDist(int n, int *d) : n(n), d(d) {
	}
	void evalTerminal(MinDistData& data, bool one) const {
		// initializing
		// avoided INT_MAX to prevent overflow 
		data.val2 = one ? 0 : 100000;
		data.sets3 = 0;
		data.sets4 = 0;
	}
	void evalNode(MinDistData& data, int level, tdzdd::DdValues<MinDistData,2> const& values) const {
		// principal declaration
		const MinDistData& data0 = values.get(0);
		const MinDistData& data1 = values.get(1);
		// a minor tweak to the codes for the maximum
		if(data0.val2 <= data1.val2 + d[n-level]){
			data.val2 = data0.val2;
			data.sets3 = data0.sets3;
			data.sets4 = data0.sets4;
		}else{
			if(level <= 64){
				// in this case, the first data set with capacity 64 is altered 
				data.val2 = data1.val2 + d[n-level];
				data.sets3 = data1.sets3 | (1llu <<(level-1));
				data.sets4 = data1.sets4;
			}else{
				// in this case, the second data set is altered
				data.val2 = data1.val2 + d[n-level];
				data.sets3 = data1.sets3;
				data.sets4 = data1.sets4 | (1llu <<(level-65));
			}
		}				
	}
};


// setting up the constraint of using each line at least once
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
    // setting up degree constraint
    tdzdd::DegreeConstraint dc(graph);

    std::string oneStr("1");
    char ss[256];
    sprintf(ss, "%d", graph.vertexSize());
    std::string nStr(ss);

    // assume start vertex is station 1 and end vertex is station 41
    for (int v = 1; v <= graph.vertexSize(); ++v) {
        if (graph.vertexName(v) == oneStr || graph.vertexName(v) == string("41")) {
            dc.setConstraint(v, &justOne);
        }
        else {
            dc.setConstraint(v, &zeroOrTwo);
        }
    }
    // the diagram representing all paths
    tdzdd::FrontierBasedSearch fbs(graph, 1, false, false);

    // in this case, there are 125 edges and 9 lines
    int n=125, L=9;
    // line assignment in accord with data
    int l[]={7,7,7,7,5,5,1,1,3,3,5,5,5,4,1,3,3,4,4,4,4,4,9,9,9,9,9,9,9,9,9,3,3,3,3,3,7,5,6,1,6,6,6,5,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,4,6,7,7,2,4,2,6,6,2,2,3,3,1,1,1,1,1,1,1,2,7,7,4,5,5,5,5,5,5,8,4,4,4,4,6,6,2,2,7,7,7,7,7,7,7,8,8,2,2,2,2,2,8,8,8,2,2,8,8,8,8};
    // weight assignment
    int d[]= {6,10,6,5,9,11,9,10,10,9,10,13,6,11,12,6,6,15,13,16,15,24,6,7,7,5,15,8,6,13,12,17,11,13,11,13,7,7,9,12,15,7,10,10,8,9,6,11,10,10,17,14,13,12,12,18,13,13,12,19,16,15,10,7,10,6,4,9,10,10,9,8,9,18,9,13,10,18,7,9,20,9,13,12,13,10,13,11,15,9,10,9,11,13,11,14,18,12,6,7,8,10,11,7,17,12,11,12,13,12,9,13,13,8,11,10,18,10,9,12,18,12,9,13,14};  
 
    Lines lines(n, L, l);

    // to display computation time
    tdzdd::MessageHandler::showMessages();

    // the ZDD for all paths in general
    DdStructure<2> dd1(tdzdd::zddIntersection(dc, fbs));

    // the ZDD for all feasible paths
    DdStructure<2> dd(tdzdd::zddIntersection(dd1, lines));

    // printing output ZDD
    if (argc >= 3 && std::string(argv[2]) == std::string("dump")) {
        dd.dumpDot(std::cout, "ZDD");

    // enumerating the solutions	
    } else if (argc >= 3 && std::string(argv[2]) == std::string("solutions")) {

        for (tdzdd::DdStructure<2>::const_iterator itor1 = dd.begin(); itor1 != dd.end(); ++itor1) {
            const std::set<int>& s = *itor1;
            for (std::set<int>::const_iterator itor2 = s.begin(); itor2 != s.end(); ++itor2) {
		// note that the level of the diagram is top-down greatest to least                
		std::cout << 125-*itor2 << " ";
            }
            std::cout << std::endl;
        }

    } else {

        std::cout << "Number of ZDD nodes = " << dd.size() << std::endl;
        std::cout << "Number of elements = " << dd.evaluate(tdzdd::ZddCardinality<>()) << std::endl;
			// maximum distance output			
			MaxDist dist1 = MaxDist(n, d);
			MaxDistData ans1 = dd.evaluate(dist1);
			std::cout << "Maximum Distance = " << ans1.val1 << std::endl;
			std::cout << "Yielding Path = (";
			unsigned long long items1 = ans1.sets1;
			unsigned long long items2 = ans1.sets2;
			// searching for path yielding maximum
			// iterating through first 64 edges			
			for(int i=0, j=0; i<64; i++){
				if( (items1 & (1llu<<i)) != 0 ){
					if(j != 0) std::cout << " ";
					std::cout << n-i-1;
					j++;
				}
			}
			std::cout<< " ";
			// iterating through the remaining 61 edges
			for(int p=64, q=0; p<125; p++){ 
				if( (items2 & (1llu<<(p-64))) != 0 ){
					if(q != 0) std::cout << " ";
					std::cout << n-p-1;
					q++;
				}
			}
			std::cout << ")" << std::endl;
			// minimum distance output			
			MinDist dist2 = MinDist(n, d);
			MinDistData ans2 = dd.evaluate(dist2);
			std::cout << "Minimum Distance = " << ans2.val2 << std::endl;
			std::cout << "Yielding Path = (";
			unsigned long long items3 = ans2.sets3;
			unsigned long long items4 = ans2.sets4;	
			// searching for path yielding minimum
			// iterating through first 64 edges		
			for(int i=0, j=0; i<64; i++){
				if( (items3 & (1llu<<i)) != 0 ){
					if(j != 0) std::cout << " ";
					std::cout << n-i-1;
					j++;
				}
			}
			std::cout<< " ";
			// iterating through the remaining 61 edges
			for(int p=64, q=0; p<125; p++){ 
				if( (items4 & (1llu<<(p-64))) != 0 ){
					if(q != 0) std::cout << " ";
					std::cout << n-p-1;
					q++;
				}
			}
			std::cout << ")" << std::endl;
    }
    return 0;
}