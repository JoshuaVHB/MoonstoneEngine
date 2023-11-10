#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <DirectXMath.h>

struct FormatJson {
	std::string nameObj;
	std::string pathObj;
	DirectX::XMVECTOR positionObj;
	DirectX::XMVECTOR forwardObj;
	DirectX::XMVECTOR scaleObj;
};

class JsonParser
{
private:
	std::string m_fileName;
	nlohmann::json m_json;

	std::ifstream m_file;

	std::vector<FormatJson>objs;


public:
	JsonParser(std::string fileName) : m_fileName(fileName) {};

	void openFile();

	void updateList();

	void getCheckpoints(std::vector<FormatJson> & chepointInfos);

	[[nodiscard]] std::vector<FormatJson> getObjs();

};