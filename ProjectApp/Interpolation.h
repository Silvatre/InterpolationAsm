#pragma once
#include "TestFunction.h"
#include <math.h>
#include <iostream>
#include <String>
#include <Windows.h>
#include "CSVRow.h"
#include "IFunction.h"


const double pi2=8*atan(1.0);

namespace Project {

	/**
	 *	Klasa modelu interpolacji trygonometrycznej metod¹ Goertzela. Funkcja interpolowana
	 *	uzyskiwana jest z pliku CSV lub wbudowanej funkcji implementuj¹cej klasê abstrakcyjn¹ (interfejs)
	 *	IFunction. Funkcja interpolowana o okresie 2pi zawieraj¹ce równoodleg³e wêz³y interpolacyjne.
	 *
	 *	@author Michal Krupinski
	 *	@version 1.0
	 */

	class Interpolation
	{
	private:		
		/**
		 * Maksymalny indeks wêz³a interpolacji, (n+1) - lb. wêz³ów.
		 */
		int n;
		/**
		 * Wspó³czynnik m.
		 */
		int m;	
		/**
		 * Wspó³czynnik delta.
		 */
		int ddelta;	
		/**
		 * Dolna granica przedzia³u zawieraj¹cego wêz³y interpolacji.
		 */
		double a;
		/**
		 * Górna granica przedzia³u zawieraj¹cego wêz³y interpolacji.
		 */
		double b;
		/**
		 * Tablica wêz³ów na osi x
		 */
		double* x;	
		/**
		 * Tablica wêz³ów na osi y.
		 */
		double* y;
		/**
		 * Tablica wartoœci funkcji w wêz³ach interpolacji.
		 */
		double* f;		
		/**
		 * Tablica wspó³czynników ai.
		 */
		double *aa; 
		/**
		 * Tablica wspó³czynników bi.
		 */
		double *bb;			
		/**
		 * Tablica punktów poœrednich generowanych do raportu.
		 */
		double* m_PointsX;
		/**
		 * Tablica wartoœci punktów poœrednich generowanych do raportu.
		 */
		double* m_PointsY;
		/**
		 * Zmienna reprezentuj¹ca funkcjê interpolowan¹ - wbudowan¹, nie pochodz¹ca z pliku CSV. Implementuje interfejs IFunction.
		 */
		IFunction* m_Func;	

	protected:
		/**
		 * Strumieñ do obs³ugi pliku CSV.
		 */
		std::ifstream CSVFile;		
		/**
		 * Metoda wyznaczaj¹ca wêz³y interpolacji wraz z ich wartoœciami funkcji.
		 */
		void generateNodes();										
		/**
		 * Metoda generuj¹ca parametry Ai oraz Bi.
		 */
		void generateFactors();										
		/**
		 * Metoda dokonuj¹ca konwersji zmiennej y na x.
		 * @param y Liczba do konwersji
		 */
		double getYtoX(double y) const;								
		/**
		 * Metoda generuj¹ca pojedyñczy wiersz w raporcie.
		 * @param ty Punkt do wygenerowania danych
		 * @param num Indeks w tablicy
		 */
		void generateRow(double ty, int index);						
		/**
		 * Metoda statycza obs³ugi w¹tku generowanego przez metodê generateFactors.
		 */
		static DWORD WINAPI generateFactorsThread(PVOID pvParam);	
		/**
		 * Metoda statycza obs³ugi w¹tku generowanego przez metodê generateReport2.
		 */
		static DWORD WINAPI generateReportThread(PVOID pvParam);

