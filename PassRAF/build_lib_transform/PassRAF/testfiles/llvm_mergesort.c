/*
 * Copyright (c) 2000 Massachusetts Institute of Technology
 * Copyright (c) 2000 Matteo Frigo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * this program uses an algorithm that we call `sort'.
 * The algorithm is essentially mergesort:
 *
 *   sort(in[1..n]) =
 *       spawn sort(in[1..n/2], tmp[1..n/2])
 *       spawn sort(in[n/2..n], tmp[n/2..n])
 *       sync
 *       spawn merge(tmp[1..n/2], tmp[n/2..n], in[1..n])
 *
 *
 * The procedure merge does the following:
 *
 *       merge(A[1..n], B[1..m], C[1..(n+m)]) =
 *          find the median of A \union B using binary
 *          search.  The binary search gives a pair
 *          (ma, mb) such that ma + mb = (n + m)/2
 *          and all elements in A[1..ma] are smaller than
 *          B[mb..m], and all the B[1..mb] are smaller
 *          than all elements in A[ma..n].
 *
 *          spawn merge(A[1..ma], B[1..mb], C[1..(n+m)/2])
 *          spawn merge(A[ma..m], B[mb..n], C[(n+m)/2 .. (n+m)])
 *          sync
 *
 * The algorithm appears for the first time (AFAIK) in S. G. Akl and
 * N. Santoro, "Optimal Parallel Merging and Sorting Without Memory
 * Conflicts", IEEE Trans. Comp., Vol. C-36 No. 11, Nov. 1987 .  The
 * paper does not express the algorithm using recursion, but the
 * idea of finding the median is there.
 *
 * For sort of n elements, T_1 = O(n log n) and
 * T_\infty = O(log^3 n).  There is a way to shave a
 * log factor in the critical path (left as homework).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getoptions.c"
#include "timer.h"

typedef long ELM;

/* MERGESIZE must be >= 2 */
#define KILO 1024
#define MERGESIZE (2*KILO)
#define QUICKSIZE (2*KILO)
#define INSERTIONSIZE 20

static unsigned long rand_nxt = 0;

static inline unsigned long my_rand(void)
{
	rand_nxt = rand_nxt * 1103515245 + 12345;
	return rand_nxt;
}

static inline void my_srand(unsigned long seed)
{
	rand_nxt = seed;
}

static inline ELM med3(ELM a, ELM b, ELM c)
{
	if (a < b) {
		if (b < c) {
			return b;
		} else {
			if (a < c)
				return c;
			else
				return a;
		}
	} else {
		if (b > c) {
			return b;
		} else {
			if (a > c)
				return c;
			else
				return a;
		}
	}
}

/*
 * simple approach for now; a better median-finding
 * may be preferable
 */
static inline ELM choose_pivot(ELM *low, ELM *high)
{
	return med3(*low, *high, low[(high - low) / 2]);
}

static ELM *seqpart(ELM *low, ELM *high)
{
	ELM pivot;
	ELM h, l;
	ELM *curr_low = low;
	ELM *curr_high = high;

	pivot = choose_pivot(low, high);

	while (1) {
		while ((h = *curr_high) > pivot)
			curr_high--;

		while ((l = *curr_low) < pivot)
			curr_low++;

		if (curr_low >= curr_high)
			break;

		*curr_high-- = l;
		*curr_low++ = h;
	}

	/*
	 * I don't know if this is really necessary.
	 * The problem is that the pivot is not always the
	 * first element, and the partition may be trivial.
	 * However, if the partition is trivial, then
	 * *high is the largest element, whence the following
	 * code.
	 */
	if (curr_high < high)
		return curr_high;
	else
		return curr_high - 1;
}

#define swap(a, b) \
{ \
	ELM tmp;\
	tmp = a;\
	a = b;\
	b = tmp;\
}

static void insertion_sort(ELM *low, ELM *high)
{
	ELM *p, *q;
	ELM a, b;

	for (q = low + 1; q <= high; ++q) {
		a = q[0];
		for (p = q - 1; p >= low && (b = p[0]) > a; p--)
			p[1] = b;
		p[1] = a;
	}
}

/*
 * tail-recursive quicksort, almost unrecognizable :-)
 */
void seqquick(ELM *low, ELM *high)
{
	ELM *p;

	while (high - low >= INSERTIONSIZE) {
		p = seqpart(low, high);
		seqquick(low, p);
		low = p + 1;
	}

	insertion_sort(low, high);
}

