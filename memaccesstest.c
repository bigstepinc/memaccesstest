/*
memaccesstest.c
Author Alex Bordei at bigstep
All rights reserved. 
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include "config.h"

#ifndef __MACH__
#include <time.h>
#else
#include <mach/mach_time.h>
#endif

struct l{
	struct l *n;
	struct l *p;
	long int pad[NPAD-1];

};

uint64_t rand64()
{
	// Combin 4 parts of low 16-bit of each rand()
	uint64_t R0 = (uint64_t)random() << 48;
	uint64_t R1 = (uint64_t)random() << 48 >> 16;
	uint64_t R2 = (uint64_t)random() << 48 >> 32;
	uint64_t R3 = (uint64_t)random() << 48 >> 48;
	return R0 | R1 | R2 | R3;
};

#ifdef DEBUG
void print_ll_lin(struct l *root,unsigned long working_set_size)
{
	struct l *current=root->n;
	unsigned long count;
	printf("0");
	count=0;
	while(current!=root)
	{
		printf("->%ld",current-root);
		current=current->n;
		count++;
		if(count>working_set_size/sizeof(struct l))
		{
			printf("Error:cycle!");
			break;
		}
			
	}	
	printf("\n");
}
void print_ll_lin_back(struct l *root,unsigned long working_set_size)
{
	struct l *current=root->p;
	unsigned long count;
	printf("0");
	count=0;
	while(current!=root)
	{
		printf("->%ld",current-root);
		current=current->p;
		count++;
		if(count>working_set_size/sizeof(struct l))
		{
			printf("Error:cycle!");
			break;
		}
			
	}	
	printf("\n");
}
void print_ll(struct l *root,unsigned long working_set_size)
{
	unsigned long i;
	for(i=0;i<(working_set_size/sizeof(struct l));i++)
		printf("DEBUG: %ld<-%ld->%ld\n",(root+i)->p-root,i,((root+i)->n)-root);		
	printf("DEBUG ");
	print_ll_lin(root, working_set_size);
	printf("DEBUG ");
	print_ll_lin_back(root, working_set_size);

}
#endif

void build_sequencial_ll(struct l *root, long working_set_size)
{

		unsigned long i;

		struct l *current;


		for(i=0;i<(working_set_size/sizeof(struct l));i++)
		{
			current=root+i;	
			if(i==(working_set_size/sizeof(struct l)-1))
				current->n=root;
			else
				current->n=current+1;
			if(i==0)
				current->p=root+working_set_size/sizeof(struct l)-1;
			else
				current->p=current-1;
		} 
}

void build_random_ll(struct l *root, long working_set_size)
{

		unsigned long i;


		struct l *a;
		struct l *b;
		struct l *oldan;
		struct l *oldbn;
		struct l *oldap;
		struct l *oldbp;

		unsigned long rnd;
		unsigned long tries;

		build_sequencial_ll(root, working_set_size);

		//construct the array of elements 
		//link the list to the next element in the array

		for(i=1;i<(working_set_size/sizeof(struct l)-1);i++)
		{

			a=(root+i);
			b=a;
			tries=5;	
			while((b==a || b==root) && tries)
			{
				rnd=rand64() % (working_set_size/sizeof(struct l)-1);	
				b=root+rnd;
				tries--;
			}
			if(!tries)
				continue;

			
			oldan=a->n; //a->n is lost along the way
			oldbn=b->n;  //b->n is lost along the way
			oldap=a->p; //a->p is lost along the way
			oldbp=b->p;  //b->p is lost along the way

			a->p->n=b;
			b->p->n=a;

			if(b!=oldan)
			{
				b->n=oldan;    		
				oldan->p=b;
			}
			else
			{
				b->n=a;
				a->p=b;
			}

			if(a!=oldbn)
			{
				a->n=oldbn;		
				oldbn->p=a;
			}
			else
			{
				a->n=b;
				b->p=a;
			}

			if(b!=oldap)
				b->p=oldap; 
			else
				b->p=a;

			if(a!=oldbp)
				a->p=oldbp;
			else
				a->p=b;


#ifdef DEBUG
			printf("DEBUG: Round i=%ld\n",i);		
			printf("DEBUG: A=%ld B=%ld\n",a-root,b-root);
			print_ll(root, working_set_size);
#endif
		} 

}



struct thread_start_data
{
	struct l *root;
	unsigned long working_set_size;
	int thread_index;		
};

void * walk(void *param)
{

		struct thread_start_data *data=(struct thread_start_data*)param;
		unsigned long start,end;


		struct l *root=data->root;	
		struct l* current=root->n;			

		#ifdef __MACH__
		start=mach_absolute_time();
		#else
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC_RAW,&ts);
		start=ts.tv_nsec; 
		#endif
	

		/////////////////////////////////////
		//traverse the linked list
		
		while(current!=root)	
		{
#ifdef ADDTEST
			current->pad[0]+=current->n->pad[0];
#endif

#ifdef INCTEST
			current->pad[0]++;
#endif
			current=current->n;	
		}

			
		/////////////////////////////////////
#ifdef __MACH__
		end=mach_absolute_time();
#else
		clock_gettime(CLOCK_MONOTONIC_RAW,&ts);
		end=ts.tv_nsec; 
#endif
	

		printf("%d,%ld,%ld,%ld\n",data->thread_index,data->working_set_size, end-start, data->working_set_size/sizeof(struct l));

		free(data);
		return NULL;
}

int main(int argc,char **argv)
{
#ifdef DEBUG
	printf("DEBUG:sizeof(struct l)=%ld\n",sizeof(struct l));
#endif
	struct l *root;
	unsigned long working_set_size;
	int i;
	struct thread_start_data *tdata;
	
	pthread_t threads[NTHREADS];

	root=calloc(MAX_WSS/sizeof(struct l),sizeof(struct l));	
		
	for(working_set_size=MIN_WSS;working_set_size<MAX_WSS;working_set_size=working_set_size<<1)
	{

#ifdef DEBUG
		printf("DEBUG:Working set size=%ld elements=%ld\n",working_set_size,working_set_size/sizeof(struct l));
#endif

#ifdef RANDOM_LIST
		build_random_ll(root, working_set_size);
#else
		build_sequencial_ll(root, working_set_size);
#endif

#if NTHREADS>0			
		for(i=0;i<NTHREADS;i++)
		{
			tdata= malloc(sizeof(struct thread_start_data));
			tdata->thread_index=i;
			tdata->root=root;
			tdata->working_set_size=working_set_size;
			pthread_create(&threads[i], NULL, walk, (void*) tdata);
		}

		for(i=0;i<NTHREADS;i++)
			pthread_join(threads[i],NULL);					
#else
		tdata= malloc(sizeof(struct thread_start_data));
		tdata->thread_index=0;
		tdata->root=root;
		tdata->working_set_size=working_set_size;
		
		walk((void*)tdata);	
#endif
	}


	free(root);	


}
