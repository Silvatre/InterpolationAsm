#include <windows.h>
#include "Interpolation.h"
#include "dll_functions.h"
#include <string.h>
#include <math.h>
#include <algorithm>
#include <iostream>

/**
 *	Maksymalna iloœæ w¹tków.
 */
const int MAX_THREADS = 2;
	
namespace Project
{
	/**
	 * Struktura u¿ywana do przekazywania parametrów do metody generateFactorsThread w¹tku.
	 */
	struct Range {
		Interpolation* instance;
		int begin;
		int end;
		Range(Interpolation* toAdd, int a, int b): instance(toAdd), begin(a), end(b) {}
	};

	/**
	 * Struktura u¿ywana do przekazywania parametrów do metody generateReportThread w¹tku.
	 */
	struct ReportStruct {
		Interpolation* instance;
		int begin;
		int end;
		FILE* plk;
		int mn;
		ReportStruct(Interpolation* toAdd, int a, int b, FILE* nFile, int ammount): instance(toAdd), begin(a), end(b), plk(nFile), mn(ammount) {}
	};

	DWORD WINAPI Interpolation::generateFactorsThread(PVOID pvParam)
	{
		// Rzutowanie i wywolanie odpowiedniej funkcji liczacej
		Range* param = (Range*)pvParam;
		param->instance->generateFactorsValues(param->begin, param->end);
		delete param;		
		return 0;
	}

	DWORD WINAPI Interpolation::generateReportThread(PVOID pvParam)
	{
		// Rzutowanie i wywolanie odpowiedniej funkcji
		ReportStruct* param = (ReportStruct*)pvParam;
		param->instance->generateReportValues(param->begin, param->end, param->plk, param->mn);
		delete param;		
		return 0;
	}

	void Interpolation::generateFactorsValues(int begin, int end)
	{
		if(isUsingAsm)
		{
			FactorsGeneration(end, n, begin, x, f, aa, bb);
		}
		else
		{
			//iteruj po wêz³ach
			for (int j = begin; j <= end; j++)
			{
				double u1=0.0, u2=0.0;
				double beta = x[j];
				for (int k = n; k >= 1; k--)
				{
					double u = f[k] + 2.0*u1*cos(beta) - u2;
					u2 = u1;
					u1 = u;
				}

				aa[j] = 2.0/(n+1) * (f[0] + u1*cos(beta) - u2);
				bb[j] = 2.0/(n+1) * u1 * sin(beta);
			}
		}
		return;
	}

	void Interpolation::generateReportValues(int begin, int end, FILE* plk, int mn)
	{
		double delta;
		for (int i=begin; i<=end; i++)
		{
			delta=(y[i+1]-y[i])/mn;
			for(int j=0; j<mn; j++)
			{
				generateRow(y[i]+delta*j, i*mn+j);
			}
		}
		return;
	}

	double Interpolation::getPolynomialValue(double t) const
	{
		// Konwersja na y na x.
		t = this->getYtoX(t);
		if(isUsingAsm)
			return PolynomialValue(t, m, ddelta,aa, bb);		
		else
		{
			double pp, tt;
			pp = 0;
			tt = 0;
			for (int j = 1; j<=m; ++j)
			{
				pp += aa[j]*cos(j*t) + bb[j]*sin(j*t);
			}
			tt = 0.5 * aa[0] + (double)ddelta/2.0 * aa[m+1] * cos((m+1)*t);
			t = tt + pp;
			return t;	
		}
	}

	double Interpolation::getYtoX(double y) const
	{
		if(isUsingAsm)
			return ConvertYtoX(y, b, a, pi2);
		else
			return (y-a)/(b-a)*pi2;
	}

	void Interpolation::generateNodes()
	{
		if(isUsingAsm)
		{
			double delta = ValuesInitialization(&m, n, b, a, &ddelta, y);
			
			if(isUsingCSV)
			{	// Uzyskiwanie wartosci dla wezlow z pliku csv
				CSVRow row;
				for (int i=0; i<=n; i++)
				{
					CSVFile >> row;
					y[i] = atof(row[0].c_str());
					x[i]=getYtoX(y[i]);
					f[i] = atof(row[1].c_str());
				}
			}
			else // isUsingCSV == false
			{
				for (int i=0; i<=n; i++)
				{
					y[i]=a+delta*i;
					x[i]=getYtoX(y[i]);
					f[i]=m_Func->getValue(y[i]);
				}
			}
		}
		else	// isUsingAsm == fasle
		{
			double delta=(b-a)/(n+1);
			y[n+1]=b;
			// Wyznaczenie wspó³czynnika m. Dla nieparzystego n obciêcie w dó³ z uwagi na typ int - wartoœæ poprawna.
			m=n/2;		
			// Wyznaczenie wspó³czynnika delta
			ddelta=n-2*m;	
			for (int i=0; i<=n; i++)
			{
				y[i]=a+delta*i;
				x[i]=getYtoX(y[i]);
				f[i]=m_Func->getValue(y[i]);
			}
		}
	}