void seqmerge(ELM *low1, ELM *high1, ELM *low2, ELM *high2,
		ELM *lowdest)
{
	ELM a1, a2;

	/*
	 * The following 'if' statement is not necessary
	 * for the correctness of the algorithm, and is
	 * in fact subsumed by the rest of the function.
	 * However, it is a few percent faster.  Here is why.
	 *
	 * The merging loop below has something like
	 *   if (a1 < a2) {
	 *        *dest++ = a1;
	 *        ++low1;
	 *        if (end of array) break;
	 *        a1 = *low1;
	 *   }
	 *
	 * Now, a1 is needed immediately in the next iteration
	 * and there is no way to mask the latency of the load.
	 * A better approach is to load a1 *before* the end-of-array
	 * check; the problem is that we may be speculatively
	 * loading an element out of range.  While this is
	 * probably not a problem in practice, yet I don't feel
	 * comfortable with an incorrect algorithm.  Therefore,
	 * I use the 'fast' loop on the array (except for the last
	 * element) and the 'slow' loop for the rest, saving both
	 * performance and correctness.
	 */

	if (low1 < high1 && low2 < high2) {
		a1 = *low1;
		a2 = *low2;
		for (;;) {
			if (a1 < a2) {
				*lowdest++ = a1;
				a1 = *++low1;
				if (low1 >= high1)
					break;
			} else {
				*lowdest++ = a2;
				a2 = *++low2;
				if (low2 >= high2)
					break;
			}
		}
	}
	if (low1 <= high1 && low2 <= high2) {
		a1 = *low1;
		a2 = *low2;
		for (;;) {
			if (a1 < a2) {
				*lowdest++ = a1;
				++low1;
				if (low1 > high1)
					break;
				a1 = *low1;
			} else {
				*lowdest++ = a2;
				++low2;
				if (low2 > high2)
					break;
				a2 = *low2;
			}
		}
	}
	if (low1 > high1) {
		memcpy(lowdest, low2, sizeof(ELM) * (high2 - low2 + 1));
	} else {
		memcpy(lowdest, low1, sizeof(ELM) * (high1 - low1 + 1));
	}
}

#define swap_indices(a, b) \
{ \
	ELM *tmp;\
	tmp = a;\
	a = b;\
	b = tmp;\
}

ELM *binsplit(ELM val, ELM *low, ELM *high)
{
	/*
	 * returns index which contains greatest element <= val.  If val is
	 * less than all elements, returns low-1
	 */
	ELM *mid;

	while (low != high) {
		mid = low + ((high - low + 1) >> 1);
		if (val <= *mid)
			high = mid - 1;
		else
			low = mid;
	}

	if (*low > val)
		return low - 1;
	else
		return low;
}

void merge(ELM *low1, ELM *high1, ELM *low2, ELM *high2, ELM *lowdest)
{
	/*
	 * merge: Merges range [low1, high1] with range [low2, high2]
	 * into the range [lowdest, ...]
	 */

	ELM *split1, *split2,*tmp1, *tmp2, *tmp3;	/*
																						 * where each of the ranges are broken for
																						 * recursive merge
																						 */
	long int lowsize;		/*
											 * total size of lower halves of two
											 * ranges - 2
											 */
	long tmp_size1, tmp_size2;

	/*
	 * We want to take the middle element (indexed by split1) from the
	 * larger of the two arrays.  The following code assumes that split1
	 * is taken from range [low1, high1].  So if [low1, high1] is
	 * actually the smaller range, we should swap it with [low2, high2]
	 */

	if (high2 - low2 > high1 - low1) {
		swap_indices(low1, low2);
		swap_indices(high1, high2);
	}
	if (high1 < low1) {
		/* smaller range is empty */
		memcpy(lowdest, low2, sizeof(ELM) * (high2 - low2));
		return;
	}
	if (high2 - low2 < MERGESIZE) {
		seqmerge(low1, high1, low2, high2, lowdest);
		return;
	}
	/*
	 * Basic approach: Find the middle element of one range (indexed by
	 * split1). Find where this element would fit in the other range
	 * (indexed by split 2). Then merge the two lower halves and the two
	 * upper halves.
	 */

	split1 = ((high1 - low1 + 1) / 2) + low1;
	split2 = binsplit(*split1, low2, high2);
	lowsize = split1 - low1 + split2 - low2;

	/*
	 * directly put the splitting element into
	 * the appropriate location
	 */
	*(lowdest + lowsize + 1) = *split1;
	tmp1 = split1 - 1;
	tmp_size1 = (tmp1 - low1) ;
	tmp_size2 = (split2 - low2) ;

	merge(low1, tmp1, low2, split2, lowdest);
	spawn();

	tmp1 = split1 + 1;
	tmp2 = split2 + 1;
	tmp3 = lowdest + lowsize + 2;
	tmp_size1 = (high1 - tmp1) ;
	tmp_size2 = (high2 - tmp2) ;

	merge(tmp1, high1, tmp2, high2, tmp3);
	spawn();

/*#pragma css sync*/
	return;
}

void wrapper_llvm_merge(struct _tpc_task_argument *t) 
{
  
  long *dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
   long slt ;
  memcpy(&slt,&t[1].addr_in,sizeof(slt));
   long slt1 ;
  memcpy(&slt1,&t[1].addr_in,sizeof(slt1));
   long slt2 ;
  memcpy(&slt2,&t[1].addr_in,sizeof(slt2));
    long slt3 ;
  memcpy(&slt3,&t[1].addr_in,sizeof(slt3));
  merge(dlt,slt,slt1,slt2,slt3);
}

