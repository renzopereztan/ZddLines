
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
        if (graph.vertexName(v) == oneStr || graph.vertexName(v) == nStr) {
            dc.setConstraint(v, &justOne);
        }
        else {
            dc.setConstraint(v, &zeroOrTwo);
        }
    }

    tdzdd::FrontierBasedSearch fbs(graph, 1, false, false);

    // This is a ZDD representing all the s-t paths
    tdzdd::MessageHandler::showMessages();
    tdzdd::DdStructure<2> dd(tdzdd::zddIntersection(dc, fbs)); 

    if (argc >= 3 && std::string(argv[2]) == std::string("dump")) {

        dd.dumpDot(std::cout, "ZDD");

    } else if (argc >= 3 && std::string(argv[2]) == std::string("solutions")) {

        for (tdzdd::DdStructure<2>::const_iterator itor1 = dd.begin(); itor1 != dd.end(); ++itor1) {
            const std::set<int>& s = *itor1;
            for (std::set<int>::const_iterator itor2 = s.begin(); itor2 != s.end(); ++itor2) {
                std::cout << *itor2 << " ";
            }
            std::cout << std::endl;
        }

    } else {

        std::cout << "Number of ZDD nodes = " << dd.size() << std::endl;
        std::cout << "Number of elements = " << dd.evaluate(tdzdd::ZddCardinality<>()) << std::endl;

    }
    return 0;
}
