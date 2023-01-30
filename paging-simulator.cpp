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
#include <unordered_set>
#include <stdlib.h> 

    using namespace std;

    // total size of local storage
    int RAM_CAPACITY;

    // percentage of total RAM_CAPCITY for looking ahead in prefetching 
    int factor;

    //all defined latencies are in ns
    int LOCAL_ACCESS_LATENCY = 15; //page exists in local memory
    int REMOTE_ACCESS_LATENCY = 10000; // page is feteched from far memory
    int DIRTY_PAGE_EVICTION_LATENCY = 10000; // writing back dirty page(page with store operation)
    int CLEAN_PAGE_EVICTION_LATENCY = 0; // writing back clean page(no store opertation)

    std::vector<pair<std::string, bool>> pages; //list of pages <pageName, isStore(bool)>
    std::unordered_set<std::string>mySet; //for tracking unique pages
    
    //local storage 
    extern std::vector<pair<std::string, pair<int, bool>>> localStorage; //<pageName, <expectedTime(int), isStore(bool)>>
    extern std::vector<int> tracker; //temp stoarge to compute differences in distance in occurences

    //global trackers
    int globalTime; //independent track of time 

    //program trackers
    int totalTime ; //sum of all times spent on program execution (without prefetech operations)
    int totalLocalAccess; //total time spent on local accesses
    int totalRemoteAccess; // total time spent on remote(far memory) access
    int totalEvictionDirty; // total time spent to write dirty page back
    int totalEvictionClean;// total time spent to write clean page back
    int totalReplacements; // total number of replacements made

    // prefetch trackers
    int totalTimePrefetch ; // sum of all times spent for prefetching
    int totalRemoteAccessPrefetch; //total time spent on local access during prefetching
    int totalEvictionDirtyPrefetch; // total time spent to write dirty page back during prefetching
    int totalReplacementsPrefetch; // total number of replacements made during prefetching
    int totalStalls; //total time spent waiting for pages to be loaded from far memory when they are not in local memory

    
  

    std::string MODE ="orcl";

//calculates the distance until the next occurenece
int calculateNextDistance (int currentIndex, std::string page){
       int distance = 0;
        for (int  j = currentIndex ;  j < pages.size(); j++){
            if(pages[j].first == page ){
                break;
            }else{
                distance +=1;
            }
        }
       return distance;
    }
//calculates distance from previous occurence
int calculatePrevDistance (int currentIndex, std::string page){
       int distance = 0;
        for (int  j = currentIndex ;  j > 0; j--){
            if(pages[j].first == page ){
                break;
            }else{
                distance +=1;
            }
        }
       return distance;
    }


//prints contents in localstorage
void printLocalStorage ( std::vector<pair<std::string, pair<int, bool>>>& localStorage){
    for ( int k = 0 ;  k < RAM_CAPACITY; k++){
                cout << "("<<localStorage[k].first << "-" << localStorage[k].second.first << ")" << " " ;
            }
    cout<< "" << endl;
}

//replacement based on knowledge of future accesses
void oracle (int currentIndex, std::vector<pair<std::string, pair<int, bool>>>& localStorage, std::vector<int> &tracker){
    //calculate distance to next occurence of every page in local storage
    for (int  z = 0 ;  z < RAM_CAPACITY; z++){
        int dist = calculateNextDistance(currentIndex, localStorage[z].first);
        tracker[z] = dist;
    }   
                
    //replacement
    auto maxDist =  std::max_element(tracker.begin(),tracker.end()) - tracker.begin();
    if(localStorage[maxDist].second.second){
        //dirty page evicition
        globalTime  += (DIRTY_PAGE_EVICTION_LATENCY + REMOTE_ACCESS_LATENCY);
        totalTime += (DIRTY_PAGE_EVICTION_LATENCY + REMOTE_ACCESS_LATENCY);
        totalEvictionDirty +=DIRTY_PAGE_EVICTION_LATENCY;
        //replacement
        totalRemoteAccess +=REMOTE_ACCESS_LATENCY;
        localStorage [maxDist].first = pages [currentIndex].first ;
        localStorage [maxDist].second.second = pages [currentIndex].second ;
    }else{
        //clean page evicition
        globalTime +=(REMOTE_ACCESS_LATENCY + CLEAN_PAGE_EVICTION_LATENCY);
        totalTime +=(REMOTE_ACCESS_LATENCY + CLEAN_PAGE_EVICTION_LATENCY);
        totalEvictionClean += CLEAN_PAGE_EVICTION_LATENCY;
        //replacement
        totalRemoteAccess += REMOTE_ACCESS_LATENCY;
        localStorage [maxDist].first = pages [currentIndex].first ;
        localStorage [maxDist].second.second = pages [currentIndex].second ;
    }
    
    totalReplacements +=1;
    // cout<< "replaced " <<endl ;
}

