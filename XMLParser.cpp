#include <iostream>
#include <sstream>
#include <queue>
#include "XMLParser.h"

using namespace std;

XMLParser::XMLParser()
{
	implementedConditionNames.reserve(4);

	implementedConditionNames.push_back(STOCK_PRICE_RANK);
	implementedConditionNames.push_back(FLUCTUATION_RANK);
	implementedConditionNames.push_back(UPPER_UNDER_RANGE);
	implementedConditionNames.push_back(MARKET_CAPITALIZATION_RANGE);
}

XMLParser::~XMLParser()
{

}

bool XMLParser::InsertTreeCommonData(CTreeCtrl* treeCtrl)
{
	TiXmlDocument readDoc;
	readDoc.LoadFile(TREE_FILE_PATH);
	TiXmlElement* readRoot = readDoc.FirstChildElement();

	if (readRoot)
	{
		DFS(treeCtrl, nullptr, readRoot->FirstChildElement());
		return true;
	}
	else
		return false;
}

void XMLParser::DFS(CTreeCtrl* treeCtrl, HTREEITEM treeItemHandle, TiXmlElement* node)
{
	if (treeCtrl == NULL)
		return;

	TiXmlElement* child = nullptr;
	HTREEITEM treeChildItem = nullptr;
	while (node)
	{
		auto val = node->Attribute("NAME");
		const string nodeId = node->Value();
		const char* str = val ? val : nodeId.c_str();

		wstring wName = MultibyteToUnicode(str);

		MarkImplementedFunction(nodeId, wName);

		treeChildItem = treeCtrl->InsertItem(wName.c_str(), 0, 0, treeItemHandle == nullptr ? TVI_ROOT : treeItemHandle, TVI_LAST);

		child = node->FirstChildElement();
		if (child)
		{
			DFS(treeCtrl, treeChildItem, child);
		}
		else if (val)
		{
			conditionIdMap[wName].id = string(nodeId);
			const string fileName = ToLower(node->Attribute(FILE_NAME));
			conditionIdMap[wName].fileName = fileName;
			fileNameSet.insert(fileName);
		}

		node = node->NextSiblingElement();
		treeChildItem = treeItemHandle;
	}
}

void XMLParser::MarkImplementedFunction(const string& nodeId, wstring& wName)
{
	for (int i = 0; i < implementedConditionNames.size(); i++)
	{
		if(nodeId == implementedConditionNames[i])
			wName = L"★" + wName;
	}
}

string XMLParser::ToLower(const char* str)
{
	string lowerStr = str;

	for (int i = 0; i < lowerStr.length(); i++)
	{
		lowerStr[i] = tolower(lowerStr[i]);
	}

	return lowerStr;
}

bool XMLParser::ReadMapFile()
{
	for (set<string>::iterator it = fileNameSet.begin(); it != fileNameSet.end(); ++it)
	{
		string mapFilePATH = MAP_FILE_PATH + *it + EXT_XML;

		TiXmlDocument readDoc;
		readDoc.LoadFile(mapFilePATH.c_str());
		TiXmlElement* readRoot = readDoc.FirstChildElement();

		DFSMapFile(readRoot->FirstChildElement(), readRoot->FirstChildElement());
	}

	return false;
}

void XMLParser::DFSMapFile(TiXmlElement* parent, TiXmlElement* current)
{
	while (current)
	{
		auto child = current->FirstChildElement();
		
		if (child)
		{
			string childVal = child->Value();
			if (childVal == "CONTROL")
			{
				string curId = string(current->Value()), compIdxName = CONT_COMPLETE_IDX_NAME;
				int idx = 1;
				while (current->Attribute(compIdxName.c_str()))
				{
					const string value = current->Attribute(compIdxName.c_str());
					completeIndexNameMap[curId].push_back(value);
					compIdxName = CONT_COMPLETE_IDX_NAME + to_string(++idx);
				}
			}
			DFSMapFile(current, child);
		}
		else
		{
			if (IsControl(current))
			{
				ParsingControlAttribute(parent->Value(), current);
			}
		}
		current = current->NextSiblingElement();
	}
}

