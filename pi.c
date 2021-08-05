// pi.c
//
// Calculate pi to the required number of places using 
// the expansion of 16*arctan(1/5) - 4*arctan(1/239)
//
// Where arctan(x) =  x - x^3 + x^5 - x^7 + x^9  .................   
//                        ¯¯¯   ¯¯¯   ¯¯¯   ¯¯¯
//                         3     5     7     9
// 
// The task is divided into into four threads
// Each thread will sum the positive or negative terms 
// for the expansion of arctan(1/5) or arctan(1/239)
// At completion they will be combined to provide the final value for pi. 
//
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <pthread.h>

#define MAX_NO_ELEMENTS 120000

int32_t pi[12][MAX_NO_ELEMENTS];
int32_t no_of_places, elmend;

void divide(int32_t *arr1,int32_t *arr2,int32_t elmstr,int32_t elmend,uint64_t value);
void add(int32_t *arr1,int32_t *arr2,int32_t elmstr,int32_t elmend);
void subtract(int32_t *arr1,int32_t *arr2,int32_t elmstr,int32_t elmend);
void output(int32_t *arr1, int32_t linelength);

void * arctan_plus(void *arg);
void * arctan_minus(void *arg);

int main()
{
 pthread_t threads[4];
 uint32_t ul5=5;
 uint32_t ul239=239;
 no_of_places=1000000;
 elmend=(no_of_places/9)+2;
 
 if(elmend>=MAX_NO_ELEMENTS)
 {
   printf("Abort! Number of places too big! Increase MAX_NO_ELEMENTS.\n");
   return(0);
 }
 
 // pi = 16*arctan(1/5) - 4*arctan(1/239)
 // Initialize positive arctan(1/5) arrays
 *pi[0]=*pi[2]=16;
 divide(pi[0],pi[0],0,elmend, (uint64_t) 5);
 divide(pi[2],pi[2],0,elmend, (uint64_t) 5);
// Initialize negative arctan(1/5) arrays (5^3 =125)
 *pi[3]=*pi[5]=16;
 divide(pi[3],pi[3],0,elmend, (uint64_t) 125); 
 divide(pi[3],pi[5],0,elmend, (uint64_t) 3);
 // Initialize positive arctan(1/239) arrays
 *pi[6]=*pi[8]=4;
 divide(pi[6],pi[6],0,elmend, (uint64_t) 239);
 divide(pi[8],pi[8],0,elmend, (uint64_t) 239);
 // Initialize negative arctan(1/239) arrays (239^3 = 13651919)
 *pi[9]=*pi[11]=4;
 divide(pi[9],pi[9],0,elmend, (uint64_t) 13651919);
 divide(pi[9],pi[11],0,elmend,(uint64_t) 3);

 // Launch the processing threads
 pthread_create(&threads[0], NULL, &arctan_plus,  &ul5);
 pthread_create(&threads[1], NULL, &arctan_minus, &ul5);
 pthread_create(&threads[2], NULL, &arctan_plus,  &ul239);
 pthread_create(&threads[3], NULL, &arctan_minus, &ul239);

 // Wait until all four threads complete.
 pthread_join(threads[0],NULL); 
 pthread_join(threads[1],NULL);
 pthread_join(threads[2],NULL);
 pthread_join(threads[3],NULL);
 
 // Set pi[2] to have the value 16*arctan(1/5)
 subtract(pi[5],pi[2],0,elmend);
 // - 4*arctan(1/239)
 // Subtract the sum of the all positive terms.....,
 subtract(pi[8],pi[2],0,elmend);
 // then add the sum of the all negative terms
 add(pi[11],pi[2],0,elmend);
 // Complete.
 // The array pi[2] now holds the value of pi
 output(pi[2],100);
 return 0;
}

