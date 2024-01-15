//DEFINE STACK OFFSET
//DEFINE RUN THREAD
//EXTERN ONLY FOR ASM functions

extern void runFirstThread(void); //LAB 2 addition for running single threads - directing to asmDump function
// Lab 2 given func/handler
#define RUN_FIRST_THREAD  0x3
#define YIELD 0x7
#define stack_spacing 0x200
//#define stack_pool 0x4000
#define max_stacks (0x4000 - stack_spacing)/stack_spacing

//Setting priorities  to tell chip SVC has higher priority then PendSV
#define SHPR2 *(uint32_t*)0xE000ED1C //for setting SVC priority, bits 31-24
#define SHPR3 *(uint32_t*)0xE000ED20 // PendSV is bits 23-16
#define _ICSR *(uint32_t*)0xE000ED04 //This lets us trigger PendSV


// Do not need all of these remove any unused
void osKernelInitilize();
void osCreateThread();
void osKernelStart();
void osSched();
void osYield();
