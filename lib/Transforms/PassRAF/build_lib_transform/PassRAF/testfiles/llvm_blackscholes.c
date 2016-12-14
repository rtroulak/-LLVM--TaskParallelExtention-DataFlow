
// Copyright (c) 2007 Intel Corp.

// Black-Scholes
// Analytical method for calculating European Options
//
//
// Reference Source: Options, Futures, and Other Derivatives, 3rd Edition, Prentice
// Hall, John C. Hull,

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "timer.h"



//Precision to use for calculations
#define fptype float


//Total number of options in optionData.txt
#define MAX_OPTIONS 1000

#define TEST

typedef struct OptionData_ {
	fptype s;                     // spot price
	fptype strike;                // strike price
	fptype r;                     // risk-free interest rate
	fptype divq;                  // dividend rate
	fptype v;                     // volatility
	fptype t;                     // time to maturity or option expiration in years
	//     (1yr = 1.0, 6mos = 0.5, 3mos = 0.25, ..., etc)
	char OptionType;              // Option type.  "P"=PUT, "C"=CALL
	fptype divs;                  // dividend vals (not used in this test)
	fptype DGrefval;              // DerivaGem Reference Value
} OptionData;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Cumulative Normal Distribution Function
// See Hull, Section 11.8, P.243-244
#define inv_sqrt_2xPI 0.39894228040143270286

fptype CNDF(fptype InputX)
{
	int sign;

	fptype OutputX;
	fptype xInput;
	fptype xNPrimeofX;
	fptype expValues;
	fptype xK2;
	fptype xK2_2, xK2_3;
	fptype xK2_4, xK2_5;
	fptype xLocal, xLocal_1;
	fptype xLocal_2, xLocal_3;

	// Check for negative value of InputX
	if (InputX < 0.0) {
		InputX = -InputX;
		sign = 1;
	} else
		sign = 0;

	xInput = InputX;

	// Compute NPrimeX term common to both four & six decimal accuracy calcs
	expValues = exp(-0.5f * InputX * InputX);
	xNPrimeofX = expValues;
	xNPrimeofX = xNPrimeofX * inv_sqrt_2xPI;

	xK2 = 0.2316419 * xInput;
	xK2 = 1.0 + xK2;
	xK2 = 1.0 / xK2;
	xK2_2 = xK2 * xK2;
	xK2_3 = xK2_2 * xK2;
	xK2_4 = xK2_3 * xK2;
	xK2_5 = xK2_4 * xK2;

	xLocal_1 = xK2 * 0.319381530;
	xLocal_2 = xK2_2 * (-0.356563782);
	xLocal_3 = xK2_3 * 1.781477937;
	xLocal_2 = xLocal_2 + xLocal_3;
	xLocal_3 = xK2_4 * (-1.821255978);
	xLocal_2 = xLocal_2 + xLocal_3;
	xLocal_3 = xK2_5 * 1.330274429;
	xLocal_2 = xLocal_2 + xLocal_3;

	xLocal_1 = xLocal_2 + xLocal_1;
	xLocal = xLocal_1 * xNPrimeofX;
	xLocal = 1.0 - xLocal;

	OutputX = xLocal;

	if (sign) {
		OutputX = 1.0 - OutputX;
	}

	return OutputX;
}