	public:
		/**
		 * Konstruktor obiektu klasy modelu interpolacji trygonometrycznej dla funkcji interpolowanej wbudowanej
		 * implementuj¹cej klasê abstrakcyjn¹ (interfejs) IFunction.
		 *
		 * @param IntFunc wskaŸnik na funkcjê interpolowan¹ implementuj¹c¹ klasê abstrakcyjn¹ (interfejs) IFunction.
		 * @param n Liczba wêz³ów interpolacji.
		 */
		Interpolation(IFunction* IntFunc, int n): m_Func(IntFunc), isTesting(false), isUsingCSV(false), isUsingAsm(true), isUsingThreads(true), n(n), a(IntFunc->getLeftRange()), b(IntFunc->getRightRange()) 
		{
			x = new double[n+3];
			y = new double[n+3];
			f = new double[n+3];
			aa = new double[n+3];
			bb = new double[n+3];
			init();
		}

		/**
		 * Konstruktor obiektu klasy modelu interpolacji trygonometrycznej dla funkcji interpolowanej wbudowanej
		 * implementuj¹cej klasê abstrakcyjn¹ (interfejs) IFunction.
		 *
		 * @param IntFunc wskaŸnik na funkcjê interpolowan¹ implementuj¹c¹ klasê abstrakcyjn¹ (interfejs) IFunction.
		 * @param n Liczba wêz³ów interpolacji.
		 * @param usesAsm Czy uzywany ma byc kod asemblera. False - uzywany bedzie w calosci kod C.
		 * @param usesThreads Czy uzywane maja byc watki.
		 * @param isTest Czy jest to test wydajnosciowy (wylacza operacje zapisu do plikow).
		 */
		Interpolation(IFunction* IntFunc, int n, bool usesAsm, bool usesThreads, bool isTest): m_Func(IntFunc), isTesting(isTest), isUsingCSV(false), isUsingAsm(usesAsm), isUsingThreads(usesThreads), n(n), a(IntFunc->getLeftRange()), b(IntFunc->getRightRange()) 
		{
			x = new double[n+3];
			y = new double[n+3];
			f = new double[n+3];
			aa = new double[n+3];
			bb = new double[n+3];
			init();
		}

		/**
		 * Konstruktor obiektu klasy modelu interpolacji trygonometrycznej dla funkcji interpolowanej
		 * zawieraj¹cej ztablicowane wartoœci funkcji w wêz³ach interpolacji w zewnêtrznym pliku CSV.
		 *
		 * @param filePath Œcie¿ka zewnêtrznego pliku CSV zawieraj¹cego ztablicowane wartoœci funkcji w wêz³ach interpolacji, liczbê wêz³ów interpolacji oraz ich przedzia³
		 */
		Interpolation(std::string filePath): isUsingAsm(true), isTesting(false), isUsingThreads(true), n(n), isUsingCSV(true), m_Func(new TestFunction(1,3))
		{
			// mozna dodac try catcha jak nie ma pliku
			CSVFile = std::ifstream(filePath.c_str());

			CSVRow row;
			CSVFile >> row;
			a = atof(row[0].c_str());
			b = atof(row[1].c_str());
			n = atoi(row[2].c_str()) - 1;	// n to maksymalny indeks wêz³a interpolacji. Dekrementowany z uwagi na liczenie od 0.

			x = new double[n+3];
			y = new double[n+3];
			f = new double[n+3];
			aa = new double[n+3];
			bb = new double[n+3];
			init();
		}

		/**
		 * Konstruktor obiektu klasy modelu interpolacji trygonometrycznej dla funkcji interpolowanej
		 * zawieraj¹cej ztablicowane wartoœci funkcji w wêz³ach interpolacji w zewnêtrznym pliku CSV.
		 *
		 * @param filePath Œcie¿ka zewnêtrznego pliku CSV zawieraj¹cego ztablicowane wartoœci funkcji w wêz³ach interpolacji, liczbê wêz³ów interpolacji oraz ich przedzia³
		 * @param usesAsm Czy uzywany ma byc kod asemblera. False - uzywany bedzie w calosci kod C.
		 * @param usesThreads Czy uzywane maja byc watki.
		 * @param isTest Czy jest to test wydajnosciowy (wylacza operacje zapisu do plikow).
		 */
		Interpolation(std::string filePath, bool usesAsm, bool usesThreads, bool isTest): isUsingAsm(usesAsm), isTesting(isTest), isUsingThreads(usesThreads), n(n), isUsingCSV(true), m_Func(new TestFunction(1,3))
		{
			// mozna dodac try catcha jak nie ma pliku
			CSVFile = std::ifstream(filePath.c_str());

			CSVRow row;
			CSVFile >> row;
			a = atof(row[0].c_str());
			b = atof(row[1].c_str());
			n = atoi(row[2].c_str()) - 1;	// n to maksymalny indeks wêz³a interpolacji. Dekrementowany z uwagi na liczenie od 0.

			x = new double[n+3];
			y = new double[n+3];
			f = new double[n+3];
			aa = new double[n+3];
			bb = new double[n+3];
			init();
		}

