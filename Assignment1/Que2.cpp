#include<bits/stdc++.h>
using namespace std;
char input_name[100];
char i_or_d;
char type;
unsigned pc;
int numtraces;
unsigned long long addr;
unsigned long long total=0,countmiss=0;
unsigned long long l3InclusiveMiss=0,l2InclusiveMiss=0,l2InclusiveHit=0,l3InclusiveHit=0;
unsigned long long l3NineMiss=0,l2NineMiss=0,l3NineHit=0,l2NineHit=0;
unsigned long long l3ExclusiveMiss=0,l2ExclusiveMiss=0,l2ExclusiveHit=0,l3ExclusiveHit=0;

unsigned long long L2tag[1024][8][2]={0};
unsigned long long L3tag[32768][2]={0};

unsigned long long capacity=0,conflictMiss=0,capacityMiss=0,coldMiss=0,coldMiss2=0;

set <unsigned long long> coldTag;


void l3Inclusive(unsigned long long addr,int zeroIndex,int lruIndex)
{
 unsigned long long setno,tag,remainaddr,l2setno,l2tag,l2remainaddr;
 tag=addr/64;
 l2remainaddr=addr/64;
 l2tag=l2remainaddr/1024;
 l2setno=l2remainaddr%1024;
 int f=1;
 long long temp=-1;
 long long l3ZeroIndex=-1,l3LruIndex=-1;
 

 
 for(long long i=0;i<32768;i++)
 {
	 if(L3tag[i][0]==tag && L3tag[i][1]!=0)
	 {
	 	for(long long j=0;j<32768;j++)
	 	 {
	 		if(i==j)
	 		L3tag[j][1]=1;
	 		else if(L3tag[j][1]!=0)
	  		L3tag[j][1]+=1;
	  	}
		f=1;
 		l3InclusiveHit++;
	 
	  	if(zeroIndex!=-1)
	 	{
	  		L2tag[l2setno][zeroIndex][0]=l2tag;
	 		for(int j=0;j<8;j++)
	  		{
	  			if(zeroIndex==j)
	  			L2tag[l2setno][j][1]=1;
	 			else if(L2tag[l2setno][j][1]!=0)
	 			L2tag[l2setno][j][1]+=1;
	  		}
	 	}
	 	else
		{
	 	 	L2tag[l2setno][lruIndex][0]=l2tag;
	 	 	for(int j=0;j<8;j++)
	 		{
	 	 		if(lruIndex==j)
	 	 		L2tag[l2setno][j][1]=1;
	 	 		else if(L2tag[l2setno][j][1]!=0)
	 	 		L2tag[l2setno][j][1]+=1;
	 	 	}
	 	}	
	 	 break;		
	 }
	 else
 	 {
 	 	long long t=L3tag[i][1];
 	 	if(L3tag[i][1]==0)
 	 	l3ZeroIndex=i;
 	 	else if(temp<t)
 	 	{
 	 	l3LruIndex=i;
 		temp=t;
	 	}
 	 	f=0;
 	 }
 
 }
 
 if(f==0)
 {
 
 //L3 miss then block copied to both L2 L3
 	 l3InclusiveMiss++;
 	 if(l3ZeroIndex!=-1)
 	 {
 	 	//capacity++;
 	 	L3tag[l3ZeroIndex][0]=tag;
 	 	for(long long j=0;j<32768;j++)
 	 	{
 	 		if(l3ZeroIndex==j)
 	 		L3tag[j][1]=1;
 	 		else if(L3tag[j][1]!=0)
 	 		L3tag[j][1]+=1;
 	 	}
 	 	coldTag.insert(tag);
 	 	coldMiss2++;
 	 	 //cout<<l3ZeroIndex<<endl;
 	}	
 	else
 	{
 		unsigned long long victimTag=L3tag[l3LruIndex][0]/1024;
 		unsigned long long victimSet=L3tag[l3LruIndex][0]%1024;
 	  	for(int k=0;k<8;k++)
 	  	{
 	  		if(L2tag[victimSet][k][0]==victimTag)
 	  		{
			 	L2tag[victimSet][k][0]=0;
				L2tag[victimSet][k][1]=0;
				 //zeroIndex=k;
				break;	  			
 	  		}
 	  	}
 	  	
 	  	auto it=coldTag.find(tag);
 	  	if(it==coldTag.end())
 	  	{
 	  	 coldTag.insert(tag);
 	  	 coldMiss2++;
 	  	}
 	  	else
 		capacityMiss++;
 		
 	  	L3tag[l3LruIndex][0]=tag;
 	  	for(long long j=0;j<32768;j++)
 	 	{
 	 		if(l3LruIndex==j)
 	 		L3tag[j][1]=1;
 	 		else if(L3tag[j][1]!=0)
 	 		L3tag[j][1]+=1;
 	 	}
 	  }
 	  	
 	 	if(zeroIndex!=-1)
 		{
 	 		L2tag[l2setno][zeroIndex][0]=l2tag;
 	 		for(int j=0;j<8;j++)
 	 		{
 	 			if(zeroIndex==j)
 	 			L2tag[l2setno][j][1]=1;
 				else if(L2tag[l2setno][j][1]!=0)
 				L2tag[l2setno][j][1]+=1;
 	 		}
 	 	}
		else
 		{
 		
 	 		L2tag[l2setno][lruIndex][0]=l2tag;
 	 		for(int j=0;j<8;j++)
 	 		{
 				if(lruIndex==j)
 				L2tag[l2setno][j][1]=1;
 	 			else if(L2tag[l2setno][j][1]!=0)
 	 			L2tag[l2setno][j][1]+=1;
 	 		}
 	 	}
 		
 }

}







