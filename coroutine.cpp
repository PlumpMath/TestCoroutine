#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <assert.h>

//要点：
//1. 在resume时要备份当前ctx，
//2.

struct crt{
	ucontext_t ctx;
	int line;
};

#define	CRT_MAX_STK	20

crt g_crt_stk[CRT_MAX_STK];
int g_crt_stk_sz =0;

int crt_create_ex(crt *pcrt, void (*f)(), int line)
{
	int ret =getcontext(&pcrt->ctx);
	if(ret<0) goto Exit;
	pcrt->ctx.uc_stack.ss_size = 16*1024;
	if((pcrt->ctx.uc_stack.ss_sp=malloc(pcrt->ctx.uc_stack.ss_size))==NULL)
	{
		 ret =-2;goto Exit;
	}
	pcrt->ctx.uc_stack.ss_flags=0;
	ret =makecontext(&pcrt->ctx,f,0);
	if(ret<0) goto Exit;

	pcrt->line =line;
Exit:
	return ret;
}

#define crt_create(pcrt,f) crt_create_ex(pcrt,f,__LINE__);

int crt_resume(crt *pcrt)
{
	if(g_crt_stk_sz>=CRT_MAX_STK)
		return -1;

	// bkup current ctx
	crt ctx;
	int ret =getcontext(&ctx.ctx);
	if(ret<0) goto Exit;

	g_crt_stk[g_crt_stk_sz++] =ctx;
	
	// jump dest
	ret = setcontext(&pcrt->ctx);
Exit:
	return ret;
}

int crt_yield()
{
	if(g_crt_stk_sz<=0)
		return -1;

	g_crt_stk_sz--;
	crt mcrt =g_crt_stk[g_crt_stk_sz];
	return crt_resume(&mcrt);
}



void f()
{
	for (int i=1; i<10; i++)
	{
		printf("co %d\n", i);
		int ret =crt_yield();
		assert( ret==0);
	}
}

void crt_test()
{
	crt crt;
	int ret =crt_create(&crt, f);
	assert( ret==0);

	for(int j=0; j<4;j++)
	{
		printf("crt_test %d\n", j);
		ret =crt_resume(&crt);
		assert( ret==0);
	}
}
