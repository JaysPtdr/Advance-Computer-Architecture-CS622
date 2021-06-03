#include<bits/stdc++.h>
using namespace std;

unordered_map< long long int, long long int> checkAccess;
map< long long int,float> distFreq;
 long long int current=1;
long long int cnt=10;

int main()
{
  //ifstream file ("missTraces1.txt");  // reading machine access for generative cumulative density of memory misstraces(after Que3.cpp)
  ifstream file ("prog1addrtrace.txt");// reading of machine accesses for generating cumulative density of memory traces
  double cummulative=0.0;
  long long int  tid, block,addr,temp,totalAccess=0;
  
  //FILE* dist=fopen("missdistFrequency1.txt","w");
  FILE* dist=fopen("distFrequency1.txt","w");//cumulative frequencies
  cout<<"File reading Started"<<endl;
  while (file>>tid>>addr)
  {
	  block=addr/64;
	  if(checkAccess.find(block)==checkAccess.end())        ////////First access of block
	  {
		 checkAccess[block]=current;
		 current++;
		 
      	  }
      	  else                                                 ////////Block is accessed already
      	  {
      	  	temp=checkAccess[block];
      	  	distFreq[current-temp]++;
      	  	checkAccess[block]=current;
      	  	totalAccess++;
      	  	current++;
      	  }       
  }
  cout<<"Calculation of cumulative distance started"<<endl;
  for(auto i:distFreq )
  {
        cummulative+=(i.second/totalAccess);                  ///////calculating cumulative distance of each distance
        fprintf(dist, "%llu %f \n", i.first, cummulative);    /////// insertion of cumulative distances in the file
  }
  cout<<"No. of distinct distances: "<<checkAccess.size()<<endl;
  cout<<"Calculation and storing of cumulative distance done..!!"<<endl;
  fclose(dist);
  return 0; 
}
