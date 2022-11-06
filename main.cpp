#include "NetworkLayout.hpp"
#include "writerUMC.hpp"
#include "writerSimple.hpp"
#include "ParserXML.hpp"
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <memory>

#include <unordered_map>
#include <string>
#include <cstring>
#include <exception>

#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <memory>

using namespace rapidxml;
using namespace std;

unique_ptr<writer> writer::write(fileType type){
    if(type == 0) {
        auto uniqueP = make_unique<writerUMC>();
        return uniqueP;
    }
    if(type == 1){
        auto uniqueP = make_unique<writerSimple>();
        return uniqueP;
    }
    return nullptr;
}

int choose(const string &s,const string& s1,const string& s2){
    int combined;
    do{
        cout << s << endl;
        cout << "0 : " << s1 << endl;
        cout << "1 : " << s2 << endl;
        cin >> combined;
    }while( combined != 0 && combined != 1);
    return combined;
}

void createFolder(const string &s1,string &outputFile, std::stringstream &ss){
    try {
        outputFile = outputFile + s1;
        cout << outputFile << endl;
        outputFile +=  + "/";
        ss.str("");
        ss.clear();
        ss <<  outputFile;
        int folder = mkdir(ss.str().c_str(),S_IRWXU | S_IRWXG);
        if(folder == 0) std::cout << "Created " << ss.str() << " success\n";
        }
    catch (const std::exception &e){
        
        std::cerr << e.what();
    }

}

int ParserXML::count = 0;

int main(int argc,const char *argv[]){
    string input;
    string outputFile;
    bool helpCalled = false;
    
    for(int i = 0; i < argc; i++){
        string stringInput = argv[i];
        if(stringInput == "-i"){
            if(argc <= i+1 && !helpCalled) 
                throw invalid_argument("There must be an input file");
            input = argv[i+1];
        }
        if(stringInput == "-o" && argc > i+1)
            outputFile = argv[i+1];
        
        if(stringInput == "-h"){
            helpCalled = true;
            break;
        }
    }
    if(input == "" && !helpCalled)
        throw invalid_argument("There must be an input file");
    if (helpCalled){
        cout << "-i <inputfile> ";
        cout << "Provides the input xml file that should be parsed" << endl;
        cout << "-o <outputfile> ";
        cout << "Provides the output text which the program should write the config data"<<endl;
        cout << "This remove the printout in the terminal and is optional"<< endl;
        cout << "-h ";
        cout << "Shows this info" << endl;

        exit(0);

    }

    auto pXML = make_unique<ParserXML>(input);
    std::stringstream ss;
    createFolder(pXML->SplitFilename(input),outputFile,ss);
    int file = choose("WHICH TYPE DO YOU WANT", "UMCFile", "SimpleFile");
    if(file == 0){
        auto obj = writer::write(UMCFile);
        createFolder("UMC",outputFile,ss);
        int train = choose("HOW MANY TRAIN DO YOU WANT?", "ONE", "TWO") + 1; // TRAIN CONFIG CAN NOT 0 //
        bool correct = false;
        int select = 0;
        while(!correct){
            switch(train){
                case 1:{
                    correct = true;
                    auto combined = choose("DO YOU WANT TO COMBINE ROUTE WITH LENGTH < 4 ?", "YES", "NO");
                    if(combined == 0){
                        createFolder("SingleRoutesCombined",outputFile, ss);
                        Interlock il_comb = pXML->getIl().routeCombiner(pXML->getIl());
                        pXML->setIl(il_comb);  
                        obj->writeFile(outputFile,pXML,train,1);
                    }
                    else{
                        select = choose("DO YOU WANT SELECT ONE SPECIFIC ROUTE?","YES", "NO");
                        if(select != 0 ){
                            createFolder("SingleRoute",outputFile,ss);
                            obj->writeFile(outputFile,pXML,train,select);
                        }else{
                            int route1;
                            int size = (int)pXML->getIl().getRoutes().size();
                            do{
                                cout << "SELECT ROUTE 1 BETWEEN 0 AND " << size << " : "; 
                                cin >> route1;
                            }while(route1 < 0 && route1 >= size );
                            createFolder("SingleRouteSelected",outputFile,ss);
                            obj->writeFile(outputFile,pXML,train,select,route1); 
                        }
                    }
                }break;
                case 2: {
                    correct = true;
                    select = choose("DO YOU WANT SELECT THE TWO ROUTES?", "YES", "NO");
                    if(select != 0){
                        createFolder("DoubleRoute",outputFile, ss);
                        obj->writeFile(outputFile,pXML,train,select);
                    }else{
                        int route1;
                        int route2;
                        auto size = (int)pXML->getIl().getRoutes().size();
                        do{
                            cout << "SELECT ROUTE 1 BETWEEN 0 AND " << size << " : "; 
                            cin >> route1;
                        }while(route1 > size );
                        do{
                            cout << "SELECT ROUTE 2 BETWEEN 0 AND " << size << " : "; 
                            cin >> route2;
                        }while(route2 > size && route2 != route1);
                        createFolder("DoubleRouteSelected",outputFile,ss);
                        obj->writeFile(outputFile,pXML,train,select,route1,route2);
                    }
                }break;
                default: 
                    cerr << "Number of trains incorrect. Please select '1' or '2'. " << endl;
                    cin >> train;
            }
        } 
    }
    if(file == 1){
        auto obj = writer::write(SimpleFile);
        createFolder("Simple",outputFile,ss);
        obj->writeFile(outputFile,pXML);
    }
    
}

