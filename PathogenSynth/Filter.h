#pragma once

#include <stdint.h>

class Filter
{

public:
	double* kernel;

	double convolute(double X, double* H);




protected:



};