#pragma once
#include <math.h>
#include "IFunction.h"

namespace Project {

	/**
	 *	Klasa reprezentuj�ca testow� funkcj� interpolowan� implementuj�ca interfejs (dziedzicz�ca po klasie abstrakcyjnej) IFunction.
	 *
	 *	@author Michal Krupinski
	 *	@version 1.0
	 */

	class TestFunction: public IFunction
	{
	private:
		/**
		 * Dolna granica przedzia�u zawieraj�cego w�z�y interpolacji.
		 */
		double a;
		/**
		 * G�rna granica przedzia�u zawieraj�cego w�z�y interpolacji.
		 */
		double b;	

	public:
		/**
		 * Konstruktor.
		 *
		 * @param LeftRange Dolna granica przedzia�u zawieraj�cego w�z�y interpolacji. 
		 * @param RightRange G�rna granica przedzia�u zawieraj�cego w�z�y interpolacji.
		 */
		TestFunction(double LeftRange, double RightRange): a(LeftRange), b(RightRange) {};

		/**
		 * Destruktor bezparametrowy.
		 */
		~TestFunction() {};

		/**
		 * Implementacja metody z klasy abstrakcyjnej (interfejsu) IFunction. 
		 * Zwraca doln� granic� przedzia�u.
		 */
		virtual double getLeftRange() const {
			return a;
		}

		/**
		 * Implementacja metody z klasy abstrakcyjnej (interfejsu) IFunction. 
		 * Zwraca g�rn� granic� przedzia�u.
		 */
		virtual double getRightRange() const {
			return b;
		}

		/**
		 * Implementacja metody z klasy abstrakcyjnej (interfejsu) IFunction. 
		 * Zwraca wato�� funkcji interpolowanej w danym punkcie.
		 * @param y Punkt
		 */
		virtual double getValue(double y) const;	// Funkcja g(y) na podstawie kt�rej wyznaczane s� warto�ci w w�z�ach interpolacji
											// y nale�y do przedzia�u <a, b>
	};

}