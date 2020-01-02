/*	proj2.c
 *
 *	Projekt 2 - Iteracni vypocty
 *	Pracovni bod diody (seriove zapojeni U0 - R - D)
 *
 *	Tomas Milostny (xmilos02)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define I0 1e-12 //closing saturation diode current (10^-12 A)
#define UT 0.0258563 //thermal diode voltage (25,8563 mV)

/* desc: function that returns diode voltage Up
 * params: source volatage U0 [V], resistor resistance R [ohm], absolute accuracy EPS */
double diode(double u0, double r, double eps);

double diode(double u0, double r, double eps)
{
	//2nd subtask: binary search algorithm
	double interval_half = u0 * 0.5; //U0 = Up + Ur
	double up = interval_half;
	double eqt = 0, old_eqt; //equation that represents the end condition

	do {
		old_eqt = eqt;
		//1st subtask:  ip - ir = 0    ->    I0 * (e^(Up/UT) - 1) - (U0 - Up) / R = 0
		eqt = r * I0 * (exp(up / UT) - 1) - u0 + up;
		interval_half *= 0.5;

		//adding or subtracting half of the interval to the Up based on equation result
		if (eqt > 0)
			up -= interval_half;
		else
			up += interval_half;
	}
	//3rd subtask: apply equation from subtask no. 1 to algorithm schema from subtask no. 2
	while (fabs(eqt - old_eqt) > eps);

	return up;
}

int main(int argc, char **argv)
{
	//check program arguments first
	if (argc < 4)
	{
		fprintf(stderr, "error: too few arguments\ncorrect format: ./proj2 U0 R EPS\n");
		return 1;
	}

	//inputs: source voltage [V], resistor [ohm], epsilon
	double inputs[3];

	//converting program arguments to double and checking their validity
	for (int i = 0; i < 3; i++)
	{
		char *endptr;
		errno = 0;
		inputs[i] = strtod(argv[i + 1], &endptr);

		//man strtod: determine if an error occurred by checking whether errno has a nonzero value after the call
		//endptr is a pointer to the character after the last character used in the conversion to double
		//or error if input value is a negative number
		if (errno != 0 || *endptr != '\0' || inputs[i] < 0)
		{
			fprintf(stderr, "error: invalid arguments\n");
			return 1;
		}
	}

	//calculation of the diode voltage
	double up = diode(inputs[0], inputs[1], inputs[2]);

	//calculation of the diode current using calculated Up
	double ip = I0 * (exp(up / UT) - 1);

	printf("Up=%g V\n", up);
	printf("Ip=%g A\n", ip);

	return 0;
}