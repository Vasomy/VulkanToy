#include"LogInfo.hpp"
#include<iostream>
void Debug::Log(int8_t content)
{
	LogPrint(content);
}

void Debug::Log(int16_t content)
{
	LogPrint(content);
}

void Debug::Log(int32_t content)
{
	LogPrint(content);
}

void Debug::Log(int64_t content)
{
	LogPrint(content);
}

void Debug::Log(float content)
{
	LogPrint(content);
}

void Debug::Log(double content)
{
	LogPrint(content);
}

void Debug::Log(uint8_t content)
{
	LogPrint(content);
}

void Debug::Log(uint16_t content)
{
	LogPrint(content);
}

void Debug::Log(uint32_t content)
{
	LogPrint(content);
}

void Debug::Log(const char* content)
{
	LogPrint(content);
}


void Debug::Log(glm::vec2 vec)
{
	LogPrint(vec[0],vec[1]);
}

void Debug::Log(glm::vec3 vec)
{
	LogPrint(vec[0], vec[1], vec[2]);
}

void Debug::Log(glm::vec4 vec)
{
	LogPrint(vec[0], vec[1], vec[2],vec[3]);
}

void Debug::Log(glm::mat4 mat)
{
	for (int i = 0; i < 4; ++i)
	{
		Log(glm::vec4{ mat[i][0],mat[i][1], mat[i][2], mat[i][3] });
	}
}

void Debug::Log(const unsigned char* content)
{
	LogPrint(content);
}

template<typename T, typename... Args>
inline void Debug::LogPrint(const T& first, const Args&...args)
{
	std::cout << first;
	if constexpr (sizeof...(args) > 0)
	{
		std::cout << ' ';
		LogPrint(args...);
	}
	else
	{
		std::cout << std::endl;
	}
}