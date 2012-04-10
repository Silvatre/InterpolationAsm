#pragma once

#include <iostream>
#include <string>

namespace Project {

	/**
	 *	Klasa u�ywana do przeprowadzania test�w wydajno�ciowych.
	 *
	 *	@author Michal Krupinski
	 *	@version 1.0
	 */

	class PerformanceTest
	{
	private:
		/**
		 * �cie�ka do pliku z wynikami.
		 */
		std::string m_Path;
		/**
		 * Pocz�tkowa ilo�� w�z��w interpolacji.
		 */
		int m_BeginValue;
		/**
		 * Ko�cowa ilo�� w�z��w interpolacji.
		 */
		int m_EndValue;
		/**
		 * Ilo�� generowanych punkt�w po�rednich.
		 */
		int m_Points;
		/**
		 * Warto�� kroku, o jaki zwi�kszana jest ilo�� w�z��w interpolacji.
		 */
		int m_Move;

	public:
		/**
		 * Konstruktor.
		 *
		 * @param path �cie�ka do pliku z wynikami.
		 * @param begin Pocz�tkowa ilo�� w�z��w interpolacji.
		 * @param end Ko�cowa ilo�� w�z��w interpolacji.
		 * @param points Ilo�� generowanych punkt�w po�rednich.
		 * @param move Warto�� kroku, o jaki zwi�kszana jest ilo�� w�z��w interpolacji.
		 */
		PerformanceTest(std::string& path, int begin, int end, int points, int move): m_Path(path), m_BeginValue(begin), m_EndValue(end), m_Points(points), m_Move(move) {}
		
		/**
		 * Destruktor bezparametrowy.
		 */
		~PerformanceTest() {}

		/**
		 * Metoda rozpoczynaj�ca testy wydajno�ciowe.
		 */
		void start();
	};


}