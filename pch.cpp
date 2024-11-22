// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일

#include "pch.h"

std::wstring MultibyteToUnicode(const char* str)
{
	if (str == NULL)
		return std::wstring();

	const int size = strlen(str);
	if (size <= 0)
		return std::wstring();

	int strSize = MultiByteToWideChar(CP_ACP, 0, str, size, NULL, 0);
	std::wstring wStr(strSize, 0);
	strSize = MultiByteToWideChar(CP_ACP, 0, str, size, &wStr[0], size);

	return wStr;
}
