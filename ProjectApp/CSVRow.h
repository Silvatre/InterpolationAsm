#pragma once

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

	/**
	 *	Klasa reprezentująca wiersz w pliku CSV.
	 *
	 *	@author Loki Astari, stackoverflow.com
	 */

class CSVRow
{
	private:
		/**
		 * Kontener danych.
		 */
        std::vector<std::string>    m_data;
    public:
		/**
		 * Przeciążenie operatora indeksu w celu zwracania wartości kolumny o danym indeksie
		 * w obecnym wierszu.
		 *
		 * @param index Indeks kolumny.
		 */
        std::string const& operator[](std::size_t index) const;

		/**
		 * Metoda zwracająca ilość kolumn w obecnym wierszu.
		 */
        std::size_t size() const;

		/**
		 * Metoda czytająca kolejny wiersz z pliku CSV.
		 */
        void readNextRow(std::istream& str);    
};

/**
 * Przeciążenie operatora ">>" strumienia istream do poprawnej obsługi CSVRow.
 *
 * @param str Strumień istream
 * @param data Referencja do zmiennej CSVRow.
 */

std::istream& operator>>(std::istream& str,CSVRow& data);