bool XMLParser::IsControl(TiXmlElement* node)
{
	return node->Attribute("TYPE") != NULL ? true : false;
}

void XMLParser::ParsingControlAttribute(const std::string& parentName, TiXmlElement* node)
{
	const EControlType controlType = GetControlType(node->Attribute(CONT_TYPE));
	if (controlType == EControlType::ECT_None)
		return;

	controlAttMap[parentName].emplace_back();
	auto& controlAtt = controlAttMap[parentName].back();

	// 구조체 값 저장
	controlAtt.controlType = controlType;
	controlAtt.id = stoi(node->Attribute(CONT_ID));

	const char* radioGroupId = node->Attribute(CONT_RADIOGROUP_ID);
	controlAtt.radioGroupId = radioGroupId != NULL ? radioGroupId : "";

	controlAtt.packetId = node->Attribute(CONT_PACKET_ID);

	const char* staticCaption = node->Attribute(CONT_STATIC_CAPTION);
	controlAtt.staticCaption = staticCaption != NULL ? staticCaption : "";

	const char* defaultValue = node->Attribute(CONT_DEFAULT_VALUE);
	controlAtt.defaultValue = defaultValue != NULL ? defaultValue : "";

	const char* editLength = node->Attribute(CONT_EDIT_LENGTH);
	if (editLength != NULL)
	{
		string editLengthStr(editLength);
		controlAtt.editLength = editLengthStr.length() > 0 ? stoi(editLengthStr) : 0;
	}

	string str = CONT_COMPLETE_IDX_NAME;
	str += "1";

	if (controlAtt.controlType == EControlType::ECT_ComboBox)
	{
		ParsingComboAttributeValue(controlAtt.comboItem, node->Attribute(CONT_COMBO_ITEM));
		ParsingComboAttributeValue(controlAtt.comboIndex, node->Attribute(CONT_COMBO_INDEX));
	}

	string temp = node->Attribute(CONT_LEFT);
	if (temp.length() > 0)
		controlAtt.left = stoi(temp);

	temp = node->Attribute(CONT_TOP_LINE);
	if (temp.length() > 0)
		controlAtt.topLine = stof(temp);

	temp = node->Attribute(CONT_WIDTH);
	if (temp.length() > 0)
		controlAtt.width = stoi(temp);

	temp = node->Attribute(CONT_HEIGHT);
	if (temp.length() > 0)
		controlAtt.height = stoi(temp);
}

EControlType XMLParser::GetControlType(std::string str)
{
	if (str == "Static")
		return EControlType::ECT_Static;
	else if (str == "ComboBox")
		return EControlType::ECT_ComboBox;
	else if (str == "Edit")
		return EControlType::ECT_Edit;
	else if (str == "Radio")
		return EControlType::ECT_Radio;
	else
		return EControlType::ECT_None;
}

void XMLParser::ParsingComboAttributeValue(vector<string>& strVec, string value)
{
	string tmp;
	stringstream sstream(value);

	while (getline(sstream, tmp, '/'))
	{
		strVec.push_back(tmp.c_str());
	}
}

const std::vector<ControlAttribute>& XMLParser::GetControlInfo(const CString& conditionName)
{
	string conditionId = conditionIdMap[conditionName.GetString()].id;
	return GetControlInfo(conditionId);
}

const std::vector<ControlAttribute>& XMLParser::GetControlInfo(const std::string& conditionId)
{
	assert(controlAttMap.find(conditionId) != controlAttMap.end());
	return controlAttMap[conditionId];
}

const std::vector<std::string>& XMLParser::GetCompleteIndexName(const CString& conditionName)
{
	string conditionId = conditionIdMap[conditionName.GetString()].id;
	return GetCompleteIndexName(conditionId);
}

