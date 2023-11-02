#include "JsonParser.h"


void JsonParser::openFile()
{
	m_file.open(m_fileName);
	m_file >> m_json;
}

void JsonParser::updateList()
{
	if (m_json.is_array())
	{
		std::for_each(m_json.begin(), m_json.end(), [&](auto& item) {
			FormatJson obj;
			obj.nameObj = item["j_Objet"];
			obj.pathObj = item["j_path"];
			obj.positionObj = DirectX::XMVectorSet(item["j_position"]["x"], item["j_position"]["y"], item["j_position"]["z"], item["j_position"]["n"]);
			obj.forwardObj = DirectX::XMVectorSet(item["j_forward"]["x"], item["j_forward"]["y"], item["j_forward"]["z"], item["j_forward"]["n"]);
			obj.scaleObj = DirectX::XMVectorSet(item["j_scale"]["x"], item["j_scale"]["y"], item["j_scale"]["z"], item["j_scale"]["n"]);
			objs.push_back(obj);
			});

	}
	else
		std::cout << "Error: Json file is not an array" << std::endl;

	m_file.close();
}

std::vector<FormatJson> JsonParser::getObjs()
{
	openFile();

	updateList();

	return objs;
}
