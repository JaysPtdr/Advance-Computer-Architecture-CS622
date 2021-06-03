#include<bits/stdc++.h>
 #define N 8
 using namespace std;
 map <unsigned long long, int[9]> bitVector; //`first 8 bits of processor and last for current state
 map <unsigned long long, int[3]> timer;   // index 0=time, index 1=processor, index 2=gCount
 map<unsigned long long, unsigned long long> victimMap;
 map<unsigned long long, string> rwMap;
 unsigned long long L1tag[8][64][8][2]={0};
 unsigned long long L2tag[4096][16][3]={0};
 set<int> getProcessor;
 unsigned long long cycles=0;
 unsigned long long gId=1,l1Hit=0,l2Hit=0,l1Miss=0,l2Miss=0;
 //map which have address and each processor with it's operation
 
 struct processor
 {
	 unsigned long long gCount;
	 string rw; 
	 unsigned long long addr;
	 processor *next;
 };
 struct l1Q
 {
	 string msg;
	 unsigned long long addr;
	 unsigned long long gCount;
	 int cacheNumber;
	 int state;
	 l1Q *next;
 };
 struct l2Q
 {
	 string msg;
	 unsigned long long addr;
	 unsigned long long gCount;
	 int cacheNumber;
	 int state;
	 l2Q *next;
 };
 
 l2Q *l2front[8]={NULL},*l2rear[8]={NULL};
 l2Q *l2mssg[8]={NULL};
 
 l1Q *l1front[8]={NULL},*l1rear[8]={NULL};
 l1Q *l1mssg[8]={NULL};
 
 map<unsigned long long, string> buffer[8];
 
 unsigned long long L1GET[8]={0}, L1GETX[8]={0}, L1PUT[8]={0}, L1PUTX[8]={0}, L1INVALID[8]={0}, L1INVALID_ACK[8]={0}, L1UPGRADE_ACK[8]={0}, L1NACK[8]={0};
 unsigned long long L2GET=0, L2GETX=0, L2PUT=0, L2PUTX=0, L2SWB=0, L2UPGRADE=0, L2ACK=0;
 
 void readRequestHome(unsigned long long addr,int p,unsigned long long gCount)
 {
 	 unsigned long long setno,tag,remainaddr,l2setno,l2tag,l2remainaddr;
	 remainaddr=addr/64;
	 tag=remainaddr/4096;
	 setno=remainaddr%4096;
	 
	 
	 
	 
	 int bankid=remainaddr%3;
	 l2mssg[bankid]=new l2Q();
  	 l2mssg[bankid]->msg="GET";
  	 l2mssg[bankid]->addr=addr;
  	 l2mssg[bankid]->gCount=gCount;
  	 l2mssg[bankid]->cacheNumber=p;
  	 l2mssg[bankid]->next=NULL;
  	        int f1=1;
  	        int temp=-1;
	        int zeroIndex=-1,lruIndex=-1;
  	        for(int i=0;i<16;i++)
 	 	{
 	 		if(L2tag[setno][i][0]==tag && L2tag[setno][i][1]!=0)
 	 		{
 	 			f1=1;
 	 			l2Hit++;
 	 			//cout<<"L2hit"<<endl;
 	 			if(bitVector[addr][8]==0)
 	 			{
 	 				bitVector[addr][8]=2;
 	 				l2mssg[bankid]->state=0;
 	 			}
 	 			else if(bitVector[addr][8]==2 || bitVector[addr][8]==3)
 	 			{
 	 				bitVector[addr][8]=3;
 	 				l2mssg[bankid]->state=3;
 	 			}
 	 			else if(bitVector[addr][8]==1)
 	 			{
 	 				bitVector[addr][8]=5;
 	 				l2mssg[bankid]->state=1;
 	 			}
 	 			else if(bitVector[addr][8]==5)
 	 			{
 	 				bitVector[addr][8]=5;
 	 				l2mssg[bankid]->state=5;
 	 				l1mssg[p]=new l1Q();
 					l1mssg[p]->msg="NACK";
 					l1mssg[p]->cacheNumber=p;
 					l1mssg[p]->addr=addr;
 					l1mssg[p]->gCount=gCount;
 					if(l1front[p]==NULL)
					  	 {
					   		l1front[p]=l1rear[p]=l1mssg[p];
					   	 }
					   	 else
						 {
					  	 	l1rear[p]->next=l1mssg[p];
					  		l1rear[p]=l1mssg[p];
					  	 }
 					//NAC timer should be handle 
 	 			}
 	 			
 	 			for(int j=0;j<16;j++)
 				{
	 				if(i==j)
 	 				L2tag[setno][j][1]=1;
 	 				else if(L2tag[setno][j][1]!=0)
	 				L2tag[setno][j][1]+=1;
		 	 	}
 	 			break;
 	 		}
 	 		else
 	 		{
 	 			f1=0;
 	 		}
 	 	}
 	 	if(f1==0)
 	 	{
 	 		l2Miss++;
 	 		l2mssg[bankid]->state=6;   ///miss state
 	 		// will do it later... invalidation and further
 	 		//cout<<"L2miss"<<endl;
 	 	}
 	 	
 	 	 if(l2front[bankid]==NULL)
	  	 {
	   		l2front[bankid]=l2rear[bankid]=l2mssg[bankid];
	   	 }
	   	 else
		 {
	  	 	l2rear[bankid]->next=l2mssg[bankid];
	  		l2rear[bankid]=l2mssg[bankid];
	  	 }
 	 	
 }
 void readRequest(unsigned long long addr,int p,unsigned long long gCount)
 {
	 unsigned long long setno,tag,remainaddr,l2setno,l2tag,l2remainaddr;
	 remainaddr=addr/64;
	 tag=remainaddr/64;
	 setno=remainaddr%64;
	 int f1=1;
	 int temp=-1;
	 int zeroIndex=-1,lruIndex=-1;
 	  for(int i=0;i<8;i++)
 	 	{
 	 		if(L1tag[p][setno][i][0]==tag && L1tag[p][setno][i][1]!=0)
 	 		{
 	 			for(int j=0;j<8;j++)
 	 			{
 	 				if(i==j)
 	 				L1tag[p][setno][j][1]=1;
 	 				else if(L1tag[p][setno][j][1]!=0)
 	 				L1tag[p][setno][j][1]+=1;
 	 			}
 	 			f1=1;
 	 			l1Hit++;
 	 			//cout<<"L1hit"<<endl;
 	 			buffer[p].erase(addr);
 	 			break;
 	 		}
 	 		else
 	 		{
 	 			
 	 			/*long long t=L1tag[p][setno][i][1];
 	 			if(L1tag[p][setno][i][1]==0)
 	 			zeroIndex=i;
 	 			else if(temp<t)
 	 			{
 	 			lruIndex=i;
 	 			temp=t;
 	 			}*/
 	 			f1=0;
 	 		}
 	 	}
 	 	if(f1==0)
 	 	{
 	 		//cout<<"L1miss"<<endl;
 	 		l1Miss++;
 	 		readRequestHome(addr,p,gCount);
 	 	}
 }
 
 
 
 void writeRequestHome(unsigned long long addr,int p,unsigned long long gCount)
 {
 	 unsigned long long setno,tag,remainaddr,l2setno,l2tag,l2remainaddr;
	 remainaddr=addr/64;
	 tag=remainaddr/4096;
	 setno=remainaddr%4096;

	 int bankid=remainaddr%3;
	 l2mssg[bankid]=new l2Q();
  	 l2mssg[bankid]->msg="GETX";
  	 l2mssg[bankid]->addr=addr;
  	 l2mssg[bankid]->gCount=gCount;
  	 l2mssg[bankid]->cacheNumber=p;
  	 l2mssg[bankid]->next=NULL;
  	 int f1=1;
  	 for(int i=0;i<16;i++)
 	 	{
 	 		if(L2tag[setno][i][0]==tag && L2tag[setno][i][1]!=0)
 	 		{
 	 			f1=1;
 	 			l2Hit++;
 	 			//cout<<"l2Hit"<<endl;
 	 			if(bitVector[addr][8]==0)
 	 			{
 	 				bitVector[addr][8]=1;
 	 				l2mssg[bankid]->state=0;
 	 			}
 	 			else if(bitVector[addr][8]==2 || bitVector[addr][8]==3)
 	 			{
 	 				bitVector[addr][8]=3;
 	 				l2mssg[bankid]->state=3;
 	 			}
 	 			else if(bitVector[addr][8]==1)
 	 			{
 	 				bitVector[addr][8]=5;
 	 				l2mssg[bankid]->state=1;
 	 			}
 	 			else if(bitVector[addr][8]==5)
 	 			{
 	 				bitVector[addr][8]=5;
 	 				l2mssg[bankid]->state=5;
 	 				l1mssg[p]=new l1Q();
 					l1mssg[p]->msg="NACK";
 					l1mssg[p]->cacheNumber=p;
 					l1mssg[p]->addr=addr;
 					l1mssg[p]->gCount=gCount;
 					if(l1front[p]==NULL)
				  	{
				   		l1front[p]=l1rear[p]=l1mssg[p];
				   	}
					else
					{
					  	l1rear[p]->next=l1mssg[p];
					  	l1rear[p]=l1mssg[p];
					}
 					//NAC timer should be handle
 	 			}
 	 			
 	 			for(int j=0;j<16;j++)
 				{
	 				if(i==j)
 	 				L2tag[setno][j][1]=1;
 	 				else if(L2tag[setno][j][1]!=0)
	 				L2tag[setno][j][1]+=1;
		 	 	}
 	 			break;
 	 		}
 	 		else
 	 		{
 	 			f1=0;
 	 		}
 	 	}
 	 	if(f1==0)
 	 	{
 	 		
 	 		l2Miss++;
 	 		l2mssg[bankid]->state=6;
 	 		//cout<<"L2miss"<<endl;
 	 		// will do it later... invalidation and further
 	 	}
 	 	if(l2front[bankid]==NULL)
	  	 {
	   		l2front[bankid]=l2rear[bankid]=l2mssg[bankid];
	   	 }
	   	 else
		 {
	  	 	l2rear[bankid]->next=l2mssg[bankid];
	  		l2rear[bankid]=l2mssg[bankid];
	  	 }
  	 
  	 
 }	 
 void writeRequest(unsigned long long addr,int p,unsigned long long gCount)
 {
 	 unsigned long long setno,tag,remainaddr,l2setno,l2tag,l2remainaddr;
	 remainaddr=addr/64;
	 tag=remainaddr/64;
	 setno=remainaddr%64;
	 int f1=1;
	 int temp=-1;
	 int zeroIndex=-1,lruIndex=-1;
	
	 
	
 	  for(int i=0;i<8;i++)
 	 	{
 	 		if(L1tag[p][setno][i][0]==tag && L1tag[p][setno][i][1]!=0)
 	 		{
 	 			for(int j=0;j<8;j++)
 	 			{
 	 				if(i==j)
 	 				L1tag[p][setno][j][1]=1;
 	 				else if(L1tag[p][setno][j][1]!=0)
 	 				L1tag[p][setno][j][1]+=1;
 	 			}
 	 			f1=1;
 	 			l1Hit++;
 	 			buffer[p].erase(addr);    // block hit so erased from buffer
 	 			//cout<<"L1hit"<<endl;
 	 			if(bitVector[addr][8]==2)
 	 			{
 	 				bitVector[addr][8]=1;
 	 			}
 	 			else if(bitVector[addr][8]==3)
 	 			{
 	 				 int bankid=remainaddr%3;
					 l2mssg[bankid]=new l2Q();
				  	 l2mssg[bankid]->addr=addr;
				  	 l2mssg[bankid]->cacheNumber=p;
				  	 l2mssg[bankid]->next=NULL;
 	 				 l2mssg[bankid]->msg="UPGRADE";
 	 				 l2mssg[bankid]->gCount=gCount;
 	 				 if(l2front[bankid]==NULL)
				  	 {
				   		l2front[bankid]=l2rear[bankid]=l2mssg[bankid];
				   	 }
				   	 else
					 {
				  	 	l2rear[bankid]->next=l2mssg[bankid];
				  		l2rear[bankid]=l2mssg[bankid];
				  	 }
 	 			}
 	 			break;
 	 		}
 	 		else
 	 		{
 	 			f1=0;
 	 		}
 	 	}
 	 	if(f1==0)
 	 	{
 	 		l1Miss++;
 	 		//cout<<"l1miss"<<endl;
 	 		writeRequestHome(addr,p,gCount);
 	 	}
 } 
 
 int main()
 {
 unsigned long long tid,addr,count,counter[8]={0},tempcount=0;
 processor *front[8]={NULL},*rear[8]={NULL};
 processor *node[8]={NULL};                //trace queue
 string s;
 ifstream file ("assign3addrtrace4.txt");// reading of machine accesses
 
 int stop=0;
 while (file>>tid>>s>>addr>>count)
  {
  	//cout<<tid<<"  "<<s<<"  "<<addr<<"   "<<count<<endl;
  	victimMap[count]=addr;
  	rwMap[count]=s;
  	
  	node[tid]=new processor();
  	node[tid]->gCount=count;
  	node[tid]->rw=s;
  	node[tid]->addr=addr;
  	node[tid]->next=NULL;
  	if(front[tid]==NULL)
  	{
  		front[tid]=rear[tid]=node[tid];
  		counter[tid]++;
  	}
  	else
  	{
  		rear[tid]->next=node[tid];
  		rear[tid]=node[tid];
  		counter[tid]++;
  	}
  	//cout<<count<<endl;
  	stop++;
  	/*if(stop==200)
  	break;*/
  }
  cout<<"total machine traces : "<<stop<<endl;
  for(int z=0;z<8;z++)
  {
  //cout<<"thread "<<z<<" no. of requests "<<counter[z]<<endl;
  if(front[z]!=NULL)
  cout<<front[z]->gCount<<endl;
  }
  
  
 int qFlag=0; // flag=1 => all queues are empty
 map<unsigned long long, int> bypass; /// Which processor to bypass key having gCount
 	//unsigned long long bypassAddr;
 set<int> bypassProc;
 set<unsigned long long>bypassgCount;
 while(qFlag==0)
 {	
 	
 	qFlag=1;
 	
 	
 	for(auto time:timer)
 	{
 		if(time.second[0]==0)
 		{
 			bypass[time.second[2]]=time.second[1];  // processor 
 			bypassProc.insert(time.second[1]);	
 			bypassgCount.insert(time.second[2]);
 		}
 		else
 		time.second[0]--;  //timer decremented
 	}
 	
 	// checking all L2 mssg queues
 	
 	for(int i=0;i<8;i++)
 	{
 		if(l2front[i]==NULL)
 		{	
 			//qFlag=1;
 			continue;
 		}
 		else
 		{
 			int p=l2front[i]->cacheNumber;
 			
 			qFlag=0;
 			
 			if(l2front[i]->msg=="GET")
 			{
 				L2GET++;
 				if(l2front[i]->state==0)
 				{
 					l1mssg[p]=new l1Q();
 					l1mssg[p]->msg="PUT";
 					l1mssg[p]->addr=l2front[i]->addr;
 					l1mssg[p]->gCount=l2front[i]->gCount;
 					
 					bitVector[l2front[i]->addr][p]=1;
 					buffer[p].erase(l2front[i]->addr);
 					
 					if(l1front[p]==NULL)
				  	 {
				   		l1front[p]=l1rear[p]=l1mssg[p];
				   	 }
				   	 else
					 {
				  	 	l1rear[p]->next=l1mssg[p];
				  		l1rear[p]=l1mssg[p];
				  	 }
 					
 					unsigned long long taddr=l2front[i]->addr;
 					unsigned long long tsetno,ttag,tremainaddr;
					tremainaddr=taddr/64;
					ttag=tremainaddr/64;
					tsetno=tremainaddr%64;
					int f=1;
 					int temp=-1,lru=-1,zero=-1;
 					
 					for(int l=0;l<8;l++)
 					{
						unsigned long long t=L1tag[p][tsetno][l][1];
		 	 			if(L1tag[p][tsetno][l][1]==0)
		 	 			zero=l;
		 	 			else if(temp<t)
		 	 			{
		 	 			lru=l;
		 	 			temp=t;
		 	 			}
 					}
 					int r;
 					if(zero!=-1)
 					{
 						L1tag[p][tsetno][zero][0]=ttag;
 						r=zero;
 					}
 					else
 					{
 						L1tag[p][tsetno][lru][0]=ttag;
 						r=lru;
 					}
 					for(int j=0;j<8;j++)
 	 				{
	 	 				if(r==j)
	 	 				L1tag[p][tsetno][j][1]=1;
	 	 				else if(L1tag[p][tsetno][j][1]!=0)
	 	 				L1tag[p][tsetno][j][1]+=1;
 	 				}
 				}
 				else if(l2front[i]->state==2 || l2front[i]->state==3 )
 				{
 					l1mssg[p]=new l1Q();
 					l1mssg[p]->addr=l2front[i]->addr;
 					l1mssg[p]->msg="PUT";
 					l1mssg[p]->gCount=l2front[i]->gCount;
 					
 					bitVector[l2front[i]->addr][p]=1;
 					buffer[p].erase(l2front[i]->addr);
 					
 					if(l1front[p]==NULL)
				  	 {
				   		l1front[p]=l1rear[p]=l1mssg[p];
				   	 }
				   	 else
					 {
				  	 	l1rear[p]->next=l1mssg[p];
				  		l1rear[p]=l1mssg[p];
				  	 }
 					
 					unsigned long long taddr=l2front[i]->addr;
 					unsigned long long tsetno,ttag,tremainaddr;
					tremainaddr=taddr/64;
					ttag=tremainaddr/64;
					tsetno=tremainaddr%64;
					int f=1;
 					int temp=-1,lru=-1,zero=-1;
 					
 					for(int l=0;l<8;l++)
 					{
						unsigned long long t=L1tag[p][tsetno][l][1];
		 	 			if(L1tag[p][tsetno][l][1]==0)
		 	 			zero=l;
		 	 			else if(temp<t)
		 	 			{
		 	 			lru=l;
		 	 			temp=t;
		 	 			}
 					}
 					int r;
 					if(zero!=-1)
 					{
 						L1tag[p][tsetno][zero][0]=ttag;
 						r=zero;
 					}
 					else
 					{
 						L1tag[p][tsetno][lru][0]=ttag;
 						r=lru;
 					}
 					for(int j=0;j<8;j++)
 	 				{
	 	 				if(r==j)
	 	 				L1tag[p][tsetno][j][1]=1;
	 	 				else if(L1tag[p][tsetno][j][1]!=0)
	 	 				L1tag[p][tsetno][j][1]+=1;
 	 				} 					
 				}
 				else if(l2front[i]->state==1)
 				{
 					
 					for(int j=0;j<8;j++)
 					{
 						if(bitVector[l2front[i]->addr][j]==1)
 						{	
 							l1mssg[j]=new l1Q();
 							l1mssg[j]->msg="GET";
 							l1mssg[j]->addr=l2front[i]->addr;
 							l1mssg[j]->cacheNumber=p;
 							l1mssg[p]->gCount=l2front[i]->gCount;
 							
 							bitVector[l2front[i]->addr][p]=1;
 							
 							if(l1front[j]==NULL)
						  	{
						   		l1front[j]=l1rear[j]=l1mssg[j];
						   	}
						   	else
							{
						  	 	l1rear[j]->next=l1mssg[j];
						  		l1rear[j]=l1mssg[j];
						  	}
 							break;
 						}
 					}
 					 					
 				}
 				/*else if(l2front[i]->state==5)
 				{
 					l1mssg[l2front[i]->cacheNumber]=new l1Q();
 					l1mssg[l2front[i]->cacheNumber]->addr=l2front[i]->addr;
 					l1mssg[l2front[i]->cacheNumber]->msg="NACK";
 					
 					//// to do some work here
 				}*/
 				else if(l2front[i]->state==6)
 				{
 						 l1mssg[p]=new l1Q();
	 					 l1mssg[p]->msg="PUT";
	 					 l1mssg[p]->addr=l2front[i]->addr;
	 					 l1mssg[p]->gCount=l2front[i]->gCount;
	 					 
	 					 bitVector[l2front[i]->addr][p]=1;
	 					 bitVector[l2front[i]->addr][8]=2;
	 					 buffer[p].erase(l2front[i]->addr);
	 					 
	 					 if(l1front[p]==NULL)
					  	 {
					   		l1front[p]=l1rear[p]=l1mssg[p];
					   	 }
					   	 else
						 {
					  	 	l1rear[p]->next=l1mssg[p];
					  		l1rear[p]=l1mssg[p];
					  	 }
					  	 
 						 unsigned long long addr=l2front[i]->addr;
 					 	 unsigned long long setno,tag,remainaddr,l2setno,l2tag,l2remainaddr;
						 remainaddr=addr/64;
						 tag=remainaddr/4096;
						 setno=remainaddr%4096;
						 int zero=-1,lru=-1,temp=-1;
						 for(int j=0;j<16;j++)
						 {
						 	long long t=L2tag[setno][j][1];
			 	 			if(L2tag[setno][j][1]==0)
			 	 			zero=j;
			 	 			else if(temp<t)
			 	 			{
			 	 			lru=j;
			 	 			temp=t;
			 	 			}
						 }
						 int x;
						 //cout<<"ZERO " <<zero<<endl;
						 //cout<<"LRU " <<lru<<endl;
						 if(zero!=-1)
						 {
						 	
						 	L2tag[setno][zero][0]=tag;
						 	L2tag[setno][zero][3]=l2front[i]->gCount;
						 	x=zero;
						 	
						 	
						 	unsigned long long taddr=l2front[i]->addr;
		 					unsigned long long tsetno,ttag,tremainaddr;
							tremainaddr=taddr/64;
							ttag=tremainaddr/64;
							tsetno=tremainaddr%64;
							int f=1;
		 					int ttemp=-1,tlru=-1,tzero=-1;
		 					
		 					for(int l=0;l<8;l++)
		 					{
								long long t=L1tag[p][tsetno][l][1];
				 	 			if(L1tag[p][tsetno][l][1]==0)
				 	 			tzero=l;
				 	 			else if(ttemp<t)
				 	 			{
				 	 			tlru=l;
				 	 			ttemp=t;
				 	 			}
		 					}
		 					int r;
		 					if(tzero!=-1)
		 					{
		 						L1tag[p][tsetno][tzero][0]=ttag;
		 						r=tzero;
		 					}
		 					else
		 					{
		 						L1tag[p][tsetno][tlru][0]=ttag;
		 						r=tlru;
		 					}
		 					for(int j=0;j<8;j++)
		 	 				{
			 	 				if(r==j)
			 	 				L1tag[p][tsetno][j][1]=1;
			 	 				else if(L1tag[p][tsetno][j][1]!=0)
			 	 				L1tag[p][tsetno][j][1]+=1;
		 	 				}
		 	 				
		 	 				/*for(int j=0;j<16;j++)
		 	 				{
		 	 				        cout<<cycles<<endl;
			 	 				if(zero==j)
			 	 				L2tag[setno][j][1]=1;
			 	 				else if(L2tag[setno][j][1]!=0)
			 	 				L2tag[setno][j][1]+=1;
		 	 				}*/
						 }
						 else
						 {
						 	unsigned long long victimTag=L2tag[setno][lru][0];
						 	unsigned long long victimCount=L2tag[setno][lru][3];
						 	L2tag[setno][lru][0]=tag;
						 	L2tag[setno][lru][3]=l2front[i]->gCount;
						 	//    victim block removal and update in L1 will be done according to victim block
						 	
						 	unsigned long long victimAddr=victimMap[victimCount];
						 	unsigned long long vsetno,vtag,vremainaddr;
							vremainaddr=victimAddr/64;
							vtag=vremainaddr/64;
							vsetno=vremainaddr%64;
							//int f=1;
		 					//int ttemp=-1,tlru=-1,tzero=-1;
		 					
		 					for(int pr=0;pr<8;pr++)
		 					for(int l=0;l<8;l++)
		 					{
		 						if(L1tag[pr][vsetno][l][0]==vtag)
		 						{
		 							L1tag[pr][vsetno][l][0]=0;
		 							L1tag[pr][vsetno][l][1]=0;
		 							break;
		 						}
		 					}
		 					
		 					unsigned long long taddr=l2front[i]->addr;
		 					unsigned long long tsetno,ttag,tremainaddr;
							tremainaddr=taddr/64;
							ttag=tremainaddr/64;
							tsetno=tremainaddr%64;
							//int f=1;
		 					int ttemp=-1,tlru=-1,tzero=-1;
		 					
		 					for(int l=0;l<8;l++)
		 					{
								long long t=L1tag[p][tsetno][l][1];
				 	 			if(L1tag[p][tsetno][l][1]==0)
				 	 			tzero=l;
				 	 			else if(ttemp<t)
				 	 			{
				 	 			tlru=l;
				 	 			ttemp=t;
				 	 			}
		 					}
		 					int r;
		 					if(tzero!=-1)
		 					{
		 						L1tag[p][tsetno][tzero][0]=ttag;
		 						r=tzero;
		 					}
		 					else
		 					{
		 						L1tag[p][tsetno][tlru][0]=ttag;
		 						r=tlru;
		 					}
		 					for(int j=0;j<8;j++)
		 	 				{
			 	 				if(r==j)
			 	 				L1tag[p][tsetno][j][1]=1;
			 	 				else if(L1tag[p][tsetno][j][1]!=0)
			 	 				L1tag[p][tsetno][j][1]+=1;
		 	 				}
						 	
						 	x=lru;
						 }
						 
						        for(int j=0;j<16;j++)
		 	 				{
		 	 					//cout<<"L2 update "<<endl;
			 	 				if(x==j)
			 	 				L2tag[setno][j][1]=1;
			 	 				else if(L2tag[setno][j][1]!=0)
			 	 				L2tag[setno][j][1]+=1;
		 	 				}
						 
 				}
 				
 			}
 			else if(l2front[i]->msg=="GETX")
 			{
 				L2GETX++;
 				if(l2front[i]->state==0)
 				{
 					l1mssg[p]=new l1Q();
 					l1mssg[p]->msg="PUTX";
 					l1mssg[p]->addr=l2front[i]->addr;
 					l1mssg[p]->gCount=l2front[i]->gCount;
 					
 					bitVector[l2front[i]->addr][p]=1;
 					buffer[p].erase(l2front[i]->addr);
 					
 					if(l1front[p]==NULL)
				  	 {
				   		l1front[p]=l1rear[p]=l1mssg[p];
				   	 }
				   	 else
					 {
				  	 	l1rear[p]->next=l1mssg[p];
				  		l1rear[p]=l1mssg[p];
				  	 }
 					
 					unsigned long long taddr=l2front[i]->addr;
 					unsigned long long tsetno,ttag,tremainaddr;
					tremainaddr=taddr/64;
					ttag=tremainaddr/64;
					tsetno=tremainaddr%64;
					int f=1;
 					int temp=-1,lru=-1,zero=-1;
 					
 					for(int l=0;l<8;l++)
 					{
						long long t=L1tag[p][tsetno][l][1];
		 	 			if(L1tag[p][tsetno][l][1]==0)
		 	 			zero=l;
		 	 			else if(temp<t)
		 	 			{
		 	 			lru=l;
		 	 			temp=t;
		 	 			}
 					}
 					int r;
 					if(zero!=-1)
 					{
 						L1tag[p][tsetno][zero][0]=ttag;
 						r=zero;
 					}
 					else
 					{
 						L1tag[p][tsetno][lru][0]=ttag;
 						r=lru;
 					}
 					for(int j=0;j<8;j++)
 	 				{
	 	 				if(r==j)
	 	 				L1tag[p][tsetno][j][1]=1;
	 	 				else if(L1tag[p][tsetno][j][1]!=0)
	 	 				L1tag[p][tsetno][j][1]+=1;
 	 				}
 	 				
 				}
 				else if(l2front[i]->state==2 || l2front[i]->state==3 )
 				{
 					l1mssg[p]=new l1Q();
 					l1mssg[p]->addr=l2front[i]->addr;
 					l1mssg[p]->msg="PUTX";
 					l1mssg[p]->cacheNumber=p;
 					l1mssg[p]->gCount=l2front[i]->gCount;
 					
 					buffer[p].erase(l2front[i]->addr);
 					
 					if(l1front[p]==NULL)
				  	 {
				   		l1front[p]=l1rear[p]=l1mssg[p];
				   	 }
				   	 else
					 {
				  	 	l1rear[p]->next=l1mssg[p];
				  		l1rear[p]=l1mssg[p];
				  	 }
				  	 
 					for(int b=0;b<8;b++)
 					{
 						if(bitVector[l2front[i]->addr][b]==1)
 						{
 							l1mssg[b]=new l1Q();
 							l1mssg[b]->msg="INVALID";
 							l1mssg[b]->cacheNumber=p;
 							l1mssg[b]->addr=l2front[i]->addr;
 							l1mssg[b]->gCount=l2front[i]->gCount;
 							
 							if(l1front[b]==NULL)
						  	 {
						   		l1front[b]=l1rear[b]=l1mssg[b];
						   	 }
						   	 else
							 {
						  	 	l1rear[b]->next=l1mssg[b];
						  		l1rear[b]=l1mssg[b];
						  	 }
 						}
 						bitVector[l2front[i]->addr][b]=0;
 					}
 					bitVector[l2front[i]->addr][p]=1; 
 					
 					unsigned long long taddr=l2front[i]->addr;
 					unsigned long long tsetno,ttag,tremainaddr;
					tremainaddr=taddr/64;
					ttag=tremainaddr/64;
					tsetno=tremainaddr%64;
					int f=1;
 					int temp=-1,lru=-1,zero=-1;
 					
 					for(int l=0;l<8;l++)
 					{
						long long t=L1tag[p][tsetno][l][1];
		 	 			if(L1tag[p][tsetno][l][1]==0)
		 	 			zero=l;
		 	 			else if(temp<t)
		 	 			{
		 	 			lru=l;
		 	 			temp=t;
		 	 			}
 					}
 					int r;
 					if(zero!=-1)
 					{
 						L1tag[p][tsetno][zero][0]=ttag;
 						r=zero;
 					}
 					else
 					{
 						L1tag[p][tsetno][lru][0]=ttag;
 						r=lru;
 					}
 					for(int j=0;j<8;j++)
 	 				{
	 	 				if(r==j)
	 	 				L1tag[p][tsetno][j][1]=1;
	 	 				else if(L1tag[p][tsetno][j][1]!=0)
	 	 				L1tag[p][tsetno][j][1]+=1;
 	 				}					
 				}
 				else if(l2front[i]->state==1)
 				{
 					for(int j=0;j<8;j++)
 					{
 						if(bitVector[l2front[i]->addr][j]==1)
 						{	
 							l1mssg[j]=new l1Q();
 							l1mssg[j]->msg="GETX";
 							l1mssg[j]->addr=l2front[i]->addr;
 							l1mssg[j]->cacheNumber=p;
 							l1mssg[j]->gCount=l2front[i]->gCount;
 							
 							if(l1front[j]==NULL)
						  	 {
						   		l1front[j]=l1rear[j]=l1mssg[j];
						   	 }
						   	 else
							 {
						  	 	l1rear[j]->next=l1mssg[j];
						  		l1rear[j]=l1mssg[j];
						  	 }
 							
 							bitVector[l2front[i]->addr][p]=1;
 							//bitVector[l2front[i]->addr][j]=0;
 							
 								
 							break;
 						}
 					}
 				}
 				/*else if(l2front[i]->state==5)
 				{
 					
 				}*/
 				else if(l2front[i]->state==6)
 				{
 						 l1mssg[p]=new l1Q();
	 					 l1mssg[p]->msg="PUTX";
	 					 l1mssg[p]->addr=l2front[i]->addr;
	 					 l1mssg[p]->gCount=l2front[i]->gCount;
	 					 
	 					 bitVector[l2front[i]->addr][p]=1;
	 					 bitVector[l2front[i]->addr][8]=1;
	 					 buffer[p].erase(l2front[i]->addr);
	 					 
	 					 if(l1front[p]==NULL)
					  	 {
					   		l1front[p]=l1rear[p]=l1mssg[p];
					   	 }
					   	 else
						 {
					  	 	l1rear[p]->next=l1mssg[p];
					  		l1rear[p]=l1mssg[p];
					  	 }
					  	 
 						 unsigned long long addr=l2front[i]->addr;
 					 	 unsigned long long setno,tag,remainaddr,l2setno,l2tag,l2remainaddr;
						 remainaddr=addr/64;
						 tag=remainaddr/4096;
						 setno=remainaddr%4096;
						 int zero=-1,lru=-1,temp=-1;
						 for(int j=0;j<16;j++)
						 {
						 	long long t=L2tag[setno][j][1];
			 	 			if(L2tag[setno][j][1]==0)
			 	 			zero=j;
			 	 			else if(temp<t)
			 	 			{
			 	 			lru=j;
			 	 			temp=t;
			 	 			}
						 }
						 int x;
						 //cout<<"ZERO " <<zero<<endl;
						 //cout<<"LRU " <<lru<<endl;
						 if(zero!=-1)
						 {
						 	
						 	L2tag[setno][zero][0]=tag;
						 	L2tag[setno][zero][3]=l2front[i]->gCount;
						 	x=zero;
						 	
						 	
						 	unsigned long long taddr=l2front[i]->addr;
		 					unsigned long long tsetno,ttag,tremainaddr;
							tremainaddr=taddr/64;
							ttag=tremainaddr/64;
							tsetno=tremainaddr%64;
							int f=1;
		 					int ttemp=-1,tlru=-1,tzero=-1;
		 					
		 					for(int l=0;l<8;l++)
		 					{
								long long t=L1tag[p][tsetno][l][1];
				 	 			if(L1tag[p][tsetno][l][1]==0)
				 	 			tzero=l;
				 	 			else if(ttemp<t)
				 	 			{
				 	 			tlru=l;
				 	 			ttemp=t;
				 	 			}
		 					}
		 					int r;
		 					if(tzero!=-1)
		 					{
		 						L1tag[p][tsetno][tzero][0]=ttag;
		 						r=tzero;
		 					}
		 					else
		 					{
		 						L1tag[p][tsetno][tlru][0]=ttag;
		 						r=tlru;
		 					}
		 					for(int j=0;j<8;j++)
		 	 				{
			 	 				if(r==j)
			 	 				L1tag[p][tsetno][j][1]=1;
			 	 				else if(L1tag[p][tsetno][j][1]!=0)
			 	 				L1tag[p][tsetno][j][1]+=1;
		 	 				}
		 	 				
		 	 				/*for(int j=0;j<16;j++)
		 	 				{
		 	 				        cout<<cycles<<endl;
			 	 				if(zero==j)
			 	 				L2tag[setno][j][1]=1;
			 	 				else if(L2tag[setno][j][1]!=0)
			 	 				L2tag[setno][j][1]+=1;
		 	 				}*/
						 }
						 else
						 {
						 	unsigned long long victimTag=L2tag[setno][lru][0];
						 	unsigned long long victimCount=L2tag[setno][lru][3];
						 	L2tag[setno][lru][0]=tag;
						 	L2tag[setno][lru][3]=l2front[i]->gCount;
						 	//    victim block removal and update in L1 will be done according to victim block
						 	
						 	unsigned long long victimAddr=victimMap[victimCount];
						 	unsigned long long vsetno,vtag,vremainaddr;
							vremainaddr=victimAddr/64;
							vtag=vremainaddr/64;
							vsetno=vremainaddr%64;
							//int f=1;
		 					//int ttemp=-1,tlru=-1,tzero=-1;
		 					
		 					for(int pr=0;pr<8;pr++)
		 					for(int l=0;l<8;l++)
		 					{
		 						if(L1tag[pr][vsetno][l][0]==vtag)
		 						{
		 							L1tag[pr][vsetno][l][0]=0;
		 							L1tag[pr][vsetno][l][1]=0;
		 							break;
		 						}
		 					}
		 					
		 					unsigned long long taddr=l2front[i]->addr;
		 					unsigned long long tsetno,ttag,tremainaddr;
							tremainaddr=taddr/64;
							ttag=tremainaddr/64;
							tsetno=tremainaddr%64;
							int f=1;
		 					int ttemp=-1,tlru=-1,tzero=-1;
		 					
		 					for(int l=0;l<8;l++)
		 					{
								long long t=L1tag[p][tsetno][l][1];
				 	 			if(L1tag[p][tsetno][l][1]==0)
				 	 			tzero=l;
				 	 			else if(ttemp<t)
				 	 			{
				 	 			tlru=l;
				 	 			ttemp=t;
				 	 			}
		 					}
		 					int r;
		 					if(tzero!=-1)
		 					{
		 						L1tag[p][tsetno][tzero][0]=ttag;
		 						r=tzero;
		 					}
		 					else
		 					{
		 						L1tag[p][tsetno][tlru][0]=ttag;
		 						r=tlru;
		 					}
		 					for(int j=0;j<8;j++)
		 	 				{
			 	 				if(r==j)
			 	 				L1tag[p][tsetno][j][1]=1;
			 	 				else if(L1tag[p][tsetno][j][1]!=0)
			 	 				L1tag[p][tsetno][j][1]+=1;
		 	 				}
						 	
						 	x=lru;
						 }
						 
						        for(int j=0;j<16;j++)
		 	 				{
		 	 					//cout<<"L2 update "<<endl;
			 	 				if(x==j)
			 	 				L2tag[setno][j][1]=1;
			 	 				else if(L2tag[setno][j][1]!=0)
			 	 				L2tag[setno][j][1]+=1;
		 	 				}
						 
 				}
 				
 				
 			}
 			else if(l2front[i]->msg=="UPGRADE")
 			{
 				L2UPGRADE++;
 				l1mssg[p]=new l1Q();
 				l1mssg[p]->addr=l2front[i]->addr;
 				l1mssg[p]->msg="UPGRADE_ACK";
 				l1mssg[p]->cacheNumber=p;
 				l1mssg[p]->gCount=l2front[i]->gCount;
 				
 				if(l1front[p]==NULL)
			  	{
			   		l1front[p]=l1rear[p]=l1mssg[p];
			   	}
			   	else
				{
			  	 	l1rear[p]->next=l1mssg[p];
			  		l1rear[p]=l1mssg[p];
				}
 				for(int b=0;b<8;b++)
 				{
 					if(b!=p)
 					{ 
 						bitVector[l2front[i]->addr][b]=0;
 						l1mssg[b]=new l1Q();
 						l1mssg[b]->msg="INVALID";
 						l1mssg[b]->cacheNumber=p;
 						l1mssg[b]->addr=l2front[i]->addr;
 						l1mssg[b]->gCount=l2front[i]->gCount;
 						
 						if(l1front[b]==NULL)
				  		{
				   			l1front[b]=l1rear[b]=l1mssg[b];
				   	 	}
				   	 	else
					 	{
				  	 		l1rear[b]->next=l1mssg[b];
				  			l1rear[b]=l1mssg[b];
				  	 	}
 					}
 					else
 					bitVector[l2front[i]->addr][b]=1;
 				}
 				
 				
 			}
 			else if(l2front[i]->msg=="SHARING_WRITE_BACK")
 			{
 				L2SWB++;
 				// nothing to do here
 				qFlag=1;
 			}
 			else if(l2front[i]->msg=="ACK")
 			{
 				L2ACK++;
 				// nothing to do here
 				qFlag=1;
 			}
 			
 			l2front[i]=l2front[i]->next;
 			
 		}
 		
 	}
 	
 	// checking all L1 mssg queues
 	
 	for(int i=0;i<8;i++)
 	{
 		if(l1front[i]==NULL)
 		{	
 			//qFlag=1;
 			continue;
 		}
 		else
 		{
 			int p=l1front[i]->cacheNumber;
 			qFlag=0;
 			if(l1front[i]->msg=="INVALID")
 			{
 				L1INVALID[i]++;
 				l1mssg[p]=new l1Q();
 				l1mssg[p]->addr=l1front[i]->addr;
 				l1mssg[p]->msg="INVALID_ACK";
 				l1mssg[p]->cacheNumber=p;
 				l1mssg[p]->gCount=l1front[i]->gCount;
 				
 				if(l1front[p]==NULL)
			  	{
			   		l1front[p]=l1rear[p]=l1mssg[p];
			   	}
			   	else
				{
			  	 	l1rear[p]->next=l1mssg[p];
			  		l1rear[p]=l1mssg[p];
				}
				
				unsigned long long taddr=l1front[i]->addr;
				unsigned long long tsetno=(taddr/64)%64;
				unsigned long long ttag=(taddr/64)/64;
				
				
				for(int b=0;b<8;b++)
				{
					if(ttag==L1tag[i][tsetno][b][0])
					{
						L1tag[i][tsetno][b][0]=0;
						L1tag[i][tsetno][b][1]=0;
						break;
					}
				}
				
 			}
 			else if(l1front[i]->msg=="NACK")
 			{
 				L1NACK[i]++;
 				// timer setting .... do it later
 				timer[l1front[i]->addr][0]=20;
 				timer[l1front[i]->addr][1]=i;
 				timer[l1front[i]->addr][1]=l1front[i]->gCount;
 			}
 			else if(l1front[i]->msg=="GET")
 			{
 				L1GET[i]++;
 				// mssg PUT send
 				l1mssg[p]=new l1Q();
 				l1mssg[p]->addr=l1front[i]->addr;
 				l1mssg[p]->msg="PUT";
 				l1mssg[p]->cacheNumber=p;
 				l1mssg[p]->gCount=l1front[i]->gCount;
 				
 				if(l1front[p]==NULL)
			  	{
			   		l1front[p]=l1rear[p]=l1mssg[p];
			   	}
			   	else
				{
			  	 	l1rear[p]->next=l1mssg[p];
			  		l1rear[p]=l1mssg[p];
				}
				
				// mssg SWB send
				int bankid = (l1front[i]->addr/64)%3;
				
				l2mssg[bankid]=new l2Q();
 				l2mssg[bankid]->addr=l1front[i]->addr;
 				l2mssg[bankid]->msg="SHARING_WRITE_BACK";
 				l2mssg[bankid]->cacheNumber=i;
 				l2mssg[bankid]->gCount=l1front[i]->gCount;
 				
 				if(l2front[bankid]==NULL)
			  	{
			   		l2front[bankid]=l2rear[bankid]=l2mssg[bankid];
			   	}
			   	else
				{
			  	 	l2rear[bankid]->next=l2mssg[bankid];
			  		l2rear[bankid]=l2mssg[bankid];
				}
				
				// states and bit vector updated
				bitVector[l1front[i]->addr][8]=3;
				bitVector[l1front[i]->addr][i]=1;
				bitVector[l1front[i]->addr][p]=1;
				
				// block inserted in L1 cache
					unsigned long long taddr=l1front[i]->addr;
 					unsigned long long tsetno,ttag,tremainaddr;
					tremainaddr=taddr/64;
					ttag=tremainaddr/64;
					tsetno=tremainaddr%64;
					int f=1;
 					int temp=-1,lru=-1,zero=-1;
 					
 					for(int l=0;l<8;l++)
 					{
						long long t=L1tag[p][tsetno][l][1];
		 	 			if(L1tag[p][tsetno][l][1]==0)
		 	 			zero=l;
		 	 			else if(temp<t)
		 	 			{
		 	 			lru=l;
		 	 			temp=t;
		 	 			}
 					}
 					int r;
 					if(zero!=-1)
 					{
 						L1tag[p][tsetno][zero][0]=ttag;
 						r=zero;
 					}
 					else
 					{
 						L1tag[p][tsetno][lru][0]=ttag;
 						r=lru;
 					}
 					for(int j=0;j<8;j++)
 	 				{
	 	 				if(r==j)
	 	 				L1tag[p][tsetno][j][1]=1;
	 	 				else if(L1tag[p][tsetno][j][1]!=0)
	 	 				L1tag[p][tsetno][j][1]+=1;
 	 				}
 	 			// block erased from buffer
 	 			buffer[p].erase(taddr);	
				
 			}
 			else if(l1front[i]->msg=="GETX")
 			{
 				L1GETX[i]++;
 				// mssg PUTx send to requester
 				l1mssg[p]=new l1Q();
 				l1mssg[p]->addr=l1front[i]->addr;
 				l1mssg[p]->msg="PUTX";
 				l1mssg[p]->cacheNumber=p;
 				l1mssg[p]->gCount=l1front[i]->gCount;
 				
 				if(l1front[p]==NULL)
			  	{
			   		l1front[p]=l1rear[p]=l1mssg[p];
			   	}
			   	else
				{
			  	 	l1rear[p]->next=l1mssg[p];
			  		l1rear[p]=l1mssg[p];
				}
				
				//requester is now new owner, so what have to do??
				// state bit to be modify?? Yes??
				bitVector[l1front[i]->addr][8]=1;
				bitVector[l1front[i]->addr][p]=1;
				bitVector[l1front[i]->addr][i]=0;  //invalidation
				// do we invalid that block in this cache?? yes
				
				unsigned long long itag,isetno,iremainaddr;
				iremainaddr=l1front[i]->addr/64;
				isetno=iremainaddr%64;
				itag=iremainaddr/64;
				
				for(int j=0;j<8;j++)
				{
					if(L1tag[i][isetno][j][0]==itag)
					{
						L1tag[i][isetno][j][0]=0;
						L1tag[i][isetno][j][1]=0;
						//L1tag[i][isetno][j][2]=0;
						break;
					}
				}
				
				
				// do we have to send any ack to home node or requester about the invalidation of block??
				
				int bankid = (l1front[i]->addr/64)%3;
				
				l2mssg[bankid]=new l2Q();
 				l2mssg[bankid]->addr=l1front[i]->addr;
 				l2mssg[bankid]->msg="ACK";
 				l2mssg[bankid]->cacheNumber=i;
 				l2mssg[bankid]->gCount=l1front[i]->gCount;
 				
 				if(l2front[bankid]==NULL)
			  	{
			   		l2front[bankid]=l2rear[bankid]=l2mssg[bankid];
			   	}
			   	else
				{
			  	 	l2rear[bankid]->next=l2mssg[bankid];
			  		l2rear[bankid]=l2mssg[bankid];
				}
				
				
				
				// block inserted in L1 cache of requester
					unsigned long long taddr=l1front[i]->addr;
 					unsigned long long tsetno,ttag,tremainaddr;
					tremainaddr=taddr/64;
					ttag=tremainaddr/64;
					tsetno=tremainaddr%64;
					int f=1;
 					int temp=-1,lru=-1,zero=-1;
 					
 					for(int l=0;l<8;l++)
 					{
						unsigned long long t=L1tag[p][tsetno][l][1];
		 	 			if(L1tag[p][tsetno][l][1]==0)
		 	 			zero=l;
		 	 			else if(temp<t)
		 	 			{
		 	 			lru=l;
		 	 			temp=t;
		 	 			}
 					}
 					int r;
 					if(zero!=-1)
 					{
 						L1tag[p][tsetno][zero][0]=ttag;
 						r=zero;
 					}
 					else
 					{
 						L1tag[p][tsetno][lru][0]=ttag;
 						r=lru;
 					}
 					for(int j=0;j<8;j++)
 	 				{
	 	 				if(r==j)
	 	 				L1tag[p][tsetno][j][1]=1;
	 	 				else if(L1tag[p][tsetno][j][1]!=0)
	 	 				L1tag[p][tsetno][j][1]+=1;
 	 				}
 	 			// block erased from buffer
 	 			buffer[p].erase(taddr);
 	 				
				
 			}
 			else if(l1front[i]->msg=="PUTX")
 			{
 				L1PUTX[i]++;
 			}
 			else if(l1front[i]->msg=="PUT")
 			{
 				L1PUT[i]++;
 			}
 			else if(l1front[i]->msg=="UPGRADE_ACK")
 			{
 				L1UPGRADE_ACK[i]++;
 			}
 			else if(l1front[i]->msg=="INVALID_ACK")
 			{
 				L1INVALID_ACK[i]++;
 			}
 			l1front[i]=l1front[i]->next;
 		}
 	}
 	
 	int t=0;
 	while(t<=7)
 	{
 		if(front[t]!=NULL && front[t]->gCount==gId && bypassProc.find(t)==bypassProc.end())                       ////selecting processor which going to request in this cycle
 		{
 			qFlag=0;
 			getProcessor.insert(t);
 			gId++;
 			t=0;
 		}
 		else
 		{
 			t++;
 		}
 	}
 	for(auto g: bypassgCount)
 	{
 		string s=rwMap[g];
 		unsigned long long addr=victimMap[g];
 		int proc=bypass[g];
 		if(s=="R")
 		{
 			buffer[proc][addr]="R";
 			readRequest(addr,proc,g);
 		}
 		else
 		{
 			buffer[proc][addr]="W";
 			readRequest(addr,proc,g);
 		}
 	}
 	bypass.clear();
 	bypassgCount.clear();
 	bypassProc.clear();
 	for(auto p:getProcessor)                  ////serving request of processor which will run in this cycle
 	{
 		if(buffer[p].find(front[p]->addr)==buffer[p].end())
 		{
	 		if(front[p]->rw=="R")
	 		{	//cout<<"cycle : "<<cycles+1<<" read "<<front[p]->addr<<endl;
	 			buffer[p][front[p]->addr]="R";
	 			readRequest(front[p]->addr,p,front[p]->gCount);
	 		}
	 		else
	 		{
	 			//cout<<"cycle : "<<cycles+1<<" write "<<front[p]->addr<<endl;
	 			buffer[p][front[p]->addr]="W";
	 			writeRequest(front[p]->addr,p,front[p]->gCount);
	 		}
 		}
 		else
 		{
 					unsigned long long remainaddr=(front[p]->addr)/64;
 			                int bankid=remainaddr%3;
					 l2mssg[bankid]=new l2Q();
				  	 l2mssg[bankid]->addr=front[p]->addr;
				  	 l2mssg[bankid]->cacheNumber=p;
				  	 l2mssg[bankid]->next=NULL;
 	 				 l2mssg[bankid]->msg="UPGRADE";
 	 				 l2mssg[bankid]->gCount=front[p]->gCount;
 	 				 if(l2front[bankid]==NULL)
				  	 {
				   		l2front[bankid]=l2rear[bankid]=l2mssg[bankid];
				   	 }
				   	 else
					 {
				  	 	l2rear[bankid]->next=l2mssg[bankid];
				  		l2rear[bankid]=l2mssg[bankid];
				  	 }
 		}
 		
 		front[p]=front[p]->next;
 	}
 	getProcessor.clear();
 	//cout<<getProcessor.size()<<endl;
 	cycles++;
 	//cout<<"L2 hit : "<<l2Hit<<endl;
 }
  cout<<l1Hit<<" "<<l1Miss<<"  "<<l2Hit<<"   "<<l2Miss<<endl;
  cout<<"Total cycles "<<cycles<<endl;
  cout<<"\n\n";
  cout<<"L1 mssgs count "<<endl;
  cout<<"\n";
  cout<<"GET : "<<endl;
  for(int i=0;i<8;i++)
  {

  cout<<L1GET[i]<<"    ";
  }
  cout<<"\n\n";
  cout<<"PUT : "<<endl;
  for(int i=0;i<8;i++)
  {

  cout<<L1PUT[i]<<"    ";
  }
  cout<<"\n\n";
  cout<<"GETX  : "<<endl;
  for(int i=0;i<8;i++)
  {

  cout<<L1GETX[i]<<"    ";
  }
  cout<<"\n\n";
  cout<<"PUTX : "<<endl;
  for(int i=0;i<8;i++)
  {

  cout<<L1PUTX[i]<<"    ";
  }
  cout<<"\n\n";
  cout<<"INVALID : "<<endl;
  for(int i=0;i<8;i++)
  {

  cout<<L1INVALID[i]<<"    ";
  }
  cout<<"\n\n";
  cout<<"INVALID_ACK : "<<endl;
  for(int i=0;i<8;i++)
  {

  cout<<L1INVALID_ACK[i]<<"    ";
  }
  cout<<"\n\n";
  cout<<"UPGRADE_ACK : "<<endl;
  for(int i=0;i<8;i++)
  {
  cout<<L1UPGRADE_ACK[i]<<"    ";
  }
  cout<<"\n\n";
  cout<<"NACK : "<<endl;
  for(int i=0;i<8;i++)
  {
  cout<<L1NACK[i]<<"    ";
  }
cout<<"\n";
   
  
  cout<<"L2 mssgs count "<<endl;
  cout<<"GET : "<<L2GET<<endl;
  cout<<"GETX : "<<L2GETX<<endl;
  cout<<"SWB : "<<L2SWB<<endl;
  cout<<"UPGRADE : "<<L2UPGRADE<<endl;
  cout<<"ACK : "<<L2ACK<<endl;
  
  
 return 0;
 }
