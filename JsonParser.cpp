#include "JsonParser.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

using namespace rapidjson;
using namespace std;

JsonParser::JsonParser()
{

}

JsonParser::~JsonParser()
{

}

void JsonParser::SaveStatusToJson(const COLORREF& bgColor, const COLORREF& fontColor, const std::string& conditionId, const std::string& completeText, const std::vector<std::pair<int, std::string>>& controlValues)
{
	Document doc(kObjectType);

	doc.AddMember("BgColor", static_cast<unsigned int>(bgColor), doc.GetAllocator());
	doc.AddMember("FontColor", static_cast<unsigned int>(fontColor), doc.GetAllocator());

	if (conditionId.length() > 0)
	{
		Value val(kStringType);
		val.SetString(conditionId.c_str(), conditionId.size());
		doc.AddMember("ConditionId", val, doc.GetAllocator());
	}
	if (completeText.length() > 0)
	{
		Value val(kStringType);
		val.SetString(completeText.c_str(), completeText.size());
		doc.AddMember("CompleteText", val, doc.GetAllocator());
	}
	if (controlValues.size() > 0)
	{
		Value controls(kArrayType);
		for (auto& value : controlValues)
		{
			Value ctrlValue(kObjectType);

			ctrlValue.AddMember("Id", value.first, doc.GetAllocator());

			Value val(kStringType);
			val.SetString(value.second.c_str(), value.second.size());
			ctrlValue.AddMember("Value", val, doc.GetAllocator());

			controls.PushBack(ctrlValue, doc.GetAllocator());
		}
		doc.AddMember("Control", controls, doc.GetAllocator());
	}

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

void JsonParser::LoadStatus(COLORREF& bgColor, COLORREF& fontColor, std::string& conditionId, std::string& completeText, std::vector<std::pair<int, std::string>>& controlValues)
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

	if (doc.HasMember("BgColor"))
	{
		bgColor = static_cast<COLORREF>(doc["BgColor"].GetUint());
	}
	if (doc.HasMember("FontColor"))
	{
		fontColor = static_cast<COLORREF>(doc["FontColor"].GetUint());
	}
	if (doc.HasMember("ConditionId"))
	{
		conditionId = doc["ConditionId"].GetString();
	}
	if (doc.HasMember("CompleteText"))
	{
		completeText = doc["CompleteText"].GetString();
	}
	if (doc.HasMember("Control"))
	{
		auto& arr = doc["Control"].GetArray();
		for (auto& obj : arr)
		{
			pair<int, string> p;
			if (obj.HasMember("Id"))
			{
				p.first = obj["Id"].GetInt();
			}
			if (obj.HasMember("Value"))
			{
				p.second = obj["Value"].GetString();
			}
			controlValues.push_back(p);
		}
	}
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
