#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using std::vector;

template<class T>
std::string vectorStr(const std::vector<T> vec)
{
	std::stringstream buf;
	buf << "[";
	if (!vec.empty())
	{
		buf << vec[0];
		for (auto it = vec.begin() + 1; it != vec.end(); ++it)
		{
			buf << ", " << *it;
		}
	}
	buf << "]";
	return buf.str();
}

void initLesson()
{
	printf("Lesson: %s\n\n", LESSON_NAME);
}