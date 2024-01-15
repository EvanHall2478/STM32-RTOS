#include "kernel.h"
#include <stdio.h>
#include <stdbool.h>
#include "main.h"


uint32_t* stackptr;
uint32_t* MSP_INIT_VAL;
int stack_pool; // total stack pool size
uint32_t* threadstackptr;
int currentThreadIndex;
int numThreadsRunning;

// Thread meta data structure storing all thread information
typedef struct k_thread{
	uint32_t* sp; //stack pointer
	void (*thread_function)(void*); //function pointer
}thread;

//Set up an array of thread data with length as long as max amount of threads that can be created
struct k_thread arr_threads[max_stacks];


// Set up anything needed before running threads
void osKernelInitilize()
{
	//set the priority of PendSV to almost the weakest
	SHPR3 |= 0xFE << 16; //shift the constant 0xFE 16 bits to set PendSV priority
	SHPR2 |= 0xFDU << 24; //Set the priority of SVC higher than PendSV

	//Initialize global variables
	numThreadsRunning = 0;
	currentThreadIndex = 0;
	stack_pool = 0x4000; // total stack pool size

	MSP_INIT_VAL = *(uint32_t**)0x0;
	stackptr = (uint32_t*)MSP_INIT_VAL; // Set stack pointer intially to MSP stack pointer location,

	stack_pool -= stack_spacing; //0x200 for MSP 0x200 for first thread
}

//Checks to ensure that there is space remaining in the stack pool to create a new thread stack
int allocate_Stack()
{
	//Check the stack pool to ensure it can fit another stack
	if (stack_pool - stack_spacing<0)
	{
		stackptr = (uint32_t*)-1; //Error flag //NEED TO CHANGE TO AN EXIT() or somethign similar
		return (uint32_t)stackptr;
	}

	stackptr -= stack_spacing; // THIS IS LIKELY WHERE THE ERROR LIES!!! SINCE MOVING TO
	stack_pool -= stack_spacing;
	return (uint32_t)stackptr;
}



//typedef void* (*ThreadFunction)(void*);  // Define a function pointer type



//When called first create stack pointer if space in pool
//Then set up stack at that pointer
void osCreateThread(void*(fcn)(void*))
{
	threadstackptr = (uint32_t*)allocate_Stack();

	// If not enough space in stack pool then return false
	if (threadstackptr==(uint32_t*)-1)
	{
		printf("Not enough space in stack pool!");
		//return;
		return false;
	}

	//update arr_threads TCB at the given index
	arr_threads[currentThreadIndex].sp = threadstackptr;
	arr_threads[currentThreadIndex].thread_function = (void(*)(void*))fcn;

	//Set up the threads stack
	*(--arr_threads[currentThreadIndex].sp) = 1<<24; //A magic number, this is xPSR
	*(--arr_threads[currentThreadIndex].sp) = (uint32_t)fcn;//the function name (PC)
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number (LR)
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number (R12)
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number (R3)
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number (R2)
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number (R1)
	*(--arr_threads[currentThreadIndex].sp) = 0xA;
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number (R11)
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number
	*(--arr_threads[currentThreadIndex].sp) = 0xA; //An arbitrary number (R4)

	// Update the thread tracking variables
	numThreadsRunning++;
	currentThreadIndex++;

	return true;
}



// Lab 2 given func/handler
void SVC_Handler_Main(unsigned int *svc_args)
 {
	  unsigned int svc_number;

	  /*
	   * Stack contains:
	   * r0, r1, r2, r3, r12, r14, the return address and xPSR
	   * First argument (r0) is svc_args[0]
	   */
	  svc_number = ((char*)svc_args[6])[-2];
	  switch(svc_number)
	  {
	  //Case for running the first thread (LAB 2)
	  	  case RUN_FIRST_THREAD:
	  		  __set_PSP((uint32_t)arr_threads[currentThreadIndex].sp); //set psp the current thread index
			  runFirstThread();
			  break;

	//Case for osYield
	//Pend an interrupt to do the context switch triggering PendSV
	  	case YIELD:
			  _ICSR |= 1<<28;
			  __asm("isb");
			  break;

	//Unknown System call
	  	  default:
			  break;
	  }
 }


//System calls

 //System call to run a thread
// void run_first_thread(void)
// {
//	 __asm("SVC #3");
// }

 //Wrapper function for a new system call
  void osYield(void)
  {
 	 __asm("SVC #7");
  }



 // Run first thread
 // Wrap system call
 void osKernelStart()
 {
	 currentThreadIndex = 0; // Where should I initialize the first thread to run

	 __asm("SVC #3"); //System call to run the thread
 }

 // Save the stack pointer of the current thread in the struct
 // Change the current thread to the next thread
 // Set PSP to the new current threads stack pointer
 void osSched()
 {
	 //Update the Thread meta data struct for under the registers
	 arr_threads[currentThreadIndex].sp =(uint32_t*)(__get_PSP() - 8*4);

	 //Change the current thread index from 0 to 1 after each osYield call
	 currentThreadIndex++;
	 currentThreadIndex %= numThreadsRunning;

	 //Set the PSP value to the thread stack pointer at the current index of the new thread indexs
	 //meta data struct
	 __set_PSP((uint32_t)arr_threads[currentThreadIndex].sp);
 	return;
 }



