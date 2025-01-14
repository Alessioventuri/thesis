#include "Interlock.hpp"
#include <iostream>
#include <algorithm>

using namespace std;

	
void Interlock::addRoute(const Route &rou){
	routes.push_back(rou);
}

void Interlock::deleteRoute(int i){
	routes.erase(routes.begin()+i);
}
	
void Interlock::setMaxValues(const int &maxpath) {
		maxPathLength = maxpath;

}

void Interlock::getCheckPointsSize(int i){
	if (maxChunk < (int)routes.at(i).getCheckPoints().size()){
			maxChunk = (int)routes.at(i).getCheckPoints().size();
		}
}

void Interlock::generateMaxChunk(){
		for(auto route : routes){
			if (maxChunk < (int)route.getCheckPoints().size()){
				maxChunk = (int)route.getCheckPoints().size();
			}
		}

}
string Interlock::toString() {
	generateMaxChunk();
	if (routes.empty() && maxChunk == 0 && maxPathLength == 0) 
		return "";
	string output = "value\n";
	
	output += "maxRoutes: Int = " + to_string(routes.size()) + ",\n" +
			"maxPathLength: Int = " + to_string(maxPathLength) + ",\n" +
			"maxChunks: Int = " + to_string(maxChunk-1) + "\n";
	output += "axiom\n";
	for ( int i = 0 ; i < (int)routes.size(); i++ )
		output += routes.at(i).toString(maxPathLength, maxChunk) + ",\n";
	if (output.length() > 0)
		output = output.substr(0, output.length() - 2);
	return output;
}
string Interlock::toStringCombiner() {
	generateMaxChunk();
	if (routes.empty() && maxChunk == 0 && maxPathLength == 0) 
		return "";
	string output = "value\n";
	
	output += "maxRoutes: Int = " + to_string(routes.size()) + ",\n" +
			"maxPathLength: Int = " + to_string(maxPathLength) + ",\n" +
			"maxChunks: Int = " + to_string(maxChunk-1) + "\n";
	output += "axiom\n";
	for(auto route : routes)
		output += route.toString(maxPathLength,maxChunk) + ",\n";
	if (output.length() > 0)
		output = output.substr(0, output.length() - 2);
	return output;
}

std::map<int,bool> Interlock::signalsCombiner(std::map<int,bool> s1, std::map<int,bool> s2){
    for(int i = 0; i < (int)s1.size();i++){
        if(s1.at(i) == false and s2.at(i) == true)
            s1.at(i) = s2.at(i);
    }
    return s1; 
}

std::map<int,bool> Interlock::conflictCombiner(std::map<int,bool> m1,std::map<int,bool>m2){
    for(int i = 0; i < (int)m2.size();i++){
        if(m1.at(i)== true and m2.at(i) == false)
            m2.at(i) = true;
    }
    return m2;
}

std::map<int,std::string> Interlock::pointsCombiner(std::map<int,std::string> s1, std::map<int,std::string> s2){
    for(int i = 0; i < (int)s1.size();i++){
        if(s1.at(i) != s2.at(i))
            s1.at(i) = s2.at(i);
    }
    return s1;
}


std::vector<int> Interlock::pathCombiner(std::vector<int> v1, std::vector<int> v2){
    std::vector<int> path;
    for(int i = 0; i < (int)v1.size();i++){
        path.push_back(v1.at(i));
    }
    for(int i = 1; i < (int)v2.size();i++){
        path.push_back(v2.at(i));
    }
    return path;
}

std::vector<bool> Interlock::overlapCombiner(std::vector<bool> v1, std::vector<bool> &v2){
    return v1;
}

Interlock Interlock::routeCombiner(Interlock il){
    Interlock new_Il;
    auto size = (int)il.getRoutes().size();
    for(int i = 0; i < size;i++){
        if(il.getRoutes().at(i).getPath().size() < 4){
            for(int j = 0; j < size;j++){ 
                if(il.getRoutes().at(i).getDest() == il.getRoutes().at(j).getSrc() && il.getRoutes().at(i).getDirection() == il.getRoutes().at(j).getDirection()){
                    Route route1(il.getRoutes().at(i));
                    Route route2(il.getRoutes().at(j));
                    int id = route2.getRouteId()*100+route1.getRouteId();
                    Route routeCombined(id,
                            route1.getSrc(),
                            route2.getDest(),
                            route1.getDirection(),
                            pointsCombiner(route1.getPoints(),route2.getPoints()),
                            pathCombiner(route1.getPath(),route2.getPath()),
                            signalsCombiner(route1.getSignals(),route2.getSignals()),
                            overlapCombiner(route1.getOverlap(),route2.getOverlap()),
                            conflictCombiner(route1.getConflict(),route2.getConflict()),
                            (int)route1.getPoints().size());
                    new_Il.addRoute(routeCombined);
                }
            }   
        }
    }
    int length = il.getMaxPathLength()+3;
    new_Il.setMaxValues(length);
    return new_Il;
}

	