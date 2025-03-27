#include "CSVParser.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

std::vector<StockData> CSVParser::stockData;

CSVParser::CSVParser()
{
	conditionLogicMap[STOCK_PRICE_RANK]				= &CSVParser::StockPriceRank;
	conditionLogicMap[FLUCTUATION_RANK]				= &CSVParser::FluctuationRateRank;
	conditionLogicMap[UPPER_UNDER_RANGE]			= &CSVParser::UpperUnderLimitRange;
	conditionLogicMap[MARKET_CAPITALIZATION_RANGE]	= &CSVParser::MarketCapitalizationRange;
}

CSVParser::~CSVParser()
{
}

bool CSVParser::ParsingCSV(CListCtrl* listCtrl)
{
	ifstream file("..//CSVFile//search_data.CSV"); // 읽을 CSV 파일 이름
	string line;

	if (file.is_open() == false)
		return false;

	bool bInsertedColumnName = false;
	int columnIndex = 1;

	stockData.reserve(3005);

	while (getline(file, line)) {
		vector<std::string> row;
		istringstream stream(line);
		string cell;

		while (std::getline(stream, cell, ','))
			row.push_back(cell);

		if (bInsertedColumnName == false)
		{
			for (const auto& cell : row)
			{
				listCtrl->InsertColumn(columnIndex, MultibyteToUnicode(cell.c_str()).c_str(), LVCFMT_CENTER, 100);
				columnIndex++;
			}
			bInsertedColumnName = true;
		}
		else
		{
			int currentPrice = stoi(row[1]);
			row[2].pop_back();
			float fluctuationRate = stof(row[2]);
			int tradingVolume = stoi(row[3]);
			int uppperLimit = stoi(row[4]);
			int underLimit = stoi(row[5]);
			int marketCapitalization = stoi(row[6]);
			stockData.emplace_back(StockData{row[0], currentPrice, fluctuationRate, tradingVolume, uppperLimit, underLimit, marketCapitalization});
		}
	}

	file.close();

	return true;
}

bool CSVParser::SearchData(CListCtrl* listCtrl, const std::vector<std::string>& ctrlValues, const std::string& conditionId)
{
	if (conditionLogicMap.find(conditionId) != conditionLogicMap.end())
	{
		conditionLogicMap[conditionId](listCtrl, ctrlValues);
		return true;
	}
	else
		return false;
}

void CSVParser::StockPriceRank(CListCtrl* listCtrl, const std::vector<std::string>& ctrlValues)
{
	if (ctrlValues[0] == "상위")
	{
		sort(stockData.begin(), stockData.end(), StockData::CurrentPriceDescending);
	}
	else
	{
		sort(stockData.begin(), stockData.end(), StockData::CurrentPriceAscending);
	}

	int itemCount = stoi(ctrlValues[1]);
	for (int i = 0; i < itemCount; i++)
	{
		InsertItemToList(listCtrl, stockData[i]);
	}
}

void CSVParser::FluctuationRateRank(CListCtrl* listCtrl, const std::vector<std::string>& ctrlValues)
{
	if (ctrlValues[0] == "상위순")
	{
		sort(stockData.begin(), stockData.end(), StockData::FluctuationRateDescending);
	}
	else
	{
		sort(stockData.begin(), stockData.end(), StockData::FluctuationRateAscending);
	}

	int itemCount = stoi(ctrlValues[1]);
	for (int i = 0; i < itemCount; i++)
	{
		InsertItemToList(listCtrl, stockData[i]);
	}
}

void CSVParser::UpperUnderLimitRange(CListCtrl* listCtrl, const std::vector<std::string>& ctrlValues)
{
	const int under = stoi(ctrlValues[1]);
	const int upper = stoi(ctrlValues[2]);

	for (auto& data : stockData)
	{
		if (ctrlValues[0] == "상한가")
		{
			if (data.uppperLimit >= under && data.uppperLimit <= upper)
			{
				InsertItemToList(listCtrl, data);
			}
		}
		else
		{
			if (data.underLimit >= under && data.underLimit <= upper)
			{
				InsertItemToList(listCtrl, data);
			}
		}
	}
}

void CSVParser::MarketCapitalizationRange(CListCtrl* listCtrl, const std::vector<std::string>& ctrlValues)
{
	const int under = stoi(ctrlValues[0]);
	const int upper = stoi(ctrlValues[1]);

	for (auto& data : stockData)
	{
		if (data.marketCapitalization >= under && data.marketCapitalization <= upper)
		{
			InsertItemToList(listCtrl, data);
		}
	}
}

void CSVParser::InsertItemToList(CListCtrl* listCtrl, const StockData& stockData)
{
	int index = listCtrl->InsertItem(listCtrl->GetItemCount(), const_cast<LPWSTR>(MultibyteToUnicode(stockData.name.c_str()).c_str()));

	listCtrl->SetItemText(index, 1, to_wstring(stockData.currentPrice).c_str());
	listCtrl->SetItemText(index, 2, to_wstring(stockData.fluctuationRate).c_str());
	listCtrl->SetItemText(index, 3, to_wstring(stockData.tradingVolume).c_str());
	listCtrl->SetItemText(index, 4, to_wstring(stockData.uppperLimit).c_str());
	listCtrl->SetItemText(index, 5, to_wstring(stockData.underLimit).c_str());
	listCtrl->SetItemText(index, 6, to_wstring(stockData.marketCapitalization).c_str());
}
