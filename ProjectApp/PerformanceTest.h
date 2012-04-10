#pragma once

#include <iostream>
#include <string>

namespace Project {

	/**
	 *	Klasa u¿ywana do przeprowadzania testów wydajnoœciowych.
	 *
	 *	@author Michal Krupinski
	 *	@version 1.0
	 */

	class PerformanceTest
	{
	private:
		/**
		 * Œcie¿ka do pliku z wynikami.
		 */
		std::string m_Path;
		/**
		 * Pocz¹tkowa iloœæ wêz³ów interpolacji.
		 */
		int m_BeginValue;
		/**
		 * Koñcowa iloœæ wêz³ów interpolacji.
		 */
		int m_EndValue;
		/**
		 * Iloœæ generowanych punktów poœrednich.
		 */
		int m_Points;
		/**
		 * Wartoœæ kroku, o jaki zwiêkszana jest iloœæ wêz³ów interpolacji.
		 */
		int m_Move;

	public:
		/**
		 * Konstruktor.
		 *
		 * @param path Œcie¿ka do pliku z wynikami.
		 * @param begin Pocz¹tkowa iloœæ wêz³ów interpolacji.
		 * @param end Koñcowa iloœæ wêz³ów interpolacji.
		 * @param points Iloœæ generowanych punktów poœrednich.
		 * @param move Wartoœæ kroku, o jaki zwiêkszana jest iloœæ wêz³ów interpolacji.
		 */
		PerformanceTest(std::string& path, int begin, int end, int points, int move): m_Path(path), m_BeginValue(begin), m_EndValue(end), m_Points(points), m_Move(move) {}
		
		/**
		 * Destruktor bezparametrowy.
		 */
		~PerformanceTest() {}

		/**
		 * Metoda rozpoczynaj¹ca testy wydajnoœciowe.
		 */
		void start();
	};


}