const std::vector<std::string>& XMLParser::GetCompleteIndexName(const std::string& conditionId)
{
	assert(completeIndexNameMap.find(conditionId) != completeIndexNameMap.end());
	return completeIndexNameMap[conditionId];
}

bool XMLParser::IsCondition(CString& cStr)
{
	wstring key = cStr.GetString();
	if (conditionIdMap.find(key) != conditionIdMap.end())
	{
		return true;
	}
	else 
		return false;
}

string XMLParser::GetConditionId(const CString& conditionName)
{
	wstring wConditionName = conditionName.GetString();
	if (conditionIdMap.find(wConditionName) != conditionIdMap.end())
		return conditionIdMap[conditionName.GetString()].id;
	else
		return string();
}

void XMLParser::SaveConditionInfoToXML(const std::string& filePath, std::vector<std::string>& addedConditionIndexes, std::unordered_map<std::string, std::shared_ptr<AddedConditionInfo>>& addedConditionInfo)
{
	CMarkup xml;

	xml.AddElem(_T("CONDITION"));
	xml.IntoElem();

	for (string& index : addedConditionIndexes)
	{
		if (addedConditionInfo.find(index) == addedConditionInfo.end())
			continue;

		const wstring wIndex = MultibyteToUnicode(index.c_str());
		xml.AddElem(wIndex.c_str());

		const auto& condInfo = addedConditionInfo[index];

		const wstring wConditionId = MultibyteToUnicode(condInfo->addedConditionId.c_str());
		xml.AddAttrib(_T("ID"), wConditionId.c_str());

		const wstring wText = MultibyteToUnicode(condInfo->completeText.c_str());
		xml.AddAttrib(_T("NAME"), wText.c_str());

		xml.IntoElem();

		auto& ctrlValues = condInfo->savedControlValues;
		for (auto& pValue : ctrlValues)
		{
			xml.AddElem(_T("CONTROL"));

			wstring wId = MultibyteToUnicode(to_string(pValue.first).c_str());
			xml.AddAttrib(_T("ID"), wId.c_str());

			wstring wVal = MultibyteToUnicode(pValue.second.c_str());
			xml.AddAttrib(_T("VALUE"), wVal.c_str());
		}

		xml.OutOfElem();
	}

	xml.OutOfElem();

	wstring wFilePath = MultibyteToUnicode(filePath.c_str());
	xml.Save(wFilePath.c_str());
}

void XMLParser::LoadConditionInfo(const std::wstring& filePath, std::vector<std::string>& addedConditionIndexes, std::unordered_map<std::string, std::shared_ptr<AddedConditionInfo>>& addedConditionInfo)
{
	CMarkup xml;
	xml.Load(filePath.c_str());

	xml.FindElem(_T("CONDITION"));
	bool bValid = xml.IntoElem();
	while (bValid)
	{
		bValid = xml.FindElem();
		if (bValid == false)
			break;

		const string index = string(CT2CA(xml.GetTagName()));
		addedConditionIndexes.push_back(index);

		shared_ptr<AddedConditionInfo> condInfo = make_shared<AddedConditionInfo>();
		addedConditionInfo[index] = condInfo;

		condInfo->addedConditionId = string(CT2CA(xml.GetAttrib(_T("ID"))));
		const CString completeTextCStr = xml.GetAttrib(_T("NAME"));
		condInfo->completeTextCStr = completeTextCStr;
		condInfo->completeText = string(CT2CA(completeTextCStr));

		bool bValid2 = xml.IntoElem();
		while (bValid2)
		{
			bValid2 = xml.FindElem(_T("CONTROL"));
			if (bValid2 == false)
				break;
			const int id = _ttoi(xml.GetAttrib(_T("ID")));
			const string value = string(CT2CA(xml.GetAttrib(_T("VALUE"))));

			condInfo->savedControlValues.push_back(make_pair(id, value));
		}
		xml.OutOfElem();
	}
}
