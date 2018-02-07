#pragma once
#include <string>
#include <Windows.h>
#include <iostream>

using std::string;
using std::cout;

class Parse {
public:
	Parse(string fname);

	int GetInt(string section, string key, int def);
	void SetInt(string section, string key, int def, int & out);

	string GetStr(string section, string key, string def);
	void SetStr(string section, string key, string def, string & out);
private:
	LPCSTR filename;
	bool FileExists();
};