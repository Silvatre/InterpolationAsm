#pragma once

namespace Project {

	/**
	 *	Klasa abstrakcyjna b�d�ca interfejsem, kt�re musz� implementowa� funkcje interpolowane przy u�yciu klasy modelu
	 *	Interpolation.
	 *
	 *	@author Michal Krupinski
	 *	@version 1.0
	 */

	class IFunction
	{
	public:
		/**
		 * Destruktor.
		 */
		virtual ~IFunction() {};

		/**
		 * Metoda zwracaj�ca doln� granic� przedzia�u.
		 */
		virtual double getLeftRange() const = 0;

		/**
		 * Metoda zwracaj�ca g�rn� granic� przedzia�u.
		 */
		virtual double getRightRange() const = 0;

		/**
		 * Metoda zwracaj�ca warto�� funkcji w danym punkcie.
		 * @param y Punkt.
		 */
		virtual double getValue(double y) const = 0;
	};

}