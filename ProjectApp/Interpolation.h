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
	 *	Klasa modelu interpolacji trygonometrycznej metod� Goertzela. Funkcja interpolowana
	 *	uzyskiwana jest z pliku CSV lub wbudowanej funkcji implementuj�cej klas� abstrakcyjn� (interfejs)
	 *	IFunction. Funkcja interpolowana o okresie 2pi zawieraj�ce r�wnoodleg�e w�z�y interpolacyjne.
	 *
	 *	@author Michal Krupinski
	 *	@version 1.0
	 */

	class Interpolation
	{
	private:		
		/**
		 * Maksymalny indeks w�z�a interpolacji, (n+1) - lb. w�z��w.
		 */
		int n;
		/**
		 * Wsp�czynnik m.
		 */
		int m;	
		/**
		 * Wsp�czynnik delta.
		 */
		int ddelta;	
		/**
		 * Dolna granica przedzia�u zawieraj�cego w�z�y interpolacji.
		 */
		double a;
		/**
		 * G�rna granica przedzia�u zawieraj�cego w�z�y interpolacji.
		 */
		double b;
		/**
		 * Tablica w�z��w na osi x
		 */
		double* x;	
		/**
		 * Tablica w�z��w na osi y.
		 */
		double* y;
		/**
		 * Tablica warto�ci funkcji w w�z�ach interpolacji.
		 */
		double* f;		
		/**
		 * Tablica wsp�czynnik�w ai.
		 */
		double *aa; 
		/**
		 * Tablica wsp�czynnik�w bi.
		 */
		double *bb;			
		/**
		 * Tablica punkt�w po�rednich generowanych do raportu.
		 */
		double* m_PointsX;
		/**
		 * Tablica warto�ci punkt�w po�rednich generowanych do raportu.
		 */
		double* m_PointsY;
		/**
		 * Zmienna reprezentuj�ca funkcj� interpolowan� - wbudowan�, nie pochodz�ca z pliku CSV. Implementuje interfejs IFunction.
		 */
		IFunction* m_Func;	

	protected:
		/**
		 * Strumie� do obs�ugi pliku CSV.
		 */
		std::ifstream CSVFile;		
		/**
		 * Metoda wyznaczaj�ca w�z�y interpolacji wraz z ich warto�ciami funkcji.
		 */
		void generateNodes();										
		/**
		 * Metoda generuj�ca parametry Ai oraz Bi.
		 */
		void generateFactors();										
		/**
		 * Metoda dokonuj�ca konwersji zmiennej y na x.
		 * @param y Liczba do konwersji
		 */
		double getYtoX(double y) const;								
		/**
		 * Metoda generuj�ca pojedy�czy wiersz w raporcie.
		 * @param ty Punkt do wygenerowania danych
		 * @param num Indeks w tablicy
		 */
		void generateRow(double ty, int index);						
		/**
		 * Metoda statycza obs�ugi w�tku generowanego przez metod� generateFactors.
		 */
		static DWORD WINAPI generateFactorsThread(PVOID pvParam);	
		/**
		 * Metoda statycza obs�ugi w�tku generowanego przez metod� generateReport2.
		 */
		static DWORD WINAPI generateReportThread(PVOID pvParam);

	public:
		/**
		 * Konstruktor obiektu klasy modelu interpolacji trygonometrycznej dla funkcji interpolowanej wbudowanej
		 * implementuj�cej klas� abstrakcyjn� (interfejs) IFunction.
		 *
		 * @param IntFunc wska�nik na funkcj� interpolowan� implementuj�c� klas� abstrakcyjn� (interfejs) IFunction.
		 * @param n Liczba w�z��w interpolacji.
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
		 * implementuj�cej klas� abstrakcyjn� (interfejs) IFunction.
		 *
		 * @param IntFunc wska�nik na funkcj� interpolowan� implementuj�c� klas� abstrakcyjn� (interfejs) IFunction.
		 * @param n Liczba w�z��w interpolacji.
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
		 * zawieraj�cej ztablicowane warto�ci funkcji w w�z�ach interpolacji w zewn�trznym pliku CSV.
		 *
		 * @param filePath �cie�ka zewn�trznego pliku CSV zawieraj�cego ztablicowane warto�ci funkcji w w�z�ach interpolacji, liczb� w�z��w interpolacji oraz ich przedzia�
		 */
		Interpolation(std::string filePath): isUsingAsm(true), isTesting(false), isUsingThreads(true), n(n), isUsingCSV(true), m_Func(new TestFunction(1,3))
		{
			// mozna dodac try catcha jak nie ma pliku
			CSVFile = std::ifstream(filePath.c_str());

			CSVRow row;
			CSVFile >> row;
			a = atof(row[0].c_str());
			b = atof(row[1].c_str());
			n = atoi(row[2].c_str()) - 1;	// n to maksymalny indeks w�z�a interpolacji. Dekrementowany z uwagi na liczenie od 0.

			x = new double[n+3];
			y = new double[n+3];
			f = new double[n+3];
			aa = new double[n+3];
			bb = new double[n+3];
			init();
		}

		/**
		 * Konstruktor obiektu klasy modelu interpolacji trygonometrycznej dla funkcji interpolowanej
		 * zawieraj�cej ztablicowane warto�ci funkcji w w�z�ach interpolacji w zewn�trznym pliku CSV.
		 *
		 * @param filePath �cie�ka zewn�trznego pliku CSV zawieraj�cego ztablicowane warto�ci funkcji w w�z�ach interpolacji, liczb� w�z��w interpolacji oraz ich przedzia�
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
			n = atoi(row[2].c_str()) - 1;	// n to maksymalny indeks w�z�a interpolacji. Dekrementowany z uwagi na liczenie od 0.

			x = new double[n+3];
			y = new double[n+3];
			f = new double[n+3];
			aa = new double[n+3];
			bb = new double[n+3];
			init();
		}

		/**
		 * Destruktor zwalniaj�cy zaalokowan� pami�� oraz zamykaj�cy strumie� odczytu pliku CSV.
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
		 * Metoda inicjalizuj�ca wykonanie algorytmu. U�ywana w konstruktorach oraz dost�pna publicznie w celu
		 * umo�liwienia wykonania algorytmu z wprowadzonymi modyfikacjami w sposobie jego dzia�ania (wielow�tkowo��, u�ycie asemblera).
		 */
		void init() {
			generateNodes();
			generateFactors();	
		}	

		/**
		 * Metoda obliczaj�ca warto�� funkcji interpoluj�cej w danym punkcie.
		 *
		 * @param t Punkt, kt�rego warto�� funkcji chcemy uzyska�.
		 */
		double getPolynomialValue(double t) const;

		/**
		 * Metoda generuj�ca raport dotycz�cy funkcji interpolowanej - w�z��w interpolacji wraz z ich warto�ciami
		 * w funkcji interpolowanej.
		 *
		 * @param fileName �cie�ka zawieraj�ca nazw� pliku do maj�cego posta� raportu.
		 */
		void generateReport1(char* fileName);

		/**
		 * Metoda generuj�ca raport dotycz�cy funkcji interpoluj�cej - wygenerowane punkty wraz z ich warto�ciami
		 * w funkcji interpoluj�cej.
		 *
		 * @param fileName �cie�ka zawieraj�ca nazw� pliku do maj�cego posta� raportu.
		 * @param mn Liczba punkt�w po�rednich generowanych pomi�dzy w�z�ami interpolacji.
		 */
		void generateReport2(const char* fileName, int mn);	

		/**
		 * Metoda wykorzystywana w w�tku wywo�ywanego z adresem do metody generateFactorsThread.
		 *
		 * @param begin Zakres oblicze� - pocz�tek.
		 * @param end Zakres oblicze� - koniec.
		 */
		void generateFactorsValues(int begin, int end);						
		
		/**
		 * Metoda wykorzystywana w w�tku wywo�ywanego z adresem do metody generateReportThread.
		 *
		 * @param begin Zakres oblicze� - pocz�tek.
		 * @param end Zakres oblicze� - koniec.
		 */
		void generateReportValues(int begin, int end, FILE* plk, int mn);
		
		/**
		 * Zmienna okre�laj�ca czy uzywane sa dane z pliku csv.
		 */
		bool isUsingCSV;			
		/**
		 * Zmienna okre�laj�ca czy u�ywany jest kod asemblera.
		 */
		bool isUsingAsm;			
		/**
		 * Zmienna okre�laj�ca czy u�ywane s� w�tki.
		 */
		bool isUsingThreads;
		/**
		 * Zmienna okre�laj�ca czy wykonywane s� testy wydajno�ciowe. Wy��cza wi�kszo�� zapis�w do plik�w.
		 */
		bool isTesting;

	};

}