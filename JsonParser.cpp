#include "JsonParser.h"

using namespace std;

JsonParser::JsonParser()
{

}

JsonParser::~JsonParser()
{

}

void JsonParser::SaveStatusToJson(const COLORREF& bgColor, const COLORREF& fontColor, std::vector<std::string>& addedConditionIndexes, std::unordered_map<std::string, std::shared_ptr<AddedConditionInfo>>& addedConditionInfo)
{
	Document doc(kObjectType);

	doc.AddMember("BgColor", static_cast<unsigned int>(bgColor), doc.GetAllocator());
	doc.AddMember("FontColor", static_cast<unsigned int>(fontColor), doc.GetAllocator());

	Value condList(kArrayType);
	for (const string& index : addedConditionIndexes)
	{
		if (addedConditionInfo.find(index) == addedConditionInfo.end())
			continue;

		auto& condInfo = addedConditionInfo[index];

		Value cond(kObjectType);

		AddStringToValue(doc, cond, index, "Index");
		
		{
			//Value valStr(kStringType);
			//valStr.SetString(condInfo->addedConditionId.c_str(), condInfo->addedConditionId.size());
			//cond.AddMember("ConditionId", valStr, doc.GetAllocator());
			AddStringToValue(doc, cond, condInfo->addedConditionId, "ConditionId");
		}

		//Value valStr2(kStringType);
		//valStr2.SetString(condInfo->completeText.c_str(), condInfo->completeText.size());
		//cond.AddMember("CompleteText", valStr2, doc.GetAllocator());
		AddStringToValue(doc, cond, condInfo->completeText, "CompleteText");
		
		Value valArr(kArrayType);
		auto& ctrlValues = condInfo->savedControlValues;
		for (auto& p : ctrlValues)
		{
			Value ctrlVal(kObjectType);
			ctrlVal.AddMember("Id", p.first, doc.GetAllocator());
			AddStringToValue(doc, ctrlVal, p.second, "Value");

			valArr.PushBack(ctrlVal, doc.GetAllocator());
		}
		cond.AddMember("Control", valArr, doc.GetAllocator());
		condList.PushBack(cond, doc.GetAllocator());
	}
	doc.AddMember("Condition", condList, doc.GetAllocator());

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	MakeFolder();

	std::ofstream output_file("..//Status//status.json");
	if (output_file.is_open()) {
		output_file << buffer.GetString();
		output_file.close();
	}
}

void JsonParser::LoadStatus(COLORREF& bgColor, COLORREF& fontColor, std::vector<std::string>& addedConditionIndexes, std::unordered_map<std::string, std::shared_ptr<AddedConditionInfo>>& addedConditionInfo, std::string& lastIndex)
{
	Document doc;
	ifstream ifs(filePath);
	if (!ifs.is_open()) {
		cout << "[Error] : No json file.\n";
		return;
	}

	string json((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));

	doc = Document(kObjectType);
	doc.Parse(json.c_str());

	lastIndex = "A";

	if (doc.HasMember("BgColor"))
	{
		bgColor = static_cast<COLORREF>(doc["BgColor"].GetUint());
	}
	if (doc.HasMember("FontColor"))
	{
		fontColor = static_cast<COLORREF>(doc["FontColor"].GetUint());
	}

	if (doc.HasMember("Condition"))
	{
		auto& condList = doc["Condition"].GetArray();
		for (auto& cond : condList)
		{
			if (cond.HasMember("Index") == false)
				continue;

			const string index = cond["Index"].GetString();
			addedConditionIndexes.push_back(index);
			lastIndex = lastIndex < index ? index : lastIndex;

			shared_ptr<AddedConditionInfo> condInfo = make_shared<AddedConditionInfo>();

			if (cond.HasMember("ConditionId"))
				condInfo->addedConditionId = cond["ConditionId"].GetString();
			if (cond.HasMember("CompleteText"))
			{
				condInfo->completeText = cond["CompleteText"].GetString();
				condInfo->completeTextCStr = MultibyteToUnicode(cond["CompleteText"].GetString()).c_str();
			}

			if (cond.HasMember("Control"))
			{
				auto& ctrlValues = cond["Control"].GetArray();
				for (auto& val : ctrlValues)
				{
					pair<int, string> p;
					if (val.HasMember("Id"))
					{
						p.first = val["Id"].GetInt();
					}
					if (val.HasMember("Value"))
					{
						p.second = val["Value"].GetString();
					}
					condInfo->savedControlValues.push_back(p);
				}
			}

			addedConditionInfo[index] = condInfo;
		}
	}

	if (lastIndex == "Z")
		lastIndex = "a";
	else if(addedConditionInfo.size())
		lastIndex[0]++;
}

void JsonParser::AddStringToValue(Document& doc, Value& val, const std::string& str, const char* name)
{
	Value valStr(kStringType);
	valStr.SetString(str.c_str(), str.size());
	val.AddMember(rapidjson::StringRef(name), valStr, doc.GetAllocator());
}

void JsonParser::MakeFolder()
{
	wstring filePath = L"..//Status";
	if (GetFileAttributes(filePath.c_str()) == INVALID_FILE_ATTRIBUTES)
	{
		if (!CreateDirectory(filePath.c_str(), NULL))
			return;
	}
}