// For debugging
void print_xmm(fptype in, char *s)
{
	printf("%s: %f", s, in);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
fptype BlkSchlsEqEuroNoDiv(fptype sptprice,
		fptype strike, fptype rate, fptype volatility, fptype time, int otype, float timet)
{
	fptype OptionPrice;

	// local private working variables for the calculation
	fptype xStockPrice;
	fptype xStrikePrice;
	fptype xRiskFreeRate;
	fptype xVolatility;
	fptype xTime;
	fptype xSqrtTime;

	fptype logValues;
	fptype xLogTerm;
	fptype xD1;
	fptype xD2;
	fptype xPowerTerm;
	fptype xDen;
	fptype d1;
	fptype d2;
	fptype FutureValueX;
	fptype NofXd1;
	fptype NofXd2;
	fptype NegNofXd1;
	fptype NegNofXd2;

	xStockPrice = sptprice;
	xStrikePrice = strike;
	xRiskFreeRate = rate;
	xVolatility = volatility;

	xTime = time;
	xSqrtTime = sqrt(xTime);

	logValues = log(sptprice / strike);

	xLogTerm = logValues;


	xPowerTerm = xVolatility * xVolatility;
	xPowerTerm = xPowerTerm * 0.5;

	xD1 = xRiskFreeRate + xPowerTerm;
	xD1 = xD1 * xTime;
	xD1 = xD1 + xLogTerm;

	xDen = xVolatility * xSqrtTime;
	xD1 = xD1 / xDen;
	xD2 = xD1 - xDen;

	d1 = xD1;
	d2 = xD2;

	NofXd1 = CNDF(d1);
	NofXd2 = CNDF(d2);

	FutureValueX = strike * (exp(-(rate) * (time)));
	if (otype == 0) {
		OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);
	} else {
		NegNofXd1 = (1.0 - NofXd1);
		NegNofXd2 = (1.0 - NofXd2);
		OptionPrice = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
	}

	return OptionPrice;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

int bs_thread(fptype * array, fptype * sptprice, fptype * strike, fptype * rate,
		fptype * volatility, fptype * otime, float *otype, int bdim)
{
	int i;

	for (i = 0; i < bdim; i++) {
		/* Calling main function to calculate option value based on
		 * Black & Sholes's equation.
		 */
		array[i] = BlkSchlsEqEuroNoDiv(sptprice[i], strike[i], rate[i], volatility[i], otime[i], otype[i], 0);
	}

	return 0;
}



int bs_split(fptype * prices, fptype * sptprice, fptype * strike, fptype * rate,
		fptype * volatility, fptype * otime, float *otype, int bdim, int limit)

{
	int i;
	for (i = 0; i < limit; i += bdim) {
		//       length = i+BS>numOptions?numOptions-i:BS;
		fptype * pricesa = &prices[i];
		fptype * sptpricea = &sptprice[i];
		fptype * strikea = &strike[i];
		fptype * ratea = &rate[i];
		fptype * volatilitya = &volatility[i];
		fptype * otimea = &otime[i];
		int * otypea = &otype[i];
// #pragma css task out(pricesa[bdim]) \
// 								 in(sptpricea[bdim],strikea[bdim],\
// 										ratea[bdim],volatilitya[bdim],\
// 										otimea[bdim],otypea[bdim],bdim)\
// 								 safe(pricesa, sptpricea, strikea,	ratea, \
// 										 	volatilitya,otimea, otypea,BS,limit_s)
		bs_thread(pricesa, sptpricea, strikea, ratea, volatilitya, otimea, otypea, bdim);
		spawn();
	}
/*#pragma css sync*/
	;
}

void wrapper_llvm_bs_split(struct _tpc_task_argument *t) 
{
  float * dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
  float * dlt1 ;
  memcpy(&dlt1,&t[1].addr_in,sizeof(dlt1));
  float * dlt2 ;
  memcpy(&dlt2,&t[2].addr_in,sizeof(dlt2));
  float * dlt3 ;
  memcpy(&dlt3,&t[3].addr_in,sizeof(dlt3));
  float * dlt4 ;
  memcpy(&dlt4,&t[4].addr_in,sizeof(dlt4));
  float * dlt5 ;
  memcpy(&dlt5,&t[5].addr_in,sizeof(dlt5));
  float *  dlt6 ;
  memcpy(&dlt6,&t[6].addr_in,sizeof(dlt6));
  int  dlt7 ;
  memcpy(&dlt7,&t[7].addr_in,sizeof(dlt7));
    int  dlt8 ;
  memcpy(&dlt8,&t[8].addr_in,sizeof(dlt8));
  bs_split(dlt,dlt1,dlt2,dlt3,dlt4,dlt5,dlt6,dlt7,dlt8);
  
}

void wrapper_llvm_bs_thread(struct _tpc_task_argument *t) 
{
  float * dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
  float * dlt1 ;
  memcpy(&dlt1,&t[1].addr_in,sizeof(dlt1));
  float * dlt2 ;
  memcpy(&dlt2,&t[2].addr_in,sizeof(dlt2));
  float * dlt3 ;
  memcpy(&dlt3,&t[3].addr_in,sizeof(dlt3));
  float * dlt4 ;
  memcpy(&dlt4,&t[4].addr_in,sizeof(dlt4));
  float * dlt5 ;
  memcpy(&dlt5,&t[5].addr_in,sizeof(dlt5));
  float * dlt6 ;
  memcpy(&dlt6,&t[6].addr_in,sizeof(dlt6));
  int  dlt7 ;
  memcpy(&dlt7,&t[7].addr_in,sizeof(dlt7));
  bs_thread(dlt,dlt1,dlt2,dlt3,dlt4,dlt5,dlt6,dlt7);
  
}


static inline unsigned long ceil_to_pow2(unsigned long x)
{
	unsigned long leeding_zeros = __builtin_clzl(x);
	unsigned long next_pow2 = 1L << (64 - leeding_zeros);
	//    printf("clzl: %lu", leeding_zeros );
	//    printf("x-> %lx | R -> %lx", x, next_pow2 );
	return next_pow2;
}

void data_initializer(OptionData * in, OptionData * out)
{
	unsigned int i;
	for (i = 0; i < MAX_OPTIONS; i++, out++, in++) {
		memcpy(out, in, sizeof(OptionData));
		//     out->s=in->s;
		//     out->strike=in->strike;
		//     out->r=in->r;
		//     out->divq=in->divq;
		//     out->v=in->v;
		//     out->t=in->t;
		//     out->s=in->s;
	}
}


void wrapper_llvm_data_initializer(struct _tpc_task_argument *t) 
{
  
  
}

OptionData data_init[MAX_OPTIONS] = {
#include "optionData.txt"
};


int main(int argc, char **argv)
{
	int i;
	int loopnum;

	// ex-globals
	OptionData *data;
	fptype *prices;
	unsigned int numOptions;
	unsigned int maxOptions = MAX_OPTIONS;

	int *otype;
	fptype *sptprice;
	fptype *strike;
	fptype *rate;
	fptype *volatility;
	fptype *otime;
	//   int numError = 0;
	int nThreads;

	// if (argc != 4) {
	// 	printf("Usage:%s <bdim> <kiloOptions> <nthreads>", argv[0]);
	// 	exit(1);
	// }
	int BS = atoi(argv[1]);
	numOptions = atoi(argv[2]) * MAX_OPTIONS;
	nThreads = atoi(argv[3]);

	//   if (nThreads > numOptions) {
	//     printf("WARNING: Not enough work, reducing number of threads to match number of options.");
	//     nThreads = numOptions;
	//   }

	assert(numOptions > BS);
	numOptions = ((numOptions/BS)/nThreads) * nThreads * BS;
	//   if (numOptions % BS) {
	//     printf("ERROR: Number of options in inputFile must be multiple of BS");
	//     exit(1);
	//   }
	unsigned int grid_sz = 2 * (numOptions * sizeof(OptionData) +
			6 * numOptions * sizeof(fptype) + numOptions * sizeof(int));

tpc_init(nThreads);

	// alloc spaces for the option data
	data = (OptionData *) malloc(numOptions * sizeof(OptionData));

	prices = (fptype *) malloc(numOptions * sizeof(fptype));
	OptionData *data_a = data;
	unsigned int limit = numOptions / MAX_OPTIONS;
	for (loopnum = 0; loopnum < limit; loopnum++, data_a += MAX_OPTIONS) {
// #pragma css task in(data_init[maxOptions]) out(data_a) safe(data_init, data_a)
		data_initializer(data_init, data_a);
		// spawn();
	}
tpc_sync();
	if (numOptions % BS) {
		numOptions = (numOptions / BS) * BS;
		printf("%u", numOptions);
		//     printf("ERROR: Number of options in inputFile must be multiple of BS");
		//     exit(1);
	}

	printf("Num of Options: %u", numOptions);
	//     printf("Num of Runs: %d", NUM_RUNS);
	//#pragma css malloc
	//     buffer = (fptype *) malloc(numOptions * sizeof(fptype));

	sptprice = (fptype *) malloc(numOptions * sizeof(fptype));

	strike = (fptype *) malloc(numOptions * sizeof(fptype));

	rate = (fptype *) malloc(numOptions * sizeof(fptype));

	volatility = (fptype *) malloc(numOptions * sizeof(fptype));

	otime = (fptype *) malloc(numOptions * sizeof(fptype));

	//     buffer2 = (int *) malloc(numOptions * sizeof(fptype) + PAD);

	otype = (int *) malloc(numOptions * sizeof(int));

	for (i = 0; i < numOptions; i++) {
		otype[i] = (data[i].OptionType == 'P') ? 1 : 0;
		sptprice[i] = data[i].s;
		strike[i] = data[i].strike;
		rate[i] = data[i].r;
		volatility[i] = data[i].v;
		otime[i] = data[i].t;
	}

	fptype *pricesa;
	fptype *sptpricea;
	fptype *strikea;
	fptype *ratea;
	fptype *volatilitya;
	fptype *otimea;
	int *otypea;

	printf("Size of data: %lu MB", (5 * (numOptions * sizeof(fptype)) + numOptions * sizeof(int)) / (1024U * 1024U));

	int limit_s = numOptions/(2*nThreads);
	START_TIME();
	for (i = 0; i < numOptions; i += limit_s) {
		//       length = i+BS>numOptions?numOptions-i:BS;
		pricesa = &prices[i];
		sptpricea = &sptprice[i];
		strikea = &strike[i];
		ratea = &rate[i];
		volatilitya = &volatility[i];
		otimea = &otime[i];
		otypea = &otype[i];
#pragma css task out(pricesa[BS]) \
								 in(sptpricea[BS], strikea[BS],\
										ratea[BS], volatilitya[BS],\
										otimea[BS], otypea[BS],BS,limit_s)\
								 safe(pricesa, sptpricea, strikea,	ratea, \
										 	volatilitya,otimea, otypea,BS,limit_s)
		bs_split(pricesa, sptpricea, strikea, ratea, volatilitya, otimea, otypea, BS, limit_s);
		spawn();
	}
	STOP_TIME();

#ifdef TEST
	fptype priceDelta;
	int numError=0;
	for (i = 0; i < numOptions; i++) {
		priceDelta = data[i].DGrefval - prices[i];
		if (fabs(priceDelta) >= 1e-4) {
			printf("Error on %d. Computed=%.5f, Ref=%.5f, Delta=%.5f", i, prices[i], data[i].DGrefval, priceDelta);
			numError++;
		}
	}
	printf("Num Errors: %d", numError);
#endif                        // TEST

#ifdef TEST2
	const char *outputFile = "blsch.out";
	//Write prices to output file
	File *file = fopen(outputFile, "w");
	if (file == NULL) {
		printf("ERROR: Unable to open file `%s'.", outputFile);
		exit(1);
	}
	rv = fprintf(file, "%i", numOptions);
	if (rv < 0) {
		printf("ERROR: Unable to write to file `%s'.", outputFile);
		fclose(file);
		exit(1);
	}
	for (i = 0; i < numOptions; i++) {
		rv = fprintf(file, "%.18f", prices[i]);
		if (rv < 0) {
			printf("ERROR: Unable to write to file `%s'.", outputFile);
			fclose(file);
			exit(1);
		}
	}
	rv = fclose(file);
	if (rv != 0) {
		printf("ERROR: Unable to close file `%s'.", outputFile);
		exit(1);
	}
#endif                          // TEST2

tpc_shutdown();
	;
	PRINT_TIME();


	return 0;
}