void oraclePrefetch (int currentIndex, int prefetchIndex, std::vector<pair<std::string, pair<int, bool>>>& localStorage, std::vector<int> &tracker){
   //calculate distance to next occurence of every page in local storage
   for (int  z = 0 ;  z < RAM_CAPACITY; z++){
        int dist = calculateNextDistance(currentIndex, localStorage[z].first);
        tracker[z] = dist;
    }   
                
    //replacement
    auto maxDist =  std::max_element(tracker.begin(),tracker.end()) - tracker.begin();
    if(localStorage[maxDist].second.second){
        //dirty page evicition
        totalTimePrefetch += (DIRTY_PAGE_EVICTION_LATENCY + REMOTE_ACCESS_LATENCY);
        totalEvictionDirtyPrefetch +=DIRTY_PAGE_EVICTION_LATENCY;
        totalRemoteAccessPrefetch +=REMOTE_ACCESS_LATENCY;
        //replacement
        localStorage [maxDist].first = pages [prefetchIndex].first ;
        localStorage [maxDist].second.second = pages [currentIndex].second ;
        //set ETA
        localStorage [maxDist].second.first = globalTime + DIRTY_PAGE_EVICTION_LATENCY + REMOTE_ACCESS_LATENCY;
    }else{
        //clean page evicition
        totalTimePrefetch +=(REMOTE_ACCESS_LATENCY + CLEAN_PAGE_EVICTION_LATENCY);
        //// totalEvictionClean += CLEAN_PAGE_EVICTION_LATENCY;
        //replacement
        totalRemoteAccessPrefetch += REMOTE_ACCESS_LATENCY;
        localStorage [maxDist].first = pages [prefetchIndex].first ;
        localStorage [maxDist].second.second = pages [currentIndex].second ;
        //set ETA
        localStorage [maxDist].second.first = globalTime + REMOTE_ACCESS_LATENCY + CLEAN_PAGE_EVICTION_LATENCY;
    }
    
    totalReplacementsPrefetch +=1;
    // cout<< "replaced " <<endl ;
}


void leastRecentlyUsed (int currentIndex, std::vector<pair<std::string, pair<int, bool>>>& localStorage ,std::vector<int> &tracker){
     //calculate distance to previous occurenece of every page in local storage
    for (int  z = 0 ;  z < RAM_CAPACITY; z++){
        int dist = calculatePrevDistance(currentIndex, localStorage[z].first);
        tracker[z] = dist;
    }   
                
    //replacement
    auto maxDist =  std::max_element(tracker.begin(),tracker.end()) - tracker.begin();
    
    if(localStorage[maxDist].second.second){
        //dirty page evicition
         globalTime  += (DIRTY_PAGE_EVICTION_LATENCY + REMOTE_ACCESS_LATENCY);
        totalTime +=(DIRTY_PAGE_EVICTION_LATENCY +REMOTE_ACCESS_LATENCY);
        totalEvictionDirty +=DIRTY_PAGE_EVICTION_LATENCY;
        //replacement
        totalRemoteAccess +=REMOTE_ACCESS_LATENCY;
        localStorage [maxDist].first = pages [currentIndex].first ;
        localStorage [maxDist].second.second = pages [currentIndex].second ;
    }else{
        //clean page evicition
        globalTime +=(REMOTE_ACCESS_LATENCY + CLEAN_PAGE_EVICTION_LATENCY);
        totalTime +=(CLEAN_PAGE_EVICTION_LATENCY+ REMOTE_ACCESS_LATENCY);
        totalEvictionClean +=CLEAN_PAGE_EVICTION_LATENCY;
        //replacement
        totalRemoteAccess +=REMOTE_ACCESS_LATENCY;
        localStorage [maxDist].first = pages [currentIndex].first ;
        localStorage [maxDist].second.second = pages [currentIndex].second ;
    }

     totalReplacements +=1;
    // cout<< "replaced " <<endl ;
}

