// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"
#include <cstring>
#include <string>
#include <vector>

#define CM_DELETE_CONDITIONITEM (WM_USER + 1)
#define CM_UP_CONDITIONITEM (WM_USER + 2)
#define CM_DOWN_CONDITIONITEM (WM_USER + 3)
#define CM_CLICK_CONDITIONITEM (WM_USER + 4)

#define CONT_TYPE "TYPE"
#define CONT_ID "ID"
#define CONT_RADIOGROUP_ID "RADIOGROUP_ID"
#define CONT_PACKET_ID "PACKET_ID"
#define CONT_STATIC_CAPTION "STATIC_CAPTION"
#define CONT_DEFAULT_VALUE "DEFAULT_VALUE"
#define CONT_EDIT_LENGTH "EDIT_LENGTH"
#define CONT_COMBO_LOAD_DATA "COMBO_LOAD_DATA"
#define CONT_COMPLETE_IDX_NAME "COMPLETE_INDEX_NAME"
#define CONT_COMBO_ITEM "COMBO_ITEM"
#define CONT_COMBO_INDEX "COMBO_INDEX"
#define CONT_LEFT "LEFT"
#define CONT_TOP_LINE "TOP_LINE"
#define CONT_WIDTH "WIDTH"
#define CONT_HEIGHT "HEIGHT"

#define STOCK_PRICE_RANK "A23"
#define FLUCTUATION_RANK "D1_3"
#define UPPER_UNDER_RANGE "D3_18"
#define MARKET_CAPITALIZATION_RANGE "D3_6"
 
enum class EControlType
{
	ECT_Static,
	ECT_ComboBox,
	ECT_Radio,
	ECT_Edit,
	ECT_None
};

struct ControlAttribute
{
	EControlType controlType;
	int id;
	std::string radioGroupId;
	std::string packetId;
	std::string staticCaption;
	std::string defaultValue;
	int editLength;
	std::vector<std::string> comboItem;
	std::vector<std::string> comboIndex;
	int left;
	float topLine;
	int width;
	int height;
};

struct AddedConditionInfo
{
	std::vector<std::pair<CWnd*, ControlAttribute>> currentControls;

	std::vector<std::pair<int, std::string>> savedControlValues;

	std::vector<std::string> currentCtrlValues;

	std::string addedConditionId;

	std::string completeText;

	CString completeTextCStr;
};

std::wstring MultibyteToUnicode(const char* str);

struct StockData
{
	std::string name;
	int currentPrice;
	float fluctuationRate;
	int tradingVolume;
	int uppperLimit;
	int underLimit;
	int marketCapitalization;

	static bool CurrentPriceAscending(StockData& a, StockData& b)
	{
		return a.currentPrice < b.currentPrice;
	}

	static bool CurrentPriceDescending(StockData& a, StockData& b)
	{
		return a.currentPrice > b.currentPrice;
	}

	static bool FluctuationRateAscending(StockData& a, StockData& b)
	{
		return a.fluctuationRate < b.fluctuationRate;
	}

	static bool FluctuationRateDescending(StockData& a, StockData& b)
	{
		return a.fluctuationRate > b.fluctuationRate;
	}
};

#endif //PCH_H