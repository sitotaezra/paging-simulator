#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <locale>
#include <vector>
#include <limits.h>
#include <math.h>
#include <unordered_map>

using namespace std;
int PAGE_SIZE = getpagesize();

 
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                    { return !std::isspace(ch); }));
}

int main(int argc, char **argv)
{
    //parse page size from input
    PAGE_SIZE = std::stoi(argv[1]);
    //read from valgrind file
    std::ifstream infile("valgrind");
    std::string line;
    int addressSize = 0;
    string minHex;

    //initalize sequence structure
    vector<pair<std::string, pair<int, bool>>> sequence;
  

    //loop through each line
    while (std::getline(infile, line))
    {
    
        ltrim(line);
        //only consider load and store operations
        if (line[0] == 'L' || line[0] == 'S')
        {
            auto type = line.substr(0, line.find(" "));
            auto address = line.substr(line.find(" "), line.find(",") - 1);
            ltrim(address);

            //check if store operation
            bool isStrore = line[0]=='S' ? true :false;
            
            //map page
            int n;
            std::istringstream(address) >> std::hex >> n;
            int pageNo = n/PAGE_SIZE;
          
         
            std::ostringstream aa;
            aa << std::hex << pageNo;
            std::string result = aa.str();

            //add to list
            pair<std::string, pair<int, bool>> item = {result,{0, isStrore}};
                    sequence.push_back(item);
            

            // //compute consecutive occurences
            // if(sequence.size() == 0 ){
            //     pair<std::string, pair<int, bool>> item = {result,{0, isStrore}};
            //     sequence.push_back(item);
            // }else{
            //     if(sequence.back().first == result){
                
            //         sequence.back().second.first += 1;
            //     }else{
            //         pair<std::string, pair<int, bool>> item = {result,{0, isStrore}};
            //         sequence.push_back(item);

            //     }
            // }
            
          
        }

    }
 

    //optput formating    
    for (int i = 0; i < sequence.size(); i++)
    {
        std::string isStore;
        if (sequence[i].second.second) {
            isStore ='*';
        }else{
            isStore=' ';
        }
        // OCCUERENCE PRINT 
        // cout << isStore << "\t"<< sequence[i].first  << "\t"<< sequence[i].second.first +1  << endl;
    
        //LIST PRINT
       cout<< isStore << sequence[i].first <<endl;
    }


    return 0;
}