void printStats(std::string currently, int ram_cap){
    
    cout<< currently << " - RAM CAPACITY = " << ram_cap<< endl ;
    cout << "Time : " << globalTime / 1000000.0  << " ms"<<endl;
    cout << "----------------------------------"<< endl;
    cout << "Total Stall : " << totalStalls / 1000000.0  << " ms"<<endl;
    cout << "Total No of Replacements : " <<  totalReplacements <<endl;
    cout << "Total Time : " << totalTime / 1000000.0  << " ms"<<endl;
    cout << "Total Time for Local Accesses : " << totalLocalAccess/ 1000000.0  << " ms"<<endl;
    cout << "Total Time for Remote Accesses : " << totalRemoteAccess/ 1000000.0  << " ms"<<endl;
    cout << "Total Time for Evicting Dirty Pages : " << totalEvictionDirty/ 1000000.0  << " ms"<<endl;
    cout << "----------------------------------"<< endl;
    cout << "Total No of Replacements Prefetching : " <<  totalReplacementsPrefetch <<endl;
    cout << "Total Time Prefetching : " << totalTimePrefetch / 1000000.0  << " ms"<<endl;
    cout << "Total Time for Remote Accesses Prefetching : " << totalRemoteAccessPrefetch/ 1000000.0  << " ms"<<endl;
    cout << "Total Time for Evicting Dirty Pages Prefetching : " << totalEvictionDirtyPrefetch/ 1000000.0  << " ms"<<endl;
    cout << "\n " <<endl;
   
}   
//simulation function
void runSimulation(){

    globalTime = 0;

    // reset program trackers
    totalTime  = 0;
    totalLocalAccess = 0;
    totalRemoteAccess = 0;
    totalEvictionDirty = 0;
    totalEvictionClean = 0;
    totalReplacements = 0;

    // reset prefetch trackers
    totalTimePrefetch  = 0;
    totalRemoteAccessPrefetch = 0;
    totalEvictionDirtyPrefetch = 0;
    totalReplacementsPrefetch = 0;
    totalStalls = 0;

    //init data structures
    std::vector<pair<std::string, pair<int, bool>>> localStorage(RAM_CAPACITY);
    std::vector<int> tracker(RAM_CAPACITY);

    //choose modes 
    if(MODE == "orcl" || MODE =="lru"){
  
    for (int i=0; i < pages.size(); i++){
        
        bool isInLocal = false ;

        //check if page is in local storage
        for (int  j = 0 ;  j < RAM_CAPACITY; j++){
            if(localStorage[j].first == pages[i].first ){
                localStorage[j].second.second = pages[i].second;
                isInLocal=true;
                break;
            }
        }
        //local access
        if (isInLocal){
            globalTime +=LOCAL_ACCESS_LATENCY;
            totalTime += LOCAL_ACCESS_LATENCY;
            totalLocalAccess +=LOCAL_ACCESS_LATENCY;
            // printLocalStorage (localStorage);
            continue ;
        }else{
            //not local access
            bool isFull;
            int emptyIdx = -1;
            //check if there is empty slot in local storage
            for (int  b = 0 ;  b < RAM_CAPACITY; b++){
                if(localStorage[b].first.empty() ){
                        emptyIdx = b;
                        break; 
                }
            }
            if(emptyIdx == -1) {
                //no empty slot
                isFull = true;
            }else{
                //found empty slot
                //fetch from remote and add to local storage - update trackers accordingly
                localStorage[emptyIdx].first =pages[i].first;
                localStorage[emptyIdx].second.second =pages[i].second;
                globalTime += REMOTE_ACCESS_LATENCY;
                totalTime += REMOTE_ACCESS_LATENCY;
                totalRemoteAccess +=REMOTE_ACCESS_LATENCY;
                //printLocalStorage (localStorage);
                continue;
            }
            //local sotrage full- no empty slot
            if(isFull){
                //replace page based on selected mode
                if(MODE =="orcl"){
                    oracle(i, localStorage, tracker);
                }else  if(MODE =="lru"){
                    leastRecentlyUsed(i, localStorage, tracker);
                } 
              
               
                
                // printLocalStorage (localStorage);
                continue;

            }
        }
            
            
    }
    }
    else  if(MODE =="orclpref"){
        
   //prefetching mode

    for (int i=0; i < pages.size(); i++){

        //program executuion

        auto currentPage = pages[i];
        bool isCurrentInLocal = false ;


        int localIndex ;
        //check if current page is in local storage
        for (int  j = 0 ;  j < RAM_CAPACITY; j++){
            if(localStorage[j].first == currentPage.first ){
                localStorage[j].second.second = currentPage.second;
                localIndex = j; 
                isCurrentInLocal=true;
                break;
            }
        }
        //current page in local stroage
        if (isCurrentInLocal){
            int expectedtime  = localStorage[localIndex].second.first;
            if(globalTime < expectedtime){
                //not available yet --- stalling 
                int stall = abs(globalTime - expectedtime);
                // cout << "stall " << stall << endl;
                globalTime += stall;
                totalStalls += stall ;

            }
            //update trackers
            globalTime+= LOCAL_ACCESS_LATENCY;
            totalTime += LOCAL_ACCESS_LATENCY;
            totalLocalAccess +=LOCAL_ACCESS_LATENCY;
            
            // cout << "GT: " << globalTime << endl;
            //printLocalStorage (localStorage);

        }else{
            //current page not in local storage
            bool isFull;
            int emptyIdx = -1;
            //check if local storage has empty slot
            for (int  b = 0 ;  b < RAM_CAPACITY; b++){
                if(localStorage[b].first.empty() ){
                        emptyIdx = b;
                        break; 
                }
            }
            if(emptyIdx == -1) {
                //no empty slot
                isFull = true;
            }else{
                //empty slot found
                //fetch from remote and add to local storage - update trackers accordingly
                localStorage[emptyIdx].first =currentPage.first;
                localStorage[emptyIdx].second.second =currentPage.second;
                globalTime += REMOTE_ACCESS_LATENCY;
                totalTime += REMOTE_ACCESS_LATENCY;
                totalRemoteAccess +=REMOTE_ACCESS_LATENCY;
                // cout << "GT: " << globalTime << endl;
                //printLocalStorage (localStorage);
               
            }
            //no empty slot found
            if(isFull){
                //replace page based on oracle algorithm
                oracle(i, localStorage, tracker);
                // cout << "GT: " << globalTime << endl;
                // printLocalStorage (localStorage);

            }
        }
      
        //prefetching execution
        //check if in bound
          if((i+factor) < pages.size()){

        
        //get prefetch page
        auto prefetchPage = pages[i+factor];
      
        bool isPrefetchInLocal = false ;

        
         //check if prefetch page is in local storage
        for (int  j = 0 ;  j < RAM_CAPACITY; j++){
            if(localStorage[j].first == prefetchPage.first ){
                localStorage[j].second.second = prefetchPage.second;
                isPrefetchInLocal=true;
                break;
            }
        }

       
        //preftech page found in local stroage
         if (isPrefetchInLocal){
            // cout << "prefetch in local" << endl;
            continue ;
        }else{
            //prefetch page not found in local storage
            bool isFull;
            int emptyIdx = -1;
            for (int  b = 0 ;  b < RAM_CAPACITY; b++){
                //check for emoty slot
                if(localStorage[b].first.empty() ){
                        emptyIdx = b;
                        break; 

                }
            }
            //no empty slot
            if(emptyIdx == -1) {
                isFull = true;
            }else{
                //empty slot found
                //fetech prefetch page from remote memory and update trackers accordingly
                localStorage[emptyIdx].first =prefetchPage.first;
                localStorage[emptyIdx].second.second =prefetchPage.second;
                localStorage[emptyIdx].second.first = globalTime + REMOTE_ACCESS_LATENCY;
                // cout << "prefeteched from remote" << endl;
                totalTimePrefetch += REMOTE_ACCESS_LATENCY;
                totalRemoteAccessPrefetch +=REMOTE_ACCESS_LATENCY;
                // cout << "GT: " << globalTime << endl;
                //printLocalStorage (localStorage);
                continue;
            }
            //no empty slot found
            if(isFull){
                // cout << "prefetch replace" << endl; 
                //repalce page with oracle prefetch algorithm
                oraclePrefetch(i, i+factor, localStorage, tracker);
                
                continue;

            }
        }
        }
            
            
    }            
                
    }
}

 

