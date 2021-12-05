
/*
        img_histogram.h
        Header file for image utilities
        Author: Bernard Heymann
        Created: 20000430       Modified: 20090909
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "rwimg.h"
#include "matrix.h"

// Function prototypes
int             img_histogram(Bimage* p, int bins, Bstring& psfile);
int*            img_calc_histogram(Bimage* p, int bins, float* new_scale, float* new_offset);
float*          img_percentiles(Bimage* p);
float*          img_fit_gauss_to_histogram(Bimage* p, int bins, int n);
float*          img_fit_gauss_to_histogram(Bimage* p, int bins, Bstring& psfile, int n);
float*          img_fit_poisson_to_histogram(Bimage* p, int bins);
float*          img_fit_poisson_to_histogram(Bimage* p, int bins, Bstring& psfile);

