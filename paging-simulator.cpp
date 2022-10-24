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


    int RAM_CAPACITY;

    int factor;

    //all defined latencies are in ns
    int LOCAL_ACCESS_LATENCY = 15;
    int REMOTE_ACCESS_LATENCY = 10000;
    int DIRTY_PAGE_EVICTION_LATENCY = 10000;
    int CLEAN_PAGE_EVICTION_LATENCY = 0;

    std::vector<pair<std::string, bool>> pages;
    std::unordered_set<std::string>mySet;
    
        
    extern std::vector<pair<std::string, pair<int, bool>>> localStorage;
    extern std::vector<int> tracker;

    //global trackers
    int globalTime;

    //program trackers
    int totalTime ;
    int totalLocalAccess;
    int totalRemoteAccess;
    int totalEvictionDirty;
    int totalEvictionClean;
    int totalReplacements;

    // prefetch trackers
    int totalTimePrefetch ;
    int totalRemoteAccessPrefetch;
    int totalEvictionDirtyPrefetch;
    int totalReplacementsPrefetch;
    int totalStalls;

    //hit-miss trackers
    int totalDemandHits;
    int totalDemandMisses;
    int totalPrefetchHits;
    int totalPreftechMisses;    
  

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



void printLocalStorage ( std::vector<pair<std::string, pair<int, bool>>>& localStorage){
    for ( int k = 0 ;  k < RAM_CAPACITY; k++){
                cout << "("<<localStorage[k].first << "-" << localStorage[k].second.first << ")" << " " ;
            }
    cout<< "" << endl;
}

void oracle (int currentIndex, std::vector<pair<std::string, pair<int, bool>>>& localStorage, std::vector<int> &tracker){
    for (int  z = 0 ;  z < RAM_CAPACITY; z++){
        int dist = calculateNextDistance(currentIndex, localStorage[z].first);
        tracker[z] = dist;
    }   
                
    //replacement
    auto maxDist =  std::max_element(tracker.begin(),tracker.end()) - tracker.begin();
    if(localStorage[maxDist].second.second){
        globalTime  += (DIRTY_PAGE_EVICTION_LATENCY + REMOTE_ACCESS_LATENCY);
        totalTime += (DIRTY_PAGE_EVICTION_LATENCY + REMOTE_ACCESS_LATENCY);
        totalEvictionDirty +=DIRTY_PAGE_EVICTION_LATENCY;
        totalRemoteAccess +=REMOTE_ACCESS_LATENCY;
        localStorage [maxDist].first = pages [currentIndex].first ;
        localStorage [maxDist].second.second = pages [currentIndex].second ;
    }else{
        globalTime +=(REMOTE_ACCESS_LATENCY + CLEAN_PAGE_EVICTION_LATENCY);
        totalTime +=(REMOTE_ACCESS_LATENCY + CLEAN_PAGE_EVICTION_LATENCY);
        totalEvictionClean += CLEAN_PAGE_EVICTION_LATENCY;
        totalRemoteAccess += REMOTE_ACCESS_LATENCY;
        localStorage [maxDist].first = pages [currentIndex].first ;
        localStorage [maxDist].second.second = pages [currentIndex].second ;
    }
    
    totalReplacements +=1;
    // cout<< "replaced " <<endl ;
}

