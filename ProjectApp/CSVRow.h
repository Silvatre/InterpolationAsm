#pragma once

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

	/**
	 *	Klasa reprezentuj�ca wiersz w pliku CSV.
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
		 * Przeci��enie operatora indeksu w celu zwracania warto�ci kolumny o danym indeksie
		 * w obecnym wierszu.
		 *
		 * @param index Indeks kolumny.
		 */
        std::string const& operator[](std::size_t index) const;

		/**
		 * Metoda zwracaj�ca ilo�� kolumn w obecnym wierszu.
		 */
        std::size_t size() const;

		/**
		 * Metoda czytaj�ca kolejny wiersz z pliku CSV.
		 */
        void readNextRow(std::istream& str);    
};

/**
 * Przeci��enie operatora ">>" strumienia istream do poprawnej obs�ugi CSVRow.
 *
 * @param str Strumie� istream
 * @param data Referencja do zmiennej CSVRow.
 */

std::istream& operator>>(std::istream& str,CSVRow& data);