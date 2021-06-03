#include<bits/stdc++.h>

using namespace std;

unsigned long long cache[2048][16][2]={0};
unordered_map<unsigned long long,unsigned long long> checkAccess;
map<unsigned long long, float> distFreq;
unsigned long long current=1;

int main()
{
	ifstream file ("prog1addrtrace.txt");// reading of machine accesses

	FILE* missTrace=fopen("missTraces1.txt","w");//the trace of accesses that is miss in the cache
	
	unsigned long long tid, block,addr,remainaddr,setno,tag;
	unsigned long long hit=0,miss=0;
	while (file>>tid>>addr)
	{
		int f=0;
 	 	long long temp=-1;
 	 	int zeroIndex=-1;
 	 	int lruIndex=-1;
		remainaddr=addr/64;
		setno=remainaddr%2048;
		tag=remainaddr/2048;
		for(int i=0;i<16;i++)
		{
			if(cache[setno][i][0]==tag && cache[setno][i][1]!=0)
			{
				for(int j=0;j<16;j++)
 	 			{
 	 				if(i==j)
 	 				cache[setno][j][1]=1;
 	 				else if(cache[setno][j][1]!=0)
 	 				cache[setno][j][1]+=1;
 	 			}
 	 			f=1;
 	 			hit++;
 	 			break;
			}
			else
			{
				long long t=cache[setno][i][1];
 	 			if(cache[setno][i][1]==0)
 	 			zeroIndex=i;
 	 			else if(temp<t)
 	 			{
 	 			lruIndex=i;
 	 			temp=t;
 	 			}
 	 			f=0;
			}
		}
		if(f==0)
 	 	{
 	 		miss++;
 	 		fprintf(missTrace,"%llu %llu\n",tid, addr);
 	 		if(zeroIndex!=-1)
 	 		{       cache[setno][zeroIndex][0]=tag;
 	 		        for(int j=0;j<16;j++)
 	 			{
 	 				if(zeroIndex==j)
 	 				cache[setno][j][1]=1;
 	 				else if(cache[setno][j][1]!=0)
 	 				cache[setno][j][1]+=1;
 	 			}
 	 		}
 	 		else
 	 		{ 	cache[setno][lruIndex][0]=tag;
 	 			for(int j=0;j<16;j++)
 	 			{
 	 				if(lruIndex==j)
 	 				cache[setno][j][1]=1;
 	 				else if(cache[setno][j][1]!=0)
 	 				cache[setno][j][1]+=1;
 	 			}
 	 		}
 	 	}
 	 	fflush(missTrace);
	}
fclose(missTrace);
cout<<"Total Hits : "<<hit<<endl;
cout<<"Total Miss : "<<miss<<endl;



return 0;
}
