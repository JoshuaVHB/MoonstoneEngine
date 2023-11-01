#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <algorithm>

struct Format {
	std::string nameObj;
	std::string pathObj;

};

class JsonParser
{
private:
	std::string m_fileName;
	nlohmann::json m_json;

	std::ifstream m_file;

	std::vector<Format>objs;
	

public:
	JsonParser(std::string fileName) {
		m_fileName = fileName;
	}

	void loadFile() {
		m_file.open(m_fileName);
		m_file >> m_json;
		m_file.close();
	}

	void updateList() {

	}

};