void sort(ELM *low, ELM *tmp, long size)
{
	/*
	 * divide the input in four parts of the same size (A, B, C, D)
	 * Then:
	 *   1) recursively sort A, B, C, and D (in parallel)
	 *   2) merge A and B into tmp1, and C and D into tmp2 (in parallel)
	 *   3) merge tmp1 and tmp2 into the original array
	 */
	long quarter = size / 4, size1, size2;
	ELM *A, *B, *C, *D, *tmpA, *tmpB, *tmpC, *tmpD, *tmp1, *tmp2;

	if (size < QUICKSIZE) {
		/* quicksort when less than 1024 elements */
		seqquick(low, low + size - 1);
		return;
	}
	A = low;
	tmpA = tmp;
	B = A + quarter;
	tmpB = tmpA + quarter;
	C = B + quarter;
	tmpC = tmpB + quarter;
	D = C + quarter;
	tmpD = tmpC + quarter;


	sort(A, tmpA, quarter);
	spawn();

	sort(B, tmpB, quarter);
	spawn();

	sort(C, tmpC, quarter);
	spawn();
	long tmp_size = size - 3 * quarter;

	sort(D, tmpD, tmp_size);
	spawn();
/*#pragma css sync*/

	tmp1 = A + quarter - 1;
	tmp2 = B + quarter - 1;

	merge(A, tmp1, B, tmp2, tmpA);
spawn();
	tmp1 = C + quarter - 1;
	tmp2 = low + size - 1;

	merge(C, tmp1, D, tmp2, tmpC);
	spawn();
/*#pragma css sync*/

	tmp1 = tmpC - 1;
	tmp2 = tmpA + size - 1;
	size1 = 2*quarter;
	size2 = quarter+tmp_size;

	merge(tmpA, tmp1, tmpC, tmp2, A);
	spawn();
/*#pragma css sync*/
	;
}

void wrapper_llvm_sort(struct _tpc_task_argument *t) 
{
  
  long *dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
   long slt ;
  memcpy(&slt,&t[1].addr_in,sizeof(slt));
   long slt1 ;
  memcpy(&slt1,&t[1].addr_in,sizeof(slt1));
   long slt2 ;
  memcpy(&slt2,&t[1].addr_in,sizeof(slt2));
    long slt3 ;
  memcpy(&slt3,&t[1].addr_in,sizeof(slt3));
  merge(dlt,slt,slt1,slt2,slt3);
}

void scramble_array(ELM *arr, unsigned long size)
{
	unsigned long i;
	unsigned long j;

	for (i = 0; i < size; ++i) {
		j = my_rand();
		j = j % size;
		swap(arr[i], arr[j]);
	}
}

void fill_array(ELM *arr, unsigned long size)
{
	unsigned long i;

	my_srand(1);
	/* first, fill with integers 1..size */
	for (i = 0; i < size; ++i) {
		arr[i] = i;
	}

	/* then, scramble randomly */
	scramble_array(arr, size);
}

int usage(char **argv)
{
	fprintf(stderr, "Usage: %s [<-options>] [-n size] [-benchmark] [-h]	|	", argv[0]);
	fprintf(stderr, "sort is a parallel sorting algorithm, donned \"Multisort\", which	");
	fprintf(stderr, "is a variant of ordinary mergesort.  Multisort begins by dividing an ");
	fprintf(stderr, "array of elements in half and sorting each half.  It then merges the 	");
	fprintf(stderr, "two sorted halves back together, but in a divide-and-conquer approach 	");
	fprintf(stderr, "rather than the usual serial merge.");

	return -1;
}

char *specifiers[] =
{"-n", "-benchmark", "-h", "-thread", 0};
int opt_types[] =
{LONGARG, BENCHMARK, BOOLARG, THREADARG, 0};

int main(int argc, char **argv)
{
	long size;
	ELM *array, *tmp;
	long i;
	int success, benchmark, help, numThreads ;

	/* standard benchmark options */
	size = 3000000;

	get_options(argc, argv, specifiers, opt_types, &size, &benchmark, &help, &numThreads);

	if (help)
		return usage(argv);

	if (benchmark) {
		switch (benchmark) {
			case 1:		/* short benchmark options -- a little work */
				size = 10000;
				break;
			case 2:		/* standard benchmark options */
				size = 3000000;
				break;
			case 3:		/* long benchmark options -- a lot of work */
				size = 4100000;
				break;
		}
	}
tpc_init(nThreads);
	;

	array = (ELM *) malloc(size * sizeof(ELM));

	tmp = (ELM *) malloc(size * sizeof(ELM));
tpc_sync();
	;

	fill_array(array, size);
	spawn();
	printf("Initialization Done!");

	/* Timing. "Start" timers */
	START_TIME();

	sort(array, tmp, size);
	spawn();
	STOP_TIME();
	/* Timing. "Stop" timers */
	success = 1;
	for (i = 0; i < size; ++i)
		if (array[i] != i)
			success = 0;

	if (!success)
		printf("SORTING FAILURE");
	else {
		printf("Example: sort |	");
		printf("	      running on %d processor%s",numThreads , numThreads > 1 ? "s" : "");
		printf("options: number of elements = %ld", size);
	}

	free(array);

	free(tmp);
tpc_shutdown();
	PRINT_TIME();
	return 0;
}