void oraclePrefetch (int currentIndex, int prefetchIndex, std::vector<pair<std::string, pair<int, bool>>>& localStorage, std::vector<int> &tracker){
   for (int  z = 0 ;  z < RAM_CAPACITY; z++){
        int dist = calculateNextDistance(currentIndex, localStorage[z].first);
        tracker[z] = dist;
    }   
                
    //replacement
    auto maxDist =  std::max_element(tracker.begin(),tracker.end()) - tracker.begin();
    if(localStorage[maxDist].second.second){
        totalTimePrefetch += (DIRTY_PAGE_EVICTION_LATENCY + REMOTE_ACCESS_LATENCY);
        totalEvictionDirtyPrefetch +=DIRTY_PAGE_EVICTION_LATENCY;
        totalRemoteAccessPrefetch +=REMOTE_ACCESS_LATENCY;
        localStorage [maxDist].first = pages [prefetchIndex].first ;
        localStorage [maxDist].second.second = pages [currentIndex].second ;
        localStorage [maxDist].second.first = globalTime + DIRTY_PAGE_EVICTION_LATENCY + REMOTE_ACCESS_LATENCY;
    }else{
        totalTimePrefetch +=(REMOTE_ACCESS_LATENCY + CLEAN_PAGE_EVICTION_LATENCY);
        // totalEvictionClean += CLEAN_PAGE_EVICTION_LATENCY;
        totalRemoteAccessPrefetch += REMOTE_ACCESS_LATENCY;
        localStorage [maxDist].first = pages [prefetchIndex].first ;
        localStorage [maxDist].second.second = pages [currentIndex].second ;
        localStorage [maxDist].second.first = globalTime + REMOTE_ACCESS_LATENCY + CLEAN_PAGE_EVICTION_LATENCY;
    }
    
    totalReplacementsPrefetch +=1;
    // cout<< "replaced " <<endl ;
}

// void demandMIN (int currentIndex, int prefetchIndex, std::vector<pair<std::string, pair<int, bool>>>& localStorage, std::vector<int> &tracker){
//    for (int  z = 0 ;  z < RAM_CAPACITY; z++){
//         int dist = calculateNextDistance(currentIndex, localStorage[z].first);
//         tracker[z] = dist;
//     }   
                
//     //replacement
//     auto maxDist =  std::max_element(tracker.begin(),tracker.end()) - tracker.begin();
//     if(localStorage[maxDist].second.second){
//         totalTimePrefetch += (DIRTY_PAGE_EVICTION_LATENCY + REMOTE_ACCESS_LATENCY);
//         totalEvictionDirtyPrefetch +=DIRTY_PAGE_EVICTION_LATENCY;
//         totalRemoteAccessPrefetch +=REMOTE_ACCESS_LATENCY;
//         localStorage [maxDist].first = pages [prefetchIndex].first ;
//         localStorage [maxDist].second.second = pages [currentIndex].second ;
//         localStorage [maxDist].second.first = globalTime + DIRTY_PAGE_EVICTION_LATENCY + REMOTE_ACCESS_LATENCY;
//     }else{
//         totalTimePrefetch +=(REMOTE_ACCESS_LATENCY + CLEAN_PAGE_EVICTION_LATENCY);
//         // totalEvictionClean += CLEAN_PAGE_EVICTION_LATENCY;
//         totalRemoteAccessPrefetch += REMOTE_ACCESS_LATENCY;
//         localStorage [maxDist].first = pages [prefetchIndex].first ;
//         localStorage [maxDist].second.second = pages [currentIndex].second ;
//         localStorage [maxDist].second.first = globalTime + REMOTE_ACCESS_LATENCY + CLEAN_PAGE_EVICTION_LATENCY;
//     }
    
//     totalReplacementsPrefetch +=1;
//     // cout<< "replaced " <<endl ;
// }

