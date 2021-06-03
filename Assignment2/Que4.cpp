#include<bits/stdc++.h>

using namespace std;

map<unsigned long long,set<int>> threadPerBlock;
int blocksPerThread[8];

int main()
{
unsigned long long tid,addr,block;
ifstream file ("prog1addrtrace.txt"); //file for which we want to blocks shared per thread 
while (file>>tid>>addr)
{
	block=addr/64;
	threadPerBlock[block].insert(tid);   //  inserting tid that are accesing this block
}
for(auto i : threadPerBlock)
{
blocksPerThread[i.second.size()-1]++;
}
for(int i=0;i<8;i++)
cout<<blocksPerThread[i]<<endl;   //memory blocks shared by threads
return 0;
}
