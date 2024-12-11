#pragma once
#include<numbers>
#include<numeric>
#include<string>
#include"glm/glm.hpp"

class Debug
{
public:
	
	static void Log(int8_t content);
	static void Log(int16_t content);
	static void Log(int32_t content);
	static void Log(int64_t content);
	static void Log(float content);
	static void Log(double content);
	static void Log(uint8_t content);
	static void Log(uint16_t content);
	static void Log(uint32_t content);
	static void Log(const char* content);
	static void Log(const unsigned char* content);
	static void Log(glm::vec2 vec);
	static void Log(glm::vec3 vec);
	static void Log(glm::vec4 vec);
	static void Log(glm::mat4 mat);

	template<typename T,typename... Args>
	static void LogPrint(const T& first,const Args&...args);

};


