#include "Interpolation.h"
#include "PerformanceTest.h"
#include <iostream>
#include <string>
#include "TestFunction.h"

using namespace std;
using namespace Project;

/**
 * Funkcja sprawdzaj¹ca istnienie pliku o podanej œcie¿ce.
 *
 * @param filePath Œcie¿ka pliku do sprawdzenia.
 */

bool FileExists(const char* filePath)
{
	FILE* fp = NULL;
	fp = fopen(filePath, "rb");
	if(fp != NULL)
	{
		fclose(fp);
		return true;
	}
	return false;
}

/**
 * Metoda main programu. Odpowiada za interakcjê z u¿ytkownikiem i wyœwietlanie
 * danych na standardowym wyjœciu.
 *
 * @param args Ilosc argumentów wejœciowych.
 * @param argv Tablica argumentów wejœciowych.
 *
 * @author Michal Krupinski
 * @version 1.0
 */

int main(int args, char** argv)
{
	char choice = 0;
	string path = "";
	double point = 0;
	int nodes = 0;
	double a, b;
	int move = 0;

	while(choice != 1) {
		cout << "\nInterpolacja trygonometryczna metoda Goertzela. \n\n";
		cout << "Wpisz numer akcji i zatwierdz: \n";
		cout << "1: Obliczenie wartosci funkcji interpolujacej w danym punkcie.\n";
		cout << "2: Wygenerowanie raportu dotyczacego funkcji interpolujacej.\n";
		cout << "3: Uruchomienie testow wydajnosciowych i wygenerowanie raportu.\n";
		cout << "4: Zakonczenie pracy programu.\n";
		std::cin >> choice;
		switch(choice)
		{
		case '1': 
		{
			cout << "\nPodaj zrodlo funkcji interpolowanej: \n";
			cout << "1. Zewnetrzny plik CSV. \n";
			cout << "2. Funkcja wbudowana (testowa). \n";
			std::cin >> choice;
			switch(choice)
			{
			case '1':
			{
				cout << "\nPodaj sciezke do pliku CSV: ";
				cin >> path;
				if(FileExists(path.c_str()))
				{
					Interpolation interpolatingFunc(path);
					choice = 't';
					while (choice == 't' || choice == 'T')
					{
						cout << "\nPodaj dla jakiego punktu obliczyc wartosci funkcji interpolujacej: ";
						cin >> point;
						cout << "Wartosc funkcji interpolujacej w punkcie " << point << " wynosi: " << interpolatingFunc.getPolynomialValue(point);
						cout << "\nPonowic operacje? (T/N): ";
						cin >> choice;
					}
					choice = 0;
				}
				else
				{
					cout << "\nBlad: Podany plik nie istnieje. \n";
					choice = 0;
					break;
				}
				break;
			}
			case '2':
			{
				cout << "\nPodaj ilosc wezlow interpolacji: ";
				cin >> nodes;
				if(nodes > 1)
				{
					cout << "\nPodaj dolna granice przedzialu wezlow interpolacji: ";
					cin >> a;
					cout << "\nPodaj gorna granice przedzialu wezlow interpolacji: ";
					cin >> b;
					if ( a >= b)
					{
						cout << "\nPodano nieprawidlowe granice.";
						choice = 0;
						break;
					}
					TestFunction* function =  new TestFunction(a, b);
					Interpolation interpolatingFunc(function, nodes-1);					
					choice = 't';
					while (choice == 't' || choice == 'T')
					{
						cout << "\nPodaj dla jakiego punktu obliczyc wartosci funkcji interpolujacej: ";
						cin >> point;
						cout << "\nWartosc funkcji interpolujacej w punkcie " << point << " wynosi: " << interpolatingFunc.getPolynomialValue(point);
						cout << "\nPonowic operacje? (T/N): ";
						cin >> choice;
					}
					delete function;
					choice = 0;
				}
				else
				{
					cout << "\nBlad: Nieprawidlowa ilosc wezlow interpolacji. \n";
					choice = 0;
					break;
				}
				break;
			}
			default:
				choice = 0;
				break;
			}
			break;
		}
		case '2':
		{
			cout << "\nPodaj zrodlo funkcji interpolowanej: \n";
			cout << "1. Zewnetrzny plik CSV. \n";
			cout << "2. Funkcja wbudowana (testowa). \n";
			std::cin >> choice;
			switch(choice)
			{
			case '1': 
				cout << "\nPodaj sciezke do pliku CSV: ";
				cin >> path;
				if(FileExists(path.c_str()))
				{
					Interpolation interpolatingFunc(path);
					cout << "\nPodaj nazwe raportu: ";
					cin >> path;
					cout << "\nPodaj ilosc punktow posrednich: ";
					cin >> point;
					interpolatingFunc.generateReport2(path.c_str(), point);
					cout << "\nWygenerowano raport: " << path << "\n";
					choice = 0;
				}
				else
				{
					cout << "\nBlad: Podany plik nie istnieje. \n";
					choice = 0;
					break;
				}
				break;
			case '2':
				cout << "\nPodaj ilosc wezlow interpolacji: ";
				cin >> nodes;
				if(nodes > 1)
				{
					cout << "\nPodaj ilosc punktow posrednich: ";
					cin >> point;
					cout << "\nPodaj dolna granice przedzialu wezlow interpolacji: ";
					cin >> a;
					cout << "\nPodaj gorna granice przedzialu wezlow interpolacji: ";
					cin >> b;
					cout << "\nPodaj nazwe raportu: ";
					cin >> path;
					if ( a >= b)
					{
						cout << "\nPodano nieprawidlowe granice.";
						choice = 0;
						break;
					}
					TestFunction* function =  new TestFunction(a, b);
					Interpolation interpolatingFunc(function, nodes-1);
					interpolatingFunc.generateReport2(path.c_str(), point);
					cout << "\nWygenerowano raport: " << path << "\n";
					delete function;
					choice = 0;
				}
				else
				{
					cout << "\nBlad: Nieprawidlowa ilosc wezlow interpolacji. \n";
					choice = 0;
					break;
				}
				break;
			default:
				choice = 0;
				break;
			}
			break;
		}
		case '3': 
		{
			cout << "\nPodaj ilosc punktow posrednich: ";
			cin >> point;
			cout << "\nPodaj poczatkowa ilosc wezlow interpolacji: ";
			cin >> a;
			cout << "\nPodaj koncowa ilosc wezlow interpolacji: ";
			cin >> b;
			if ( a > b)
			{
				cout << "\nPodano nieprawidlowe granice.";
				choice = 0;
				break;
			}
			cout << "\nPodaj wartosc kroku petli: ";
			cin >> move;
			if(move <= 0)
			{
				cout << "Blad: Podano nieprawidlowa wartosc kroku. \n";
				choice = 0;
				break;
			}
			cout << "\nPodaj nazwe raportu: \n";
			cin >> path;						
			PerformanceTest perfTest(path, (int)a, (int)b, point, move);
			perfTest.start();
			cout << "\nWygenerowano raport wydajnosciowy: " << path << "\n";
			choice = 0;
			break;
		}
		case '4':
		case 'q':
			choice = 1;
			break;
		default: 
			choice = 0;
			break;
		}
	}
}
