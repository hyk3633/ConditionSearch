#pragma once
#include "tinyxml.h"
#include "Markup.h"
#include "pch.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

#define DATA_SIZE 256
#define ID_SIZE 32

struct ConditionData
{
	std::string id;
	std::string fileName;
};

class XMLParser
{
public:

	XMLParser();

	virtual ~XMLParser();

	bool InsertTreeCommonData(CTreeCtrl* treeCtrl);

protected:

	void DFS(CTreeCtrl* treeCtrl, HTREEITEM treeItemHandle, TiXmlElement* node);

	void MarkImplementedFunction(const std::string& nodeId, std::wstring& wName);

	std::string ToLower(const char* str);

public:

	bool ReadMapFile();

protected:

	void DFSMapFile(TiXmlElement* parent, TiXmlElement* current);

	bool IsControl(TiXmlElement* node);

	void ParsingControlAttribute(const std::string& parentName, TiXmlElement* node);

	EControlType GetControlType(std::string str);

	void ParsingComboAttributeValue(std::vector<std::string>& strVec, std::string value);

public:

	const std::vector<ControlAttribute>& GetControlInfo(const CString& conditionName);

	const std::vector<ControlAttribute>& GetControlInfo(const std::string& conditionId);

	const std::vector<std::string>& GetCompleteIndexName(const CString& conditionName);

	const std::vector<std::string>& GetCompleteIndexName(const std::string& conditionId);

	bool IsCondition(CString& cStr);

	std::string GetConditionId(const CString& conditionName);

	void SaveConditionInfoToXML(const std::string& filePath, std::vector<std::string>& addedConditionIndexes, std::unordered_map<std::string, std::shared_ptr<AddedConditionInfo>>& addedConditionInfo);

	void LoadConditionInfo(const std::wstring& filePath, std::vector<std::string>& addedConditionIndexes, std::unordered_map<std::string, std::shared_ptr<AddedConditionInfo>>& addedConditionInfo);

private:

	const char* TREE_FILE_PATH = "..//XMLFile//treecommon.xml";

	std::string MAP_FILE_PATH = "..//XMLFile//map//map";

	std::string EXT_XML = ".xml";

	const char* FILE_NAME = "FILE_NAME";

	wchar_t wName[DATA_SIZE];
	wchar_t wId[ID_SIZE];

	std::unordered_map<std::wstring, ConditionData> conditionIdMap;

	std::set<std::string> fileNameSet;

	std::unordered_map<std::string, std::vector<ControlAttribute>> controlAttMap;

	std::unordered_map <std::string, std::vector<std::string>> completeIndexNameMap;

	std::vector<std::string> implementedConditionNames;

};