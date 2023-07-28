#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

//We are 3 students at hadar group, and we learned and made that task together,
//so it might be simillarity between our codes. We have talked about that with hadar 
//and he said that it is ok by him. Our code is not the same but with some similarity,
//everyone of us wrote his own code on his own computer but when we have been together.  


void parseargs(char *argv[], int argc, int *lval, int *uval, int* nval, int* pval);
int isprime(int n);
void unmapAndCheck (char* flagarr, size_t size);
void waitForAllSons(int pval);
void countPrimes(int* count,char* flagarr, int lval, int uval);

int main (int argc, char **argv)
{
  int lval = 1;
  int uval = 100;
  int nval = 10;
  int pval = 4;
  char *flagarr = NULL;
  int num=0;
  size_t size;
  int count = 0;

  // Parse arguments
  parseargs(argv, argc, &lval, &uval,&nval, &pval);
  if (uval < lval)
  {
    fprintf(stderr, "Upper bound should not be smaller then lower bound\n");
    exit(1);
  }    
  if (lval < 2)
  {
    lval = 2;
    uval = (uval > 1) ? uval : 1;
  }
  if (pval > uval-lval+1)
  	pval = uval-lval;
  	
  size = sizeof(char) * (uval-lval+1);
  
  flagarr = mmap(NULL, size, PROT_READ | PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   if(flagarr == MAP_FAILED)
   	exit(1);
   	
  for(int i=0;i<pval;i++)
  {
  	switch (fork())
  	{
  		case -1:
  			exit(1);
  		case 0: 
  			for (int j = i; j<=uval-lval; j+=pval)
  			{
   	 			if (isprime(j+lval))
   	 			{
     			 	flagarr[j] = 1; 
    			} 	else {
     			 		flagarr[j] = 0; 
    				}
  			}
  			
  			unmapAndCheck(flagarr,size);
  			exit(EXIT_SUCCESS);
  	}
  }
  
  waitForAllSons(pval);
  
  countPrimes(&count,flagarr ,lval,uval);
     
  printf("Found %d primes%c\n", count, count ? ':' : '.');
  for (num = lval; num <= uval && nval !=0; num++)
    if (flagarr[num - lval])
    {
      	count--;
      	nval--;
      	printf("%d%c", num, (count && nval) ? ',' : '\n');  
    }
    
  unmapAndCheck(flagarr,size);
  return 0;
}

// NOTE : use 'man 3 getopt' to learn about getopt(), opterr, optarg and optopt 
void parseargs(char *argv[], int argc, int *lval, int *uval,int *nval, int* pval)
{
  int ch;

  opterr = 0;
  while ((ch = getopt (argc, argv, "l:u:n:p:")) != -1)
    switch (ch)
    {
      case 'l':  // Lower bound flag
        *lval = atoi(optarg);
        break;
      case 'u':  // Upper bound flag 
        *uval = atoi(optarg);
        break;
      case 'n':
      	*nval = atoi(optarg);
      	break;
      case 'p':
      	*pval = atoi(optarg);
      	break;
      case '?':
        if ((optopt == 'l') || (optopt == 'u') || (optopt == 'n') || (optopt == 'p'))
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        exit(1);
      default:
        exit(1);
    }    
}

int isprime(int n)
{
  static int *primes = NULL; 	// NOTE: static !
  static int size = 0;		// NOTE: static !
  static int maxprime;		// NOTE: static !
  int root;
  int i;

  // Init primes array (executed on first call)
  if (primes == NULL)
  {
    primes = (int *)malloc(2*sizeof(int));
    if (primes == NULL)
      exit(1);
    size = 2;
    primes[0] = 2;
    primes[1] = 3;
    maxprime = 3;
  }

  root = (int)(sqrt(n));

  // Update primes array, if needed
  while (root > maxprime)
    for (i = maxprime + 2 ;  ; i+=2)
      if (isprime(i))
      {
        size++;
        primes = (int *)realloc(primes, size * sizeof(int));
        if (primes == NULL)
          exit(1);
        primes[size-1] = i;
        maxprime = i;
        break;
      }

  // Check 'special' cases
  if (n <= 0)
    return -1;
  if (n == 1)
    return 0;

  // Check prime
  for (i = 0 ; ((i < size) && (root >= primes[i])) ; i++)
    if ((n % primes[i]) == 0)
      return 0;
  return 1;
}

void unmapAndCheck (char* flagarr, size_t size)
{
	if(munmap(flagarr, size) == -1)
		exit(1);
}

void waitForAllSons(int pval)
{
	  for(int i=0 ; i<pval ; i++)
  		if (wait(NULL) == -1)
            exit(1);
}

void countPrimes(int* count,char* flagarr ,int lval, int uval)
{
	  for(int i = lval ; i<=uval ;i++)
    	 if(flagarr[i-lval] == 1)
       		 *count += 1;
}
