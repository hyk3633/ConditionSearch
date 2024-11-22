#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "framework.h"

class JsonParser
{
public:

	JsonParser();
	virtual ~JsonParser();

	void SaveStatusToJson(const COLORREF& bgColor, const COLORREF& fontColor, const std::string& conditionId = "", const std::string& completeText = "", const std::vector<std::pair<int, std::string>>& controlValues = std::vector<std::pair<int, std::string>>());

	void LoadStatus(COLORREF& bgColor, COLORREF& fontColor, std::string& conditionId, std::string& completeText, std::vector<std::pair<int, std::string>>& controlValues);

protected:

	void MakeFolder();


private:

	std::string filePath = "..//Status//status.json";

};