void inclusivePolicy(char *filename)
{
unsigned long long  l2Access=0,totalrecord=0;

for (int k=0; k<numtraces; k++) {

      sprintf(input_name, "%s_%d", filename, k);
      FILE *fp = fopen(input_name, "rb");
      assert(fp != NULL);
   
      while (1) {
      
         fread(&i_or_d, sizeof(char), 1, fp);
         fread(&type, sizeof(char), 1, fp);
         fread(&addr, sizeof(unsigned long long), 1, fp);
         fread(&pc, sizeof(unsigned), 1, fp);
         if(feof(fp))
         break;
         totalrecord++;
         
         unsigned long long setno,tag,remainaddr;	
 	 if(int(type)!=0)
 	 {
 	 	l2Access++;
 	 	int f=0;
 	 	int temp=-1;
 	 	int zeroIndex=-1;
 	 	int lruIndex=-1;
 	 	remainaddr=addr/64;
 	 	tag=remainaddr/1024;
 	 	setno=remainaddr%1024;
 	 	//cout<<remainaddr<<"	"<<tag<<"	"<<setno<<endl;
 	 	for(int i=0;i<8;i++)
 	 	{
 	 		if(L2tag[setno][i][0]==tag && L2tag[setno][i][1]!=0)
 	 		{
 	 			for(int j=0;j<8;j++)
 	 			{
 	 				if(i==j)
 	 				L2tag[setno][j][1]=1;
 	 				else if(L2tag[setno][j][1]!=0)
 	 				L2tag[setno][j][1]+=1;
 	 			}
 	 			f=1;
 	 			l2InclusiveHit++;
 	 			break;
 	 		}
 	 		else
 	 		{
 	 			long long t=L2tag[setno][i][1];
 	 			if(L2tag[setno][i][1]==0)
 	 			zeroIndex=i;
 	 			else if(temp<t)
 	 			{
 	 			lruIndex=i;
 	 			temp=t;
 	 			}
 	 			f=0;
			}
 	 	}
 	 	//cout<<"l2 lru "<<lruIndex<<endl;
 	 	if(f==0)
 	 	{
 	 		l2InclusiveMiss++;
 	 		l3Inclusive(addr,zeroIndex,lruIndex);
 	 	}
 	 	
 	 }
	
         // Process the entry
         
        
      }
      fclose(fp);
      printf("Done reading file %d!\n", k);
     
}                                                                                                              
 	
        cout<<"L3 cold Misses "<<l3InclusiveMiss-capacityMiss<<endl;
        cout<<"L3 Capacity Miss "<<capacityMiss<<endl;
        
   


}




int main(int argc, char* argv[]){

numtraces = atoi(argv[2]);
inclusivePolicy(argv[1]);
return 0;
}
