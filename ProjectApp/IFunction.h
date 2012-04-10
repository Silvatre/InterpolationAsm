#pragma once

namespace Project {

	/**
	 *	Klasa abstrakcyjna bêd¹ca interfejsem, które musz¹ implementowaæ funkcje interpolowane przy u¿yciu klasy modelu
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
		 * Metoda zwracaj¹ca doln¹ granicê przedzia³u.
		 */
		virtual double getLeftRange() const = 0;

		/**
		 * Metoda zwracaj¹ca górn¹ granicê przedzia³u.
		 */
		virtual double getRightRange() const = 0;

		/**
		 * Metoda zwracaj¹ca wartoœæ funkcji w danym punkcie.
		 * @param y Punkt.
		 */
		virtual double getValue(double y) const = 0;
	};

}