void * arctan_plus(void *arg)
{
 uint32_t n;
 uint64_t n_4;
 uint64_t next_odd_number=1;
 double lead_zeros,temp_lead_zeros;
 double logn4;
 int32_t no_of_zero_elements;
 int32_t int_lead_zeros;
 int32_t temp_zero_elements;
 int32_t *p0,*p1,*p2;

 n=*(uint32_t *) arg;
 n_4=n*n*n*n;
 logn4=log10((double)n_4);
 lead_zeros=log10((double)n);

 int_lead_zeros=(unsigned) lead_zeros;
 no_of_zero_elements=int_lead_zeros/9;

 p0 = (n==5)? pi[0] : pi[6];
 p1 = (n==5)? pi[1] : pi[7];
 p2 = (n==5)? pi[2] : pi[8];
 
 while(no_of_zero_elements<elmend)
 {
  next_odd_number+=4;
  lead_zeros+=logn4;
  temp_lead_zeros=lead_zeros+log10((double) next_odd_number);

  no_of_zero_elements=(uint32_t) temp_lead_zeros/9;
  temp_zero_elements=no_of_zero_elements-1;

  if(temp_zero_elements<0)
	  temp_zero_elements=0;

  divide(p0,p0,temp_zero_elements,elmend,n_4);
  divide(p0,p1,temp_zero_elements,elmend,next_odd_number);
  add(p1,p2,temp_zero_elements,elmend);
 }
 return arg;
}

void * arctan_minus(void *arg)
{
 uint32_t n;
 uint64_t n_4;
 uint64_t next_odd_number=3;
 double lead_zeros,temp_lead_zeros;
 double logn4;
 int32_t no_of_zero_elements;
 int32_t int_lead_zeros;
 int32_t temp_zero_elements;
 int32_t *p0,*p1,*p2;

 n=*(uint32_t *) arg;
 n_4=n*n*n*n;
 logn4=log10((double)n_4);
 lead_zeros=log10((double)n);

 int_lead_zeros=(unsigned) lead_zeros;
 no_of_zero_elements=int_lead_zeros/9;

 p0 = (n==5)? pi[3] : pi[9];
 p1 = (n==5)? pi[4] : pi[10];
 p2 = (n==5)? pi[5] : pi[11];

 while(no_of_zero_elements<elmend)
 {
  next_odd_number+=4;
  lead_zeros+=logn4;
  temp_lead_zeros=lead_zeros+log10((double) next_odd_number);

  no_of_zero_elements=(uint32_t) temp_lead_zeros/9;
  temp_zero_elements=no_of_zero_elements-1;

  if(temp_zero_elements<0)
	  temp_zero_elements=0;

  divide(p0,p0,temp_zero_elements,elmend,n_4);
  divide(p0,p1,temp_zero_elements,elmend, next_odd_number);
  add(p1,p2,temp_zero_elements,elmend);
  }
  return arg;
}

void add(int32_t *arr1,int32_t *arr2,int32_t elmstr,int32_t elmend)
{
	int32_t i,carry=0;
	
	for(i=elmend;i>=elmstr;i--)
	{
		arr2[i]=arr2[i]+arr1[i]+carry;
		carry=arr2[i]/1000000000;
		arr2[i]%=1000000000;
	}
}

void subtract(int32_t *arr1,int32_t *arr2,int32_t elmstr,int32_t elmend)
{
	int32_t i,carry=0;
	
	for(i=elmend;i>=elmstr;i--)
	{
		arr2[i]=arr2[i]-arr1[i]-carry;
		if(arr2[i]<0)
		{
		 carry=1;
	     arr2[i]+=1000000000;
		}
		else
		 carry=0;
	}
}

void divide(int32_t *arr1,int32_t *arr2,int32_t elmstr,int32_t elmend, uint64_t value)
{
	uint64_t product,carry=0;
	int32_t i;

 	for(i=elmstr;i<elmend;i++)
	{
	 product=1000000000*carry+arr1[i];
	 carry=product%value;
	 product/=value;
	 arr2[i]=product;
	}
}

void output(int32_t *arr1, int32_t linelength)
{
 int32_t i,j;
 int32_t charsout;
 FILE *pi_text;
 char pi_file[20];
 char buffer[15];

 sprintf(pi_file,"P%d.txt",no_of_places);

 if((pi_text=fopen(pi_file,"w"))==NULL)
	{
	 printf("Can't open %s for output!\n",pi_file);
	 return;
	}
 fprintf(pi_text,"%d.",arr1[0]);
 charsout=2;
 for(i=1;i<elmend-1;i++)
 {
	 sprintf(buffer,"%09ld",(long) arr1[i]);
	 for(j=0;j<9;j++)
	 {
	  fputc(buffer[j],pi_text);
	  charsout++;
	  charsout%=linelength;
	  if(charsout==0)
	    fputc('\n',pi_text);
	 }
 }
 fputc('\n',pi_text);
 fclose(pi_text);
}
