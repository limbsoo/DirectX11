#pragma once

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>

namespace StringUtils
{
	void ExctractFolder(const std::string* filePath, std::string* pOutFolder)
	{
		size_t pos = filePath->find_last_of("/\\");
		if (pos != std::string::npos)
		{
			(*pOutFolder) = filePath->substr(0,pos).append("\\");;
		}
		else
		{
			pOutFolder->clear();
		}
	}
}

#endif