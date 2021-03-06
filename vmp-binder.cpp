#include <Windows.h>
#include <iostream>
#include <string>
#include <locale.h>
#include <algorithm>

using namespace std;

unsigned int delaybef;
unsigned int delayaft;

string Fmsg[12];
string Nmsg[9];
string replacekey;

DWORD CodePage = 0x419;

bool debug = false;

void Close(string conclusion) {
	if (conclusion != "")
		cout << conclusion << endl;
	cout << "Закрываюсь...\n";
	system("pause");
	exit(0);
}

bool StringContains(string original, string key) {
	size_t pos = original.find(key);
	if (pos != std::string::npos)
		return true;
	else
		return false;
}

string ReplaceAll(string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	return str;
}

string GetString(string section, string key, string def)
{
	char temp[1024];
	int result = GetPrivateProfileStringA(section.c_str(), key.c_str(), def.c_str(), temp, sizeof(temp), ".//vmp-binder.ini");
	return temp;
}

int GetInteger(string section, string key, int def) {
	return GetPrivateProfileInt(section.c_str(), key.c_str(), def, ".//vmp-binder.ini");
}

bool FileExists(LPCSTR fname) {
	return ::GetFileAttributesA(fname) != DWORD(-1);;
}

bool ParseSettings() {
	cout << "\n---\n";
	if (debug) cout << "Starting ParseSettings function...\n";
	if (!FileExists("vmp-binder.ini")) {
		return false;
	}
	if (debug) cout << "Settings file found!\n\n";

	delaybef = GetInteger("main", "delaybefore", 50);
	cout << "Задержка перед каждой проверкой на нажатие: " << delaybef << "мс\n";
	if (delaybef < 5) cout << "Не рекомендуется ставить значение задержки менее 5мс\n";

	delayaft = GetInteger("main", "delayafter", 500);
	cout << "Задержка после использования бинда: " << delayaft << "мс\n";

	replacekey = GetString("main", "replacekey", "&");
	if (replacekey == "" || replacekey == " ") replacekey = "&";
	cout << "Символ замены пробела в сообщении: " << replacekey << endl;

	cout << "\nБинды для F-кнопок:\n";
	for (int i = 1; i <= 12; i++) {
		Fmsg[i - 1] = GetString("F keys", "F" + to_string(i), "");
		Fmsg[i - 1] = ReplaceAll(Fmsg[i - 1], replacekey, " ");
		cout << "F" << i << ": \"" << Fmsg[i - 1] << "\"" << endl;
	}

	cout << "\nБинды для numpad:\n";
	for (int i = 1; i <= 9; i++) {
		Nmsg[i - 1] = GetString("N keys", "N" + to_string(i), "");
		Nmsg[i - 1] = ReplaceAll(Nmsg[i - 1], replacekey, " ");
		cout << "N" << i << ": \"" << Nmsg[i - 1] << "\"" << endl;
	}
	if (debug) cout << "Stopping ParseSettings function...\n";
	cout << "---\n\n";
	return true;
}

void PressKey(int key1, int key2) {
	if (debug) cout << "\nStarting PressKey function...\n";
	if (key2 != NULL) {
		if (debug) cout << "0x" << hex << key2 << " != NULL\n";
		keybd_event(key1, 0, 0, 0);
		if (debug) cout << "0x" << hex << key1 << " pressed!\n";
		keybd_event(key2, 0, 0, 0);
		if (debug) cout << "0x" << hex << key2 << " pressed!\n";
		Sleep(100);
		keybd_event(key2, 0, 0x02, 0);
		if (debug) cout << "0x" << hex << key2 << " released!\n";
		keybd_event(key1, 0, 0x02, 0);
		if (debug) cout << "0x" << hex << key1 << " released!\n";
	}
	else {
		if (debug) cout << "0x" << hex << key2 << " = NULL\n";
		keybd_event(key1, 0, 0, 0);
		if (debug) cout << "0x" << hex << key1 << " pressed!\n";
		Sleep(100);
		keybd_event(key1, 0, 0x02, 0);
		if (debug) cout << "0x" << hex << key1 << " released!\n";
	}
	if (debug) cout << "Stopping PressKey function...\n";
}

bool SetClipboard(string str)
{
	if (debug) cout << "\nStarting SetClipBoard function...\n";
	if (OpenClipboard(GetDesktopWindow()))
	{
		if (debug)	cout << "Succesfully opened clipboard!\n";
		HGLOBAL hgBuffer = GlobalAlloc(GMEM_DDESHARE, str.size() + 1);
		char* chBuffer = (char*)GlobalLock(hgBuffer);
		strcpy(chBuffer, str.c_str());
		EmptyClipboard();
		if (debug) cout << "Clipboard cleared!\n";
		HGLOBAL Data = GlobalAlloc(GMEM_MOVEABLE + GMEM_DDESHARE, 4);
		char* DataPtr = (char*)GlobalLock(Data);
		memcpy(DataPtr, (const VOID*)&CodePage, 4);
		GlobalUnlock(Data);
		SetClipboardData(CF_LOCALE, Data);
		GlobalUnlock(hgBuffer);
		SetClipboardData(CF_TEXT, hgBuffer);
		if (debug) cout << "Text on the clipboard!\n";
		CloseClipboard();
		if (debug) cout << "Succesfully closed clipboard!\n";
		if (debug) cout << "Stopping SetClipBoard function...\n";
		return true;
	}
	else {
		if (debug) cout << "Failed to open clipboard!\n";
		if (debug) cout << "Stopping SetClipBoard function...\n";
		return false;
	}
}

bool SendChatMessage(string msg) {
	if (msg != "") {
		PressKey('T', NULL);
		if (!SetClipboard(msg)) return false;
		PressKey(VK_CONTROL, 'V');
		cout << "\nОтправлено: \"" << msg << "\"\n---\n";
		return true;
	}
	else {
		if (debug) cout << "Empty string!\n";
		cout << "Пустая строка, ничего не отправлено\n---\n";
		Sleep(150);
		return false;
	}
}

bool DetectKeyPress(int firstnum, int lastnum, bool numpad) {
	for (int i = firstnum, m = 0; i <= lastnum; i++, m++) {
		if (GetAsyncKeyState(i) & 0x8000) {
			cout << "\n---\nНажата: " << "0x" << hex << i << endl;
			if (numpad) return SendChatMessage(Nmsg[m]);
			else return SendChatMessage(Fmsg[m]);
		}
	}
	return false;
}

int main(int argc, char *argv[])
{
	std::ios::sync_with_stdio(false);
	setlocale(LC_ALL, "Russian");
	cout << "Спасибо за использование vmp-binder.\nСекундочку...\n";

	if (argc > 1) {
		if (StringContains(argv[1], "debug-mode")) {
			debug = true;
			cout << "Debug started!\n";
		}
	}

	if (!ParseSettings()) Close("Failed to load settings!\n---\n");

	if (debug) cout << "Starting loop...\n";
	cout << "Так, всё готово, можете начинать работу!\n";
	cout << "Для закрытия программы вы можете использовать также комбинацию клавиш:\nRShift+Backslash\n";
	while (true) {
		if (DetectKeyPress(0x61, 0x69, true) || DetectKeyPress(0x70, 0x7B, false)) Sleep(delayaft);
		else if (GetAsyncKeyState(0xDC) & 0x8000 && GetAsyncKeyState(VK_RSHIFT) & 0x8000) {
			if (debug) cout << "\nStopped loop!\n";
			Close("");
		}
		else Sleep(delaybef);
	}
}