		/**
		 * Destruktor zwalniaj¹cy zaalokowan¹ pamiêæ oraz zamykaj¹cy strumieñ odczytu pliku CSV.
		 */
		~Interpolation() 
		{
			delete [] x;
			delete [] y;
			delete [] f;
			delete [] aa;
			delete [] bb;
			CSVFile.close();
		}

		/**
		 * Metoda inicjalizuj¹ca wykonanie algorytmu. U¿ywana w konstruktorach oraz dostêpna publicznie w celu
		 * umo¿liwienia wykonania algorytmu z wprowadzonymi modyfikacjami w sposobie jego dzia³ania (wielow¹tkowoœæ, u¿ycie asemblera).
		 */
		void init() {
			generateNodes();
			generateFactors();	
		}	

		/**
		 * Metoda obliczaj¹ca wartoœæ funkcji interpoluj¹cej w danym punkcie.
		 *
		 * @param t Punkt, którego wartoœæ funkcji chcemy uzyskaæ.
		 */
		double getPolynomialValue(double t) const;

		/**
		 * Metoda generuj¹ca raport dotycz¹cy funkcji interpolowanej - wêz³ów interpolacji wraz z ich wartoœciami
		 * w funkcji interpolowanej.
		 *
		 * @param fileName Œcie¿ka zawieraj¹ca nazwê pliku do maj¹cego postaæ raportu.
		 */
		void generateReport1(char* fileName);

		/**
		 * Metoda generuj¹ca raport dotycz¹cy funkcji interpoluj¹cej - wygenerowane punkty wraz z ich wartoœciami
		 * w funkcji interpoluj¹cej.
		 *
		 * @param fileName Œcie¿ka zawieraj¹ca nazwê pliku do maj¹cego postaæ raportu.
		 * @param mn Liczba punktów poœrednich generowanych pomiêdzy wêz³ami interpolacji.
		 */
		void generateReport2(const char* fileName, int mn);	

		/**
		 * Metoda wykorzystywana w w¹tku wywo³ywanego z adresem do metody generateFactorsThread.
		 *
		 * @param begin Zakres obliczeñ - pocz¹tek.
		 * @param end Zakres obliczeñ - koniec.
		 */
		void generateFactorsValues(int begin, int end);						
		
		/**
		 * Metoda wykorzystywana w w¹tku wywo³ywanego z adresem do metody generateReportThread.
		 *
		 * @param begin Zakres obliczeñ - pocz¹tek.
		 * @param end Zakres obliczeñ - koniec.
		 */
		void generateReportValues(int begin, int end, FILE* plk, int mn);
		
		/**
		 * Zmienna okreœlaj¹ca czy uzywane sa dane z pliku csv.
		 */
		bool isUsingCSV;			
		/**
		 * Zmienna okreœlaj¹ca czy u¿ywany jest kod asemblera.
		 */
		bool isUsingAsm;			
		/**
		 * Zmienna okreœlaj¹ca czy u¿ywane s¹ w¹tki.
		 */
		bool isUsingThreads;
		/**
		 * Zmienna okreœlaj¹ca czy wykonywane s¹ testy wydajnoœciowe. Wy³¹cza wiêkszoœæ zapisów do plików.
		 */
		bool isTesting;

	};

}