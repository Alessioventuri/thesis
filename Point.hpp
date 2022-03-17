#include "Section.hpp"
#include <string>

using namespace std;
class Point :  public Section{
private:
    int stemNeighbour;
    int plusNeighbour;
    int minusNeighbour;
public:
    Point(int &id, int &stem, int &plus, int &minus) : Section(id){
        stemNeighbour = stem;
		plusNeighbour = plus;
		minusNeighbour = minus;
    }
    string toString(map<int,string>);
    string toString();
    int getStem(){ return stemNeighbour;}
    int getPlus(){ return plusNeighbour;}
    int getMinus(){ return minusNeighbour;}
    int getSectionId(){return sectionId;}
    
};

