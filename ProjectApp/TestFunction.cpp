#include "TestFunction.h"

namespace Project {

	double TestFunction::getValue(double y) const
	{
		double A1 = 3;
		double B1 = b - 4*a - 0.75*(b-a);
		double A2 = -1;
		double B2 = b + 0.25*(b-a);
		double A4 = (0.5*(b-a))/(a-b+0.75*(b-a));
		double B4 = 0.25 * (b-a) - b * ( (0.5*(b-a)) / (a-b+0.75*(b-a)));

		if (y <= a+0.25*(b-a))
			return (A1*y + B1);
		else if (y <= a+0.5*(b-a))
			return (A2*y + B2);
		else if (y <= a+0.75*(b-a))
			return (0.75*(b-a));
		else
			return (A4*y + B4);
	}

}