#include <stdio.h>
#include "pin.H"

FILE * trace;
PIN_LOCK pinLock;

long long unsigned count=1;

//Machine access from a memory access

VOID RecordMemRead(VOID * ip, uint64_t addr,THREADID tid, UINT32 Size)
{
    PIN_GetLock(&pinLock, tid+1);
    if(addr%64==0)
    {
	    while(Size)
	    {   
	    	if(Size>=8)
	    	{
	    	   Size-=8;
	    	   fprintf(trace,"%d R %lu %llu\n",tid, addr, count);
	    	   addr+=8;
	    	}
	    	else if(Size>=4)
	    	{
	    	   Size-=4;
	    	   fprintf(trace,"%d R %lu %llu\n",tid, addr, count);
	    	   addr+=4;
	    	}
	    	else if(Size>=2)
	    	{
	    	  Size-=2;
	    	  fprintf(trace,"%d R %lu %llu\n",tid, addr, count);
	    	  addr+=2;
	    	}
	    	else
	    	{
	    	  Size--;
	    	  fprintf(trace,"%d R %lu %llu\n",tid, addr, count);
	    	  addr+=1;
	    	}
	    	count++;
	    	fflush(trace);
	    }
    }
    else
    {
    	   UINT32 remainSize=64-(addr%64);
    	   if(remainSize<Size)
    	   {
    	        Size=Size-remainSize;
    	        
    	        while(remainSize)
	        {       
		    	if(remainSize>=8)
		    	{
		    	   remainSize-=8;
		    	   fprintf(trace,"%d R %lu %llu\n",tid, addr, count);
		    	   addr+=8;
		    	}
		    	else if(remainSize>=4)
		    	{
		    	   remainSize-=4;
		    	   fprintf(trace,"%d R %lu %llu\n",tid, addr, count);
		    	   addr+=4;
		    	}
		    	else if(remainSize>=2)
		    	{
		    	  remainSize-=2;
		    	  fprintf(trace,"%d R %lu %llu\n",tid, addr, count);
		    	  addr+=2;
		    	}
		    	else
		    	{
		    	  remainSize--;
		    	  fprintf(trace,"%d R %lu %llu\n",tid, addr, count);
		    	  addr+=1;
		    	}
		    	count++;
		    	fflush(trace);
	        }
	       
    	   }
    	   
    	       while(Size)
	       {        
		    	if(Size>=8)
		    	{
		    	   Size-=8;
		    	   fprintf(trace,"%d R %lu %llu\n",tid, addr, count);
		    	   addr+=8;
		    	}
		    	else if(Size>=4)
		    	{
		    	   Size-=4;
		    	   fprintf(trace,"%d R %lu %llu\n",tid, addr, count);
		    	   addr+=4;
		    	}
		    	else if(Size>=2)
		    	{
		    	  Size-=2;
		    	  fprintf(trace,"%d R %lu %llu\n",tid, addr, count);
		    	  addr+=2;
		    	}
		    	else
		    	{
		    	  Size--;
		    	  fprintf(trace,"%d R %lu %llu\n",tid, addr, count);
		    	  addr+=1;
		    	}
		    	count++;
		    	fflush(trace);
	       }
   
    }
    
   fflush(trace);
   PIN_ReleaseLock(&pinLock);
}

