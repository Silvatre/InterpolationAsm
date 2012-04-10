#pragma once
#include <math.h>
#include "IFunction.h"

namespace Project {

	/**
	 *	Klasa reprezentuj¹ca testow¹ funkcjê interpolowan¹ implementuj¹ca interfejs (dziedzicz¹ca po klasie abstrakcyjnej) IFunction.
	 *
	 *	@author Michal Krupinski
	 *	@version 1.0
	 */

	class TestFunction: public IFunction
	{
	private:
		/**
		 * Dolna granica przedzia³u zawieraj¹cego wêz³y interpolacji.
		 */
		double a;
		/**
		 * Górna granica przedzia³u zawieraj¹cego wêz³y interpolacji.
		 */
		double b;	

	public:
		/**
		 * Konstruktor.
		 *
		 * @param LeftRange Dolna granica przedzia³u zawieraj¹cego wêz³y interpolacji. 
		 * @param RightRange Górna granica przedzia³u zawieraj¹cego wêz³y interpolacji.
		 */
		TestFunction(double LeftRange, double RightRange): a(LeftRange), b(RightRange) {};

		/**
		 * Destruktor bezparametrowy.
		 */
		~TestFunction() {};

		/**
		 * Implementacja metody z klasy abstrakcyjnej (interfejsu) IFunction. 
		 * Zwraca doln¹ granicê przedzia³u.
		 */
		virtual double getLeftRange() const {
			return a;
		}

		/**
		 * Implementacja metody z klasy abstrakcyjnej (interfejsu) IFunction. 
		 * Zwraca górn¹ granicê przedzia³u.
		 */
		virtual double getRightRange() const {
			return b;
		}

		/**
		 * Implementacja metody z klasy abstrakcyjnej (interfejsu) IFunction. 
		 * Zwraca watoœæ funkcji interpolowanej w danym punkcie.
		 * @param y Punkt
		 */
		virtual double getValue(double y) const;	// Funkcja g(y) na podstawie której wyznaczane s¹ wartoœci w wêz³ach interpolacji
											// y nale¿y do przedzia³u <a, b>
	};

}