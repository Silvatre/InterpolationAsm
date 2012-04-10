#include "PerformanceTest.h"
#include "TestFunction.h"
#include "Interpolation.h"
#include <Windows.h>

namespace Project {

	void PerformanceTest::start()
	{
		FILE *plk;
		plk = fopen( m_Path.c_str(), "w" );
		fprintf(plk, "factors\t asm1\t asm2\t cpp1\t cpp2\t\n");

		LARGE_INTEGER* begin = new LARGE_INTEGER();
		LARGE_INTEGER* end = new LARGE_INTEGER();
		TestFunction* function =  new TestFunction(1, 3);
		long int asm1Val, asm2Val, cpp1Val, cpp2Val;

		for(int i = m_BeginValue; i <= m_EndValue; )
		{
			QueryPerformanceCounter(begin);
			Interpolation asm1(function, i, true, false, true);
			asm1.generateReport1("file1.rex");
			asm1.generateReport2("file2.rex", m_Points);
			QueryPerformanceCounter(end);
			asm1Val = end->QuadPart - begin->QuadPart;

			QueryPerformanceCounter(begin);
			Interpolation asm2(function, i, true, true, true);			
			asm2.generateReport1("file1.rex");
			asm2.generateReport2("file2.rex", m_Points);
			QueryPerformanceCounter(end);
			asm2Val = end->QuadPart - begin->QuadPart;

			QueryPerformanceCounter(begin);
			Interpolation cpp1(function, i, false, false, true);			
			cpp1.generateReport1("file1.rex");
			cpp1.generateReport2("file2.rex", m_Points);
			QueryPerformanceCounter(end);
			cpp1Val = end->QuadPart - begin->QuadPart;

			QueryPerformanceCounter(begin);
			Interpolation cpp2(function, i, false, true, true);
			cpp2.generateReport1("file1.rex");
			cpp2.generateReport2("file2.rex", m_Points);
			QueryPerformanceCounter(end);
			cpp2Val = end->QuadPart - begin->QuadPart;

			fprintf(plk, "%d\t %li\t %li\t %li\t %li\t\n", i, asm1Val, asm2Val, cpp1Val, cpp2Val);
			i += m_Move;
		}
		delete function;
		fclose(plk);
	}

}