// Machine access from memory access
VOID RecordMemWrite(VOID * ip, uint64_t addr,THREADID tid, UINT32 Size)
{
    PIN_GetLock(&pinLock, tid+1);
    //fprintf(trace,"%d %p: W %p Size = %u\n",tid, ip, addr, Size);
    if(addr%64==0)
    {
	    while(Size)
	    {   
	    	if(Size>=8)
	    	{
	    	   Size-=8;
	    	   fprintf(trace,"%d W %lu %llu\n",tid, addr, count);
	    	   addr+=8;
	    	}
	    	else if(Size>=4)
	    	{
	    	   Size-=4;
	    	   fprintf(trace,"%d W %lu %llu\n",tid, addr, count);
	    	   addr+=4;
	    	}
	    	else if(Size>=2)
	    	{
	    	  Size-=2;
	    	  fprintf(trace,"%d W %lu %llu\n",tid, addr, count);
	    	  addr+=2;
	    	}
	    	else
	    	{
	    	  Size--;
	    	  fprintf(trace,"%d W %lu %llu\n",tid, addr, count);
	    	  addr+=1;
	    	}
	    	count++;
	    	fflush(trace);
	    }
    }
    else
    {
    	   UINT32 remainSize=64-(addr%64);
    	   if(remainSize<Size)
    	   {
    	        Size=Size-remainSize;
    	        
    	        while(remainSize)
	        {       
		    	if(remainSize>=8)
		    	{
		    	   remainSize-=8;
		    	   fprintf(trace,"%d W %lu %llu\n",tid, addr, count);
		    	   addr+=8;
		    	}
		    	else if(remainSize>=4)
		    	{
		    	   remainSize-=4;
		    	   fprintf(trace,"%d W %lu %llu\n",tid, addr, count);
		    	   addr+=4;
		    	}
		    	else if(remainSize>=2)
		    	{
		    	  remainSize-=2;
		    	  fprintf(trace,"%d W %lu %llu\n",tid, addr, count);
		    	  addr+=2;
		    	}
		    	else
		    	{
		    	  remainSize--;
		    	  fprintf(trace,"%d W %lu %llu\n",tid, addr, count);
		    	  addr+=1;
		    	}
		    	count++;
		    	fflush(trace);
	        }
	       
    	   }
    	   
    	       while(Size)
	       {         
		    	if(Size>=8)
		    	{
		    	   Size-=8;
		    	   fprintf(trace,"%d W %lu %llu\n",tid, addr, count);
		    	   addr+=8;
		    	}
		    	else if(Size>=4)
		    	{
		    	   Size-=4;
		    	   fprintf(trace,"%d W %lu %llu\n",tid, addr, count);
		    	   addr+=4;
		    	}
		    	else if(Size>=2)
		    	{
		    	  Size-=2;
		    	  fprintf(trace,"%d W %lu %llu\n",tid, addr, count);
		    	  addr+=2;
		    	}
		    	else
		    	{
		    	  Size--;
		    	  fprintf(trace,"%d W %lu %llu\n",tid, addr, count);
		    	  addr+=1;
		    	}
		    	count++;
		    	fflush(trace);
	       }
   
    }
    
    fflush(trace);
    PIN_ReleaseLock(&pinLock);
}


// This routine is executed every time a thread is created.
VOID ThreadStart(THREADID tid, CONTEXT *ctxt, INT32 flags, VOID *v)
{
    PIN_GetLock(&pinLock, tid+1);
    fprintf(stdout, "thread begin %d\n",tid);
    fflush(stdout);
    PIN_ReleaseLock(&pinLock);
}

// This routine is executed every time a thread is destroyed.
VOID ThreadFini(THREADID tid, const CONTEXT *ctxt, INT32 code, VOID *v)
{
    PIN_GetLock(&pinLock, tid+1);
    fprintf(stdout, "thread end %d code %d\n",tid, code);
    fflush(stdout);
    PIN_ReleaseLock(&pinLock);
}

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{
    // Insert a call to printip before every instruction, and pass it the IP
     UINT32 memOperands = INS_MemoryOperandCount(ins);
    //INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)printip, IARG_INST_PTR, IARG_THREAD_ID, IARG_END);
    
      // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        UINT32 Size=INS_MemoryOperandSize(ins, memOp);
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,IARG_THREAD_ID,
                IARG_UINT32, Size,
                IARG_END);
        }
        // Note that in some architectures a single memory operand can be 
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,IARG_THREAD_ID,
                IARG_UINT32, Size,
                IARG_END);
        }
    }
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    
   fprintf(stdout, "Total Number of machine accesses : %llu\n",count);
    fflush(stdout);
    
    fclose(trace);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR("This Pintool prints the IPs of every instruction executed\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char * argv[])
{
    trace = fopen("assign3addrtrace1.txt", "w");                 ///File name to generate the text file, need to change for other programs
    
    PIN_InitLock(&pinLock);

    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    PIN_AddThreadStartFunction(ThreadStart, 0);
    PIN_AddThreadFiniFunction(ThreadFini, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
