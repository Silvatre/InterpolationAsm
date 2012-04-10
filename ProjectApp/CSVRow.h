#pragma once

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

	/**
	 *	Klasa reprezentuj¹ca wiersz w pliku CSV.
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
		 * Przeci¹¿enie operatora indeksu w celu zwracania wartoœci kolumny o danym indeksie
		 * w obecnym wierszu.
		 *
		 * @param index Indeks kolumny.
		 */
        std::string const& operator[](std::size_t index) const;

		/**
		 * Metoda zwracaj¹ca iloœæ kolumn w obecnym wierszu.
		 */
        std::size_t size() const;

		/**
		 * Metoda czytaj¹ca kolejny wiersz z pliku CSV.
		 */
        void readNextRow(std::istream& str);    
};

/**
 * Przeci¹¿enie operatora ">>" strumienia istream do poprawnej obs³ugi CSVRow.
 *
 * @param str Strumieñ istream
 * @param data Referencja do zmiennej CSVRow.
 */

std::istream& operator>>(std::istream& str,CSVRow& data);