int main(int argc, char **argv)
{
    

    //get mode from console input
    MODE = argv[1];
    //read from page mapper output
    std::ifstream infile("pageMapperList.out");
    std::string line;
    
    //reading from file
    while (std::getline(infile, line))
    {
        bool isStore;
        if(line[0]== '*'){
            isStore = true;
        }

        auto pageString = line.substr(1, line.length());
        //add to mySet and pages structures
        mySet.insert(pageString);
        pages.push_back({pageString, isStore});
        
    }
    //total number of pages
    auto pagesSize = pages.size();
    //total number of unique pages
    auto unique = mySet.size();
    //percentages of total unique pages - used to set RAM_CAPACITY during simulation
    auto fivepercent = std::ceil (unique *0.05);
    auto onefourth = std::ceil(unique * 0.25);
    auto half = std::ceil(unique * 0.5);
    auto threefourth = std::ceil(unique * 0.75);
    auto full = unique;

    //make factor calculation

    cout<< "MODE : " << MODE << endl;
    cout<< "Total No of Pages : " << pagesSize << endl;
    cout<< "No of Unique Pages : " << mySet.size() << endl;
  
    factor = std::ceil (full*7.5);

    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = fivepercent;
    // runSimulation();
    // printStats("5%", fivepercent);

     

    // factor = std::ceil (full*0.5);

    // cout << "-------------------------------mySet------------------------"<< endl;
    // RAM_CAPACITY = fivepercent;
    // runSimulation();
    // printStats("5%", fivepercent);


    // factor = std::ceil (full);

    // cout << "-------------------------------mySet------------------------"<< endl;
    // RAM_CAPACITY = fivepercent;
    // runSimulation();
    // printStats("5%", fivepercent);


    // factor = std::ceil (full*2.5);

    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = fivepercent;
    // runSimulation();
    // printStats("5%", fivepercent);


    // factor = std::ceil (full*5);

    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = fivepercent;
    // runSimulation();
    // printStats("5%", fivepercent);


    // factor = std::ceil (full*7.5);

    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = fivepercent;
    // runSimulation();
    // printStats("5%", fivepercent);

    // factor = std::ceil (full*10);

    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = fivepercent;
    // runSimulation();
    // printStats("5%", fivepercent);

    // factor = std::ceil (full*15);

    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = fivepercent;
    // runSimulation();
    // printStats("5%", fivepercent);

    // factor = std::ceil (full*20);

    cout << "-------------------------------------------------------"<< endl;
    RAM_CAPACITY = fivepercent;
    runSimulation();
    printStats("5%", fivepercent);
    cout << "-------------------------------------------------------"<< endl;
    RAM_CAPACITY = onefourth;
    runSimulation();
    printStats("25%", onefourth);
    cout << "-------------------------------------------------------"<< endl;
    RAM_CAPACITY = half;
    runSimulation();
    printStats("50%", half);
    cout << "-------------------------------------------------------"<< endl;
    RAM_CAPACITY = threefourth;
    runSimulation();
    printStats("75%", threefourth);
    cout << "-------------------------------------------------------"<< endl;
    RAM_CAPACITY = full;
    runSimulation();
    printStats("100%", full);
    


    return 0;
}