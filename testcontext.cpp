/* testcontext.c : demo of win32 ucontex_t operations */
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
 
ucontext_t auc,buc,mainuc;
 
void a()
{
	int	i;
 
    for (i=0; i<10; i++) {
		printf("a");
		swapcontext(&auc,&buc);			/* switch to thread B */
	}
	printf("\nswitching to main\n");
	swapcontext(&auc,&mainuc);			/* switch to main thread */
}
 
void b()
{
	int	i;
 
	for (i=0; i<10; i++) {
		printf("b");
		swapcontext(&buc,&auc);			/* switch to thread A */
	}
}
 
int main1(void)
{
	printf("start\n");             /* main thread starts */
 
	/* Set up context for thread A (Unix code, see manpages) */
	getcontext(&auc);
	auc.uc_stack.ss_size = 16*1024;
	if((auc.uc_stack.ss_sp=malloc(auc.uc_stack.ss_size))==NULL)
		perror("malloc"),exit(1);
	auc.uc_stack.ss_flags=0;
	makecontext(&auc,a,0);
 
	/* Set up context for thread B */
	getcontext(&buc);
	buc.uc_stack.ss_size = 16*1024;
	if((buc.uc_stack.ss_sp=malloc(buc.uc_stack.ss_size))==NULL)
		perror("malloc"),exit(1);
	buc.uc_stack.ss_flags=0;
	makecontext(&buc,b,0);
	
	/* Switch to A */
	//getcontext(&mainuc);           /* Save the context of main thread */
	swapcontext(&mainuc,&auc);     /* Switch to thread A */
	
	printf("\ndone\n");            /* Execution control returned to main thread */
	
	return 0;
}

void crt_test();
int main(void)
{
	crt_test();
	//main1();
}