	void Interpolation::generateFactors()
	{
		if(isUsingAsm)
		{
			if(isUsingThreads)
			{
				// Okreslenie przedzialu dla watkow i wywolanie watkow pobocznych
				int interval = m/MAX_THREADS;
				HANDLE hThread[MAX_THREADS-1];
				// Minimalny przedzial dla tworzenia watku musi byc wiekszy od 10
				if(interval > 10)	
				{
					for(int i = 1; i < MAX_THREADS; ++i)
					{
						int begin = interval*(i-1);
						int end = interval*i-1;
						Range* newRange = new Range(this, begin, end);
						DWORD IdThread;
						hThread[i-1] = CreateThread(NULL, 0, generateFactorsThread, (PVOID)(newRange), 0, &IdThread);
					}
				}
				else
					interval = m;
				FactorsGeneration(m+1, n, (m-interval), x, f, aa, bb);
				WaitForMultipleObjects(MAX_THREADS-1, hThread, true, INFINITE);
			}
			else {
				FactorsGeneration(m+1, n, 0, x, f, aa, bb);
			}
			return;
		}
		else // isUsingAsm
		{
			if(isUsingThreads)
			{
				// Okreslenie przedzialu dla watkow i wywolanie watkow pobocznych
				int interval = m/MAX_THREADS;
				HANDLE hThread[MAX_THREADS-1];
				// Minimalny przedzial dla tworzenia watku musi byc wiekszy od 10
				if(interval > 10)	
				{
					for(int i = 1; i < MAX_THREADS; ++i)
					{
						int begin = interval*(i-1);
						int end = interval*i-1;
						Range* newRange = new Range(this, begin, end);
						DWORD IdThread;
						hThread[i-1] = CreateThread(NULL, 0, generateFactorsThread, (PVOID)(newRange), 0, &IdThread);
					}
				}
				else
					interval = m;
				//iteruj po wêz³ach
				for (int j = (m-interval); j <= m+1; j++)
				{
					double u1=0.0, u2=0.0;
					double beta = x[j];
					for (int k = n; k >= 1; k--)
					{
						double u = f[k] + 2.0*u1*cos(beta) - u2;
						u2 = u1;
						u1 = u;
					}

					aa[j] = 2.0/(n+1) * (f[0] + u1*cos(beta) - u2);
					bb[j] = 2.0/(n+1) * u1 * sin(beta);
				}
				// Oczekiwanie na zakonczenie watkow pobocznych, jesli jest taka potrzeba
				WaitForMultipleObjects(MAX_THREADS-1, hThread, true, INFINITE);
			}
			else { // isUsingThreads
				for (int j = 0; j <= m+1; j++)
				{
					double u1=0.0, u2=0.0;
					double beta = x[j];
					for (int k = n; k >= 1; k--)
					{
						double u = f[k] + 2.0*u1*cos(beta) - u2;
						u2 = u1;
						u1 = u;
					}
					aa[j] = 2.0/(n+1) * (f[0] + u1*cos(beta) - u2);
					bb[j] = 2.0/(n+1) * u1 * sin(beta);
				}
			}
			return;
		} // isUsingAsm
	}

	void Interpolation::generateReport1(char* fileName)
	{
		if(!isTesting)
		{
			FILE *plk;
			plk = fopen( fileName, "w" );
			// wydruk nag³ówka dla pliku .rex
			fprintf(plk, "yi\t xi\t g(yi)\n");
			// wydruk trójek wêze³  na osi y, wêze³ na osi x i wartoœæ funkcji w wêŸle interpolacji
			for (int i=0; i<=n; i++)
			{
				fprintf(plk, "%lf\t %lf\t %lf\n", y[i], x[i], f[i]);
			}
			// wydruk stopki dla pliku .rex
			fprintf(plk, "*InfKon*\nV 0 0 0 %d",n+1 );
			fclose(plk);
		}
	}	

	void Interpolation::generateReport2(const char* fileName, int mn)
	{
		double delta;
		int i, j;
		FILE *plk;
		if(!isTesting)
		{			
			plk = fopen( fileName, "w" );
			// wydruk nag³ówka dla pliku .rex
			fprintf(plk, "y\t tt(y)\t\n");
		}
		// Przydzielenie pamiêci
		m_PointsX = new double[(n+1)*mn+1];
		m_PointsY = new double[(n+1)*mn+1];

		if(isUsingThreads)
		{
			// Okreslenie przedzialu dla watkow i wywolanie watkow pobocznych
			int interval = n/MAX_THREADS;
			HANDLE hThread[MAX_THREADS-1];
			if(interval > 10)	// Minimalny przedzial dla tworzenia watku musi byc wiekszy od 10
			{
				for(int i = 1; i < MAX_THREADS; ++i)
				{
					int begin = interval*(i-1);
					int end = interval*i-1;
					ReportStruct* newStruct = new ReportStruct(this, begin, end, plk, mn);
					DWORD IdThread;
					hThread[i-1] = CreateThread(NULL, 0, generateReportThread, (PVOID)(newStruct), 0, &IdThread);
				}
			}
			else
				interval = n;

			for (i = (n-interval); i <= n; i++)
			{
				delta=(y[i+1]-y[i])/mn;
				for(j=0; j<mn; j++)
				{
					generateRow(y[i]+delta*j, i*mn+j);
				}
			}
			generateRow(b, (n+1)*mn);
			WaitForMultipleObjects(MAX_THREADS-1, hThread, true, INFINITE);
		}
		else
		{
			for (i = 0; i <= n; i++)
			{
				delta=(y[i+1]-y[i])/mn;
				for(j=0; j<mn; j++)
				{
					generateRow(y[i]+delta*j, i*mn+j);
				}
			}
			generateRow(b, (n+1)*mn);
		}
		if(!isTesting)
		{
			// Zapis do pliku
			for(i = 0; i < (n+1)*mn+1; i++)
			{
				fprintf(plk, "%lf\t %lf\t\n", m_PointsX[i], m_PointsY[i]);
			}
			// wydruk stopki dla pliku .rex
			fprintf(plk, "*InfKon*\nV 0 0 0 %d", (n+1)*mn+1);
			fclose(plk);
		}
	}

	void Interpolation::generateRow(double ty, int index)
	{
		m_PointsX[index] = ty;
		m_PointsY[index] = this->getPolynomialValue(ty);
	}

}