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
unsigned long long L3tag[2048][16][2]={0};



void l3Inclusive(unsigned long long addr,int zeroIndex,int lruIndex)
{
 unsigned long long setno,tag,remainaddr,l2setno,l2tag,l2remainaddr;
 remainaddr=addr/64;
 tag=remainaddr/2048;
 setno=remainaddr%2048;
 l2remainaddr=addr/64;
 l2tag=l2remainaddr/1024;
 l2setno=l2remainaddr%1024;
 int f=1;
 int temp=-1;
 int l3ZeroIndex=-1,l3LruIndex=-1;
 for(int i=0;i<16;i++)
 	 {
 	 	if(L3tag[setno][i][0]==tag && L3tag[setno][i][1]!=0)
 	 	{
 	 		// L3 hit then copied to L2
 	 		for(int j=0;j<16;j++)
 	 		{
 	 			if(i==j)
 	 			L3tag[setno][j][1]=1;
 	 			else if(L3tag[setno][j][1]!=0)
 	 			L3tag[setno][j][1]+=1;
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
 	 		long long t=L3tag[setno][i][1];
 	 		if(L3tag[setno][i][1]==0)
 	 			l3ZeroIndex=i;
 	 		else if(temp<t)
 	 		{
 	 		l3LruIndex=i;
 			temp=t;
	 		}
 	 		f=0;
 	 	}
 	 }
 	 //cout<<"L3 lru "<<l3LruIndex<<endl;
 	 if(f==0)
 	 {
 	 	//L3 miss then block copied to both L2 L3
 	 	l3InclusiveMiss++;
 	 	if(l3ZeroIndex!=-1)
 	 	{
 	 		L3tag[setno][l3ZeroIndex][0]=tag;
 	 		for(int j=0;j<16;j++)
 	 		{
 	 			if(l3ZeroIndex==j)
 	 			L3tag[setno][j][1]=1;
 	 			else if(L3tag[setno][j][1]!=0)
 	 			L3tag[setno][j][1]+=1;
 	 		}
 		}
  		else
 		{
 			int bit=setno/1024;
 			unsigned long long victimTag=(2*L3tag[setno][l3LruIndex][0])+bit;
 	  		for(int k=0;k<8;k++)
 	  		{
 	  			if(L2tag[l2setno][k][0]==victimTag)
 	  			{
				 	L2tag[l2setno][k][0]=0;
				 	L2tag[l2setno][k][1]=0;
				 	zeroIndex=k;
				 	break;	  			
 	  			}
 	  		}
 	  		L3tag[setno][l3LruIndex][0]=tag;
 	  		for(int j=0;j<16;j++)
 	 		{
 	 			if(l3LruIndex==j)
 	 			L3tag[setno][j][1]=1;
 	 			else if(L3tag[setno][j][1]!=0)
 	 			L3tag[setno][j][1]+=1;
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
  cout<<"l2 access "<<l2Access<<endl;
 cout<<"L2 Inclusive misses "<<l2InclusiveMiss<<endl;
 cout<<"L2 Inclusive hit "<<l2InclusiveHit<<endl;
 cout<<"L3 Inclusive misses "<<l3InclusiveMiss<<endl;
 cout<<"L3 Inclusive hit "<<l3InclusiveHit<<endl;
      cout<<totalrecord<<endl;
   


}

void l3Nine(unsigned long long addr,int zeroIndex,int lruIndex)
{
 unsigned long long setno,tag,remainaddr,l2setno,l2tag,l2remainaddr;
 remainaddr=addr/64;
 tag=remainaddr/2048;
 setno=remainaddr%2048;
 l2remainaddr=addr/64;
 l2tag=l2remainaddr/1024;
 l2setno=l2remainaddr%1024;
 int f=1;
 int temp=-1;
 int l3ZeroIndex=-1,l3LruIndex=-1;
 for(int i=0;i<16;i++)
 	 {
 	 	if(L3tag[setno][i][0]==tag && L3tag[setno][i][1]!=0)
 	 	{
 	 		// L3 hit then copied to L2
 	 		for(int j=0;j<16;j++)
 	 		{
 	 			if(i==j)
 	 			L3tag[setno][j][1]=1;
 	 			else if(L3tag[setno][j][1]!=0)
 	 			L3tag[setno][j][1]+=1;
 	 		}
 	 		f=1;
 	 		l3NineHit++;
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
 	 		long long t=L3tag[setno][i][1];
 	 		if(L3tag[setno][i][1]==0)
 	 			l3ZeroIndex=i;
 	 		else if(temp<t)
 	 		{
 	 		l3LruIndex=i;
 			temp=t;
	 		}
 	 		f=0;
 	 	}
 	 }
 	 //cout<<"L3 lru "<<l3LruIndex<<endl;
 	 if(f==0)
 	 {
 	 	//L3 miss then block copied to both L2 L3
 	 	l3NineMiss++;
 	 	if(l3ZeroIndex!=-1)
 	 	{
 	 		L3tag[setno][l3ZeroIndex][0]=tag;
 	 		for(int j=0;j<16;j++)
 	 		{
 	 			if(l3ZeroIndex==j)
 	 			L3tag[setno][j][1]=1;
 	 			else if(L3tag[setno][j][1]!=0)
 	 			L3tag[setno][j][1]+=1;
 	 		}
 		}
  		else
 		{
 			
 	  		L3tag[setno][l3LruIndex][0]=tag;
 	  		for(int j=0;j<16;j++)
 	 		{
 	 			if(l3LruIndex==j)
 	 			L3tag[setno][j][1]=1;
 	 			else if(L3tag[setno][j][1]!=0)
 	 			L3tag[setno][j][1]+=1;
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

void ninePolicy(char *filename)
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
 	 			l2NineHit++;
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
 	 		l2NineMiss++;
 	 		l3Nine(addr,zeroIndex,lruIndex);
 	 	}
 	 	
 	 }
	
         // Process the entry
         
        
      }
      fclose(fp);
      printf("Done reading file %d!\n", k);
     
}
    cout<<"l2 access "<<l2Access<<endl;
 cout<<"L2 Nine misses "<<l2NineMiss<<endl;
  cout<<"L2 Nine hit "<<l2NineHit<<endl;
      cout<<"L3 Nine misses "<<l3NineMiss<<endl;
  
        cout<<"L3 Nine hit "<<l3NineHit<<endl;
     
   



}

void l3Exclusive(unsigned long long addr,int zeroIndex,int lruIndex)
{
 unsigned long long setno,tag,remainaddr,l2setno,l2tag,l2remainaddr,evictAddr;
 remainaddr=addr/64;
 tag=remainaddr/2048;
 setno=remainaddr%2048;
 l2remainaddr=addr/64;
 l2tag=l2remainaddr/1024;
 l2setno=l2remainaddr%1024;
 int f=0;
 long long temp=-1;
 int l3ZeroIndex=-1,l3LruIndex=-1;
 for(int i=0;i<16;i++)
 	 {
 	 	if(L3tag[setno][i][0]==addr && L3tag[setno][i][1]!=0)
 	 	{
 	 		// L3 hit then copied to L2
 	 		/*for(int j=0;j<16;j++)
 	 		{
 	 			if(i==j)
 	 			L3tag[setno][j][1]=1;
 	 			else if(L3tag[setno][j][1]!=0)
 	 			L3tag[setno][j][1]+=1;
 	 		}*/
 	 		f=1;
 	 		l3ExclusiveHit++;
 	 		if(zeroIndex!=-1)
 	 		{
 	 			L2tag[l2setno][zeroIndex][0]=addr;
 	 			for(int j=0;j<8;j++)
	 	 		{
	 	 			if(zeroIndex==j)
	 	 				L2tag[l2setno][j][1]=1;
	 				else if(L2tag[l2setno][j][1]!=0)
	 					L2tag[l2setno][j][1]+=1;
	 	 		}
	 	 		L3tag[setno][i][0]=0;
	 	 		L3tag[setno][i][1]=0;   //Invalidating L3    may be changes
	 	 		//l3ZeroIndex=i;
	 	 		
 	 		}
 	 		else
 	 		{
 	 			L3tag[setno][i][0]=0;
	 	 		L3tag[setno][i][1]=0;
 	 			evictAddr=L2tag[l2setno][lruIndex][0];
 	 			L2tag[l2setno][lruIndex][0]=addr;
 	 			for(int j=0;j<8;j++)
 	 			{
 	 				if(lruIndex==j)
 	 				L2tag[l2setno][j][1]=1;
 	 				else if(L2tag[l2setno][j][1]!=0)
 	 				L2tag[l2setno][j][1]+=1;
 	 			}
 	 			L3tag[setno][i][0]=evictAddr;
 	 			for(int j=0;j<16;j++)
		 	 		{
		 	 			if(i==j)
		 	 			L3tag[setno][j][1]=1;
		 	 			else if(L3tag[setno][j][1]!=0)
		 	 			L3tag[setno][j][1]+=1;
		 	 		}
 	 			/*long long evictSet=(evictAddr/64)%2048;
 	 			int evictLruIndex=-1;
 	 			int lruflag=0;
 	 			long long temp1=-1;
 	 			for(int k=0;k<16;k++)
 	 			{
	 	 			long long t=L3tag[evictSet][k][1];
		 	 		if(L3tag[evictSet][k][1]==0)
		 	 		{
			 	 		L3tag[evictSet][k][0]=evictAddr;
			 	 		lruflag=0;
			 	 		for(int j=0;j<16;j++)
		 	 			{
		 	 			if(k==j)
		 	 			L3tag[evictSet][j][1]=1;
		 	 			else if(L3tag[evictSet][j][1]!=0)
		 	 			L3tag[evictSet][j][1]+=1;
		 	 			}
			 	 		break;
		 	 		}
		 	 		else if(temp1<t)
		 	 		{
		 	 		evictLruIndex=k;
		 			temp1=t;
		 			lruflag=1;
	 				}
 	 			}
 	 			if(lruflag==1)
 	 			{
 	 				L3tag[evictSet][evictLruIndex][0]=evictAddr;
 	 			
		 	 		for(int j=0;j<16;j++)
		 	 		{
		 	 			if(evictLruIndex==j)
		 	 			L3tag[evictSet][j][1]=1;
		 	 			else if(L3tag[evictSet][j][1]!=0)
		 	 			L3tag[evictSet][j][1]+=1;
		 	 		}
	 	 		}*/
 	 		}
 	 		break;
 	 	}
 	 	else
 	 	{
 	 		long long t=L3tag[setno][i][1];
 	 		if(L3tag[setno][i][1]==0)
 	 		l3ZeroIndex=i;
 	 		else if(temp<t)
 	 		{
 	 		l3LruIndex=i;
 			temp=t;
	 		}
 	 		f=0;
 	 	}
 	 }
 	 //cout<<"L3 lru "<<l3LruIndex<<endl;
 	 if(f==0)
 	 {
 	 	//L3 miss then block copied to both L2 L3
 	 	l3ExclusiveMiss++;
 	 	
 	 	if(zeroIndex!=-1)
 		{
 	 		L2tag[l2setno][zeroIndex][0]=addr;
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
 			unsigned evictAddr=L2tag[l2setno][lruIndex][0];
 			L2tag[l2setno][lruIndex][0]=addr;
 	 		for(int j=0;j<8;j++)
 	 		{
 				if(lruIndex==j)
 				L2tag[l2setno][j][1]=1;
 	 			else if(L2tag[l2setno][j][1]!=0)
 	 			L2tag[l2setno][j][1]+=1;
 	 		}
 	 		
 			long long evictSet=(evictAddr/64)%2048;
 			//if(evictSet==setno)
 			//cout<<"Haan ewual"<<endl;
 	 		int evictLruIndex=-1;
 	 		int lruflag=0;
 	 		for(int k=0;k<16;k++)
 	 		{
	 	 		long long t=L3tag[evictSet][k][1];
		 	 	if(L3tag[evictSet][k][1]==0)
		 	 	{
			 	 	L3tag[evictSet][k][0]=evictAddr;
			 	 	lruflag=0;
			 	 	for(int j=0;j<16;j++)
		 	 		{
		 	 		if(k==j)
		 	 		L3tag[evictSet][j][1]=1;
		 	 		else if(L3tag[evictSet][j][1]!=0)
		 	 		L3tag[evictSet][j][1]+=1;
		 	 		}
			 	 	break;
		 	 	}
		 	 	else if(temp<t)
		 	 	{
		 	 	evictLruIndex=k;
		 		temp=t;
		 		lruflag=1;
	 			}
 	 		}
 	 		if(lruflag==1)
 	 		{
 	 			L3tag[evictSet][evictLruIndex][0]=evictAddr;
 	 		
		 	 	for(int j=0;j<16;j++)
		 	 	{
		 	 		if(evictLruIndex==j)
		 	 		L3tag[evictSet][j][1]=1;
		 	 		else if(L3tag[evictSet][j][1]!=0)
		 	 		L3tag[evictSet][j][1]+=1;
		 	 	}
	 	 	}		
 	 	}
 	 		 	
 	 }
}

void exclusivePolicy(char *filename)
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
 	 	long long temp=-1;
 	 	int zeroIndex=-1;
 	 	int lruIndex=-1;
 	 	remainaddr=addr/64;
 	 	tag=remainaddr/1024;
 	 	setno=remainaddr%1024;
 	 	//cout<<remainaddr<<"	"<<tag<<"	"<<setno<<endl;
 	 	for(int i=0;i<8;i++)
 	 	{
 	 		if(L2tag[setno][i][0]==addr && L2tag[setno][i][1]!=0)
 	 		{
 	 			for(int j=0;j<8;j++)
 	 			{
 	 				if(i==j)
 	 				L2tag[setno][j][1]=1;
 	 				else if(L2tag[setno][j][1]!=0)
 	 				L2tag[setno][j][1]+=1;
 	 			}
 	 			f=1;
 	 			l2ExclusiveHit++;
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
 	 		l2ExclusiveMiss++;
 	 		l3Exclusive(addr,zeroIndex,lruIndex);
 	 	}
 	 	
 	 }
	
         // Process the entry
         
        
      }
      fclose(fp);
      printf("Done reading file %d!\n", k);
     
}
 cout<<"l2 access "<<l2Access<<endl;
 cout<<"L2 Exclusive misses "<<l2ExclusiveMiss<<endl;
  cout<<"L2 Exclusive hit "<<l2ExclusiveHit<<endl;
 cout<<"L3 Exclusive misses "<<l3ExclusiveMiss<<endl;
cout<<"L3 Exclusive hit "<<l3ExclusiveHit<<endl;
}


int main(int argc, char* argv[]){

numtraces = atoi(argv[2]);
int option;
cout<<"1. Inclusive policy"<<endl;
cout<<"2. NINE Policy"<<endl;
cout<<"3. Exclusive Policy"<<endl;
cin>>option;
switch(option)
{
case 1:
inclusivePolicy(argv[1]);
break;

case 2:
ninePolicy(argv[1]);
break;

case 3:
exclusivePolicy(argv[1]);
break;

default:
cout<<"Enter valid choice"<<endl;
}


return 0;
}
