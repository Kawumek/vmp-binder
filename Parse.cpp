#include "Parse.h"

Parse::Parse(string fname)
{
	filename = fname.c_str();
	if (!FileExists()) {
		cout << "Не найдён файл конфигурации!";
		system("pause");
		exit(0x01);
	}
}

int Parse::GetInt(string section, string key, int def)
{
	return GetPrivateProfileInt(section.c_str(), key.c_str(), def, filename);
}

void Parse::SetInt(string section, string key, int def, int & out)
{
	out = GetPrivateProfileInt(section.c_str(), key.c_str(), def, filename);
}

string Parse::GetStr(string section, string key, string def)
{
	char temp[1024];
	int result = GetPrivateProfileStringA(section.c_str(), key.c_str(), def.c_str(), temp, sizeof(temp), filename);
	return temp;
}

void Parse::SetStr(string section, string key, string def, string & out)
{
	char temp[1024];
	int result = GetPrivateProfileStringA(section.c_str(), key.c_str(), def.c_str(), temp, sizeof(temp), filename);
	out = temp;
}

bool Parse::FileExists()
{
	return ::GetFileAttributesA(filename) != DWORD(-1);;
}