void leastRecentlyUsed (int currentIndex, std::vector<pair<std::string, pair<int, bool>>>& localStorage ,std::vector<int> &tracker){
    for (int  z = 0 ;  z < RAM_CAPACITY; z++){
        int dist = calculatePrevDistance(currentIndex, localStorage[z].first);
        tracker[z] = dist;
    }   
                
    //replacement
    auto maxDist =  std::max_element(tracker.begin(),tracker.end()) - tracker.begin();
    
    if(localStorage[maxDist].second.second){
         globalTime  += (DIRTY_PAGE_EVICTION_LATENCY + REMOTE_ACCESS_LATENCY);
        totalTime +=(DIRTY_PAGE_EVICTION_LATENCY +REMOTE_ACCESS_LATENCY);
        totalEvictionDirty +=DIRTY_PAGE_EVICTION_LATENCY;
        totalRemoteAccess +=REMOTE_ACCESS_LATENCY;
        localStorage [maxDist].first = pages [currentIndex].first ;
        localStorage [maxDist].second.second = pages [currentIndex].second ;
    }else{
        globalTime +=(REMOTE_ACCESS_LATENCY + CLEAN_PAGE_EVICTION_LATENCY);
        totalTime +=(CLEAN_PAGE_EVICTION_LATENCY+ REMOTE_ACCESS_LATENCY);
        totalEvictionClean +=CLEAN_PAGE_EVICTION_LATENCY;
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
    cout << "----------------------------------"<< endl;
    cout << "Total No of Demand Hits : " <<  totalDemandHits <<endl;
    cout << "Total No of Demand Misses : " <<  totalDemandMisses <<endl;
    cout << "Total No of Prefetch Hits : " <<  totalPrefetchHits <<endl;
    cout << "Total No of Prefetch Misses : " <<  totalPreftechMisses <<endl;

   cout << "\n " <<endl;
   
}   

void runSimulation(){

    globalTime = 0;
    
    //program trackers
    totalTime  = 0;
    totalLocalAccess = 0;
    totalRemoteAccess = 0;
    totalEvictionDirty = 0;
    totalEvictionClean = 0;
    totalReplacements = 0;

    // prefetch trackers
    totalTimePrefetch  = 0;
    totalRemoteAccessPrefetch = 0;
    totalEvictionDirtyPrefetch = 0;
    totalReplacementsPrefetch = 0;
    totalStalls = 0;

    totalDemandHits =0 ;
     totalDemandMisses =0 ;
     totalPrefetchHits =0 ;
     totalPreftechMisses =0 ;    

    std::vector<pair<std::string, pair<int, bool>>> localStorage(RAM_CAPACITY);
    std::vector<int> tracker(RAM_CAPACITY);
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

        if (isInLocal){
            globalTime +=LOCAL_ACCESS_LATENCY;
            totalTime += LOCAL_ACCESS_LATENCY;
            totalLocalAccess +=LOCAL_ACCESS_LATENCY;
            // printLocalStorage (localStorage);
            continue ;
        }else{
            bool isFull;
            int emptyIdx = -1;
            for (int  b = 0 ;  b < RAM_CAPACITY; b++){
                if(localStorage[b].first.empty() ){
                        emptyIdx = b;
                        break; 
                }
            }
            if(emptyIdx == -1) {
                isFull = true;
            }else{
                localStorage[emptyIdx].first =pages[i].first;
                localStorage[emptyIdx].second.second =pages[i].second;
                globalTime += REMOTE_ACCESS_LATENCY;
                totalTime += REMOTE_ACCESS_LATENCY;
                totalRemoteAccess +=REMOTE_ACCESS_LATENCY;
                //printLocalStorage (localStorage);
                continue;
            }

            if(isFull){
                if(MODE =="orcl"){
                    oracle(i, localStorage, tracker);
                }else  if(MODE =="lru"){
                    leastRecentlyUsed(i, localStorage, tracker);
                } 
              
               
                
                //printLocalStorage (localStorage);
                continue;

            }
        }
            
            
    }
    }
    else  if(MODE =="orclpref"){
        
   

    for (int i=0; i < pages.size(); i++){

        

        auto currentPage = pages[i];
        cout << "Current page:" << currentPage.first << endl;
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

        if (isCurrentInLocal){
            int expectedtime  = localStorage[localIndex].second.first;
            if(globalTime < expectedtime){
             
                //unavialable 
                int stall = abs(globalTime - expectedtime);
                // cout << "stall " << stall << endl;
                globalTime += stall;
                totalStalls += stall ;

            }

            globalTime+= LOCAL_ACCESS_LATENCY;
            totalTime += LOCAL_ACCESS_LATENCY;
            totalLocalAccess +=LOCAL_ACCESS_LATENCY;
            
            // cout << "GT: " << globalTime << endl;
            // printLocalStorage (localStorage);

        }else{
            bool isFull;
            int emptyIdx = -1;
            for (int  b = 0 ;  b < RAM_CAPACITY; b++){
                if(localStorage[b].first.empty() ){
                        emptyIdx = b;
                        break; 
                }
            }
            if(emptyIdx == -1) {
                isFull = true;
            }else{
                localStorage[emptyIdx].first =currentPage.first;
                localStorage[emptyIdx].second.second =currentPage.second;
                globalTime += REMOTE_ACCESS_LATENCY;
                totalTime += REMOTE_ACCESS_LATENCY;
                totalRemoteAccess +=REMOTE_ACCESS_LATENCY;
                cout << "GT: " << globalTime << endl;
                // printLocalStorage (localStorage);
               
            }

            if(isFull){
                
                oracle(i, localStorage, tracker);
                cout << "GT: " << globalTime << endl;
                // printLocalStorage (localStorage);

            }
        }
      

          if((i+factor) < pages.size()){

        

        auto prefetchPage = pages[i+factor];
        cout << "Prefetch page: " << prefetchPage.first <<endl;
        bool isPrefetchInLocal = false ;

        
         //check if prefetch page is in local storage
        for (int  j = 0 ;  j < RAM_CAPACITY; j++){
            if(localStorage[j].first == prefetchPage.first ){
                localStorage[j].second.second = prefetchPage.second;
                isPrefetchInLocal=true;
                break;
            }
        }

       

         if (isPrefetchInLocal){
            cout << "prefetch in local" << endl;
            continue ;
        }else{
            bool isFull;
            int emptyIdx = -1;
            for (int  b = 0 ;  b < RAM_CAPACITY; b++){
                if(localStorage[b].first.empty() ){
                        emptyIdx = b;
                        break; 

                }
            }
            if(emptyIdx == -1) {
                isFull = true;
            }else{
                localStorage[emptyIdx].first =prefetchPage.first;
                localStorage[emptyIdx].second.second =prefetchPage.second;
                localStorage[emptyIdx].second.first = globalTime + REMOTE_ACCESS_LATENCY;
                cout << "prefeteched from remote" << endl;
                totalTimePrefetch += REMOTE_ACCESS_LATENCY;
                totalRemoteAccessPrefetch +=REMOTE_ACCESS_LATENCY;
                cout << "GT: " << globalTime << endl;
                // printLocalStorage (localStorage);
                continue;
            }

            if(isFull){
                cout << "prefetch replace" << endl; 
                oraclePrefetch(i, i+factor, localStorage, tracker);
                
                continue;

            }
        }
        }
            
            
    }            
                
    } else  if(MODE =="orclpref2"){
        
   

    for (int i=0; i < pages.size(); i++){

        

        auto currentPage = pages[i];
        cout << "Current page:" << currentPage.first << endl;
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

        if (isCurrentInLocal){
            int expectedtime  = localStorage[localIndex].second.first;
            if(globalTime < expectedtime){
             
                //unavialable 
                int stall = abs(globalTime - expectedtime);
                cout << "stall " << stall << endl;
                globalTime += stall;
                totalStalls += stall ;

            }

            globalTime+= LOCAL_ACCESS_LATENCY;
            totalTime += LOCAL_ACCESS_LATENCY;
            totalLocalAccess +=LOCAL_ACCESS_LATENCY;
            totalDemandHits +=1;
            cout << "GT: " << globalTime << endl;
            cout << "demand hit - in local storage"<< endl;
            // printLocalStorage (localStorage);

        }else{
            totalDemandMisses +=1;
            bool isFull;
            int emptyIdx = -1;
            for (int  b = 0 ;  b < RAM_CAPACITY; b++){
                if(localStorage[b].first.empty() ){
                        emptyIdx = b;
                        break; 
                }
            }
            if(emptyIdx == -1) {
                isFull = true;
            }else{
                localStorage[emptyIdx].first =currentPage.first;
                localStorage[emptyIdx].second.second =currentPage.second;
                globalTime += REMOTE_ACCESS_LATENCY;
                totalTime += REMOTE_ACCESS_LATENCY;
                totalRemoteAccess +=REMOTE_ACCESS_LATENCY;
                cout << "GT: " << globalTime << endl;
                cout << "demand miss - remote fetch"<< endl;
                // printLocalStorage (localStorage);
               
            }

            if(isFull){
                
                oracle(i, localStorage, tracker);
                cout << "GT: " << globalTime << endl;
                cout << "demand miss - remote fetch - replaced by oracle"<< endl;
                // printLocalStorage (localStorage);

            }
        }
      

          if((i+factor) < pages.size()){

        

        auto prefetchPage = pages[i+factor];
        cout << "Prefetch page: " << prefetchPage.first <<endl;
        bool isPrefetchInLocal = false ;

        
         //check if prefetch page is in local storage
        for (int  j = 0 ;  j < RAM_CAPACITY; j++){
            if(localStorage[j].first == prefetchPage.first ){
                localStorage[j].second.second = prefetchPage.second;
                isPrefetchInLocal=true;
                break;
            }
        }

       

         if (isPrefetchInLocal){
            // cout << "prefetch in local" << endl;
            totalPrefetchHits +=1;
            cout << "prefetch hit - in local storage"<< endl;
            continue ;
        }else{
            totalPreftechMisses +=1 ;
            bool isFull;
            int emptyIdx = -1;
            for (int  b = 0 ;  b < RAM_CAPACITY; b++){
                if(localStorage[b].first.empty() ){
                        emptyIdx = b;
                        break; 

                }
            }
            if(emptyIdx == -1) {
                isFull = true;
            }else{
                localStorage[emptyIdx].first =prefetchPage.first;
                localStorage[emptyIdx].second.second =prefetchPage.second;
                localStorage[emptyIdx].second.first = globalTime + REMOTE_ACCESS_LATENCY;
                // cout << "prefeteched from remote" << endl;
              
                totalTimePrefetch += REMOTE_ACCESS_LATENCY;
                totalRemoteAccessPrefetch +=REMOTE_ACCESS_LATENCY;
                cout << "GT: " << globalTime << endl;
                cout << "preftech miss - remote fetch"<< endl;
                // printLocalStorage (localStorage);
                continue;
            }

            if(isFull){
                // cout << "prefetch replace" << endl; 
                oraclePrefetch(i, i+factor, localStorage, tracker);
                cout << "preftech miss - remote fetch - replaced by oraclepref"<< endl;
                continue;

            }
        }
        }
            
            
    }            
                
    }else  if(MODE =="dmnd"){
        
   

    for (int i=0; i < pages.size(); i++){
        // cout << "*********************"<<endl;
        

        auto currentPage = pages[i];
        // cout << "Load - " << currentPage.first << endl;
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

        if (isCurrentInLocal){
            int expectedtime  = localStorage[localIndex].second.first;
            if(globalTime < expectedtime){
             
                //unavialable 
                int stall = abs(globalTime - expectedtime);
                // cout << "stall " << stall << endl;
                globalTime += stall;
                totalStalls += stall ;

            }

            globalTime+= LOCAL_ACCESS_LATENCY;
            totalTime += LOCAL_ACCESS_LATENCY;
            totalLocalAccess +=LOCAL_ACCESS_LATENCY;
            totalDemandHits +=1;
            // cout << "GT: " << globalTime << endl;
            // cout << "demand hit - in local storage"<< endl;
            // printLocalStorage (localStorage);

        }else{
            totalDemandMisses +=1;
            bool isFull = false;
            int emptyIdx = -1;
            for (int  b = 0 ;  b < RAM_CAPACITY; b++){
                if(localStorage[b].first.empty() ){
                        emptyIdx = b;
                        break; 
                }
            }
            if(emptyIdx == -1) {
                isFull = true;
            }else{
                localStorage[emptyIdx].first =currentPage.first;
                localStorage[emptyIdx].second.second =currentPage.second;
                globalTime += REMOTE_ACCESS_LATENCY;
                totalTime += REMOTE_ACCESS_LATENCY;
                totalRemoteAccess +=REMOTE_ACCESS_LATENCY;
                // cout << "GT: " << globalTime << endl;
                // cout << "demand miss - remote fetch"<< endl;
                // printLocalStorage (localStorage);
               
            }

            if(isFull){
                
                oracle(i, localStorage, tracker);
                // cout << "GT: " << globalTime << endl;
                // cout << "demand miss - remote fetch - replaced by oracle"<< endl;
                // printLocalStorage (localStorage);

            }
        }
      

          if((i+factor) < pages.size()){

        

        auto prefetchPage = pages[i+factor];
        // cout << "Prefetch - " << prefetchPage.first <<endl;
        bool isPrefetchInLocal = false ;

        
         //check if prefetch page is in local storage
        for (int  j = 0 ;  j < RAM_CAPACITY; j++){
            if(localStorage[j].first == prefetchPage.first ){
                localStorage[j].second.second = prefetchPage.second;
                isPrefetchInLocal=true;
                break;
            }
        }

       

         if (isPrefetchInLocal){
            // cout << "prefetch in local" << endl;
            totalPrefetchHits +=1;
            // cout << "prefetch hit - in local storage"<< endl;
            continue ;
        }else{
            totalPreftechMisses +=1 ;
            bool isFull;
            int emptyIdx = -1;
            for (int  b = 0 ;  b < RAM_CAPACITY; b++){
                if(localStorage[b].first.empty() ){
                        emptyIdx = b;
                        break; 

                }
            }
            if(emptyIdx == -1) {
                isFull = true;
            }else{
                localStorage[emptyIdx].first =prefetchPage.first;
                localStorage[emptyIdx].second.second =prefetchPage.second;
                localStorage[emptyIdx].second.first = globalTime + REMOTE_ACCESS_LATENCY;
                // cout << "prefeteched from remote" << endl;
              
                totalTimePrefetch += REMOTE_ACCESS_LATENCY;
                totalRemoteAccessPrefetch +=REMOTE_ACCESS_LATENCY;
            
                // cout << "preftech miss - remote fetch"<< endl;
                // printLocalStorage (localStorage);
                continue;
            }

            if(isFull){
                // cout << "prefetch replace" << endl; 
                oraclePrefetch(i, i+factor, localStorage, tracker);
                // cout << "preftech miss - remote fetch - replaced by oraclepref"<< endl;
                continue;

            }
        }
        }
            
            
    }            
                
    }
}

 

