#pragma once

#include "pch.h"
#include <unordered_map>

class CSVParser
{
public:

	CSVParser();

	virtual ~CSVParser();

	bool ParsingCSV(CListCtrl* listCtrl);

	bool SearchData(CListCtrl* listCtrl, const std::vector<std::string>& ctrlValues, const std::string& conditionId);

protected:

	static void StockPriceRank(CListCtrl* listCtrl, const std::vector<std::string>& ctrlValues);

	static void FluctuationRateRank(CListCtrl* listCtrl, const std::vector<std::string>& ctrlValues);

	static void UpperUnderLimitRange(CListCtrl* listCtrl, const std::vector<std::string>& ctrlValues);

	static void MarketCapitalizationRange(CListCtrl* listCtrl, const std::vector<std::string>& ctrlValues);

	static void InsertItemToList(CListCtrl* listCtrl, const StockData& stockData);

private:

	const char* CSV_PATH = "..//CSVFile//search_data.CSV";

	static std::vector<StockData> stockData;

	std::unordered_map<std::string, void(*)(CListCtrl*, const std::vector<std::string>&)> conditionLogicMap;

};

