#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include "pch.h"
#include "framework.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

using namespace rapidjson;

class JsonParser
{
public:

	JsonParser();
	virtual ~JsonParser();

	void SaveStatusToJson(const COLORREF& bgColor, const COLORREF& fontColor, std::vector<std::string>& addedConditionIndexes = std::vector<std::string>(), std::unordered_map<std::string, std::shared_ptr<AddedConditionInfo>>& addedConditionInfo = std::unordered_map<std::string, std::shared_ptr<AddedConditionInfo>>());

	void LoadStatus(COLORREF& bgColor, COLORREF& fontColor, std::vector<std::string>& addedConditionIndexes, std::unordered_map<std::string, std::shared_ptr<AddedConditionInfo>>& addedConditionInfo, std::string& lastIndex);

protected:

	void AddStringToValue(Document& doc, Value& val, const std::string& str, const char* name);

	void MakeFolder();


private:

	std::string filePath = "..//Status//status.json";

};