int main(int argc, char **argv)
{
    

  
    MODE = argv[1];
   
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
        
        mySet.insert(pageString);
        pages.push_back({pageString, isStore});
        
    }
    auto pagesSize = pages.size();
    auto unique = mySet.size();
    auto fivepercent = std::ceil (unique *0.05);
    auto tenpercent =  std::ceil (unique *0.1);
    auto twentypercent =  std::ceil (unique *0.2);
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
    // RAM_CAPACITY = onefourth;
    // runSimulation();
    // printStats("25%", onefourth);

     



    // factor = std::ceil (full);

    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = twentypercent;
    // runSimulation();
    // cout << "factor 1"<< endl;
    // printStats("20%", twentypercent);


    // factor = std::ceil (full*2.5);

    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = twentypercent;
    // runSimulation();
    // cout << "factor 2.5"<< endl;
    // printStats("20%", twentypercent);


    // factor = std::ceil (full*5);

    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = twentypercent;
    // runSimulation();
    // cout << "factor 5"<< endl;
    // printStats("20%", twentypercent);


    // factor = std::ceil (full*7.5);

    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = twentypercent;
    // runSimulation();
    // cout << "factor 7.5"<< endl;
    // printStats("20%", twentypercent);

    // factor = std::ceil (full*10);

    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = twentypercent;
    // runSimulation();
    // cout << "factor 10"<< endl;
    // printStats("20%", twentypercent);

    // factor = std::ceil (full*15);

    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = twentypercent;
    // runSimulation();
    // cout << "factor 15"<< endl;
    // printStats("20%", twentypercent);

 cout << "-------------------------------------------------------"<< endl;
    RAM_CAPACITY = onefourth;
    runSimulation();
    printStats("25%", onefourth);

    cout << "-------------------------------------------------------"<< endl;
    RAM_CAPACITY = fivepercent;
    runSimulation();
    printStats("5%", fivepercent);
    // cout << "-------------------------------------------------------"<< endl;
    // RAM_CAPACITY = onefourth;
    // runSimulation();
    // printStats("25%", onefourth);
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