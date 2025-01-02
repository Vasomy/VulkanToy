#pragma once
#include<numbers>
#include<numeric>
#include<string>
#include<vector>
#include"glm/glm.hpp"
#include<iostream>
#include<fstream>
class Debug
{
public:

	static void c();

	template<typename vec_t>
	static void CoutToFile(const char* filename, std::vector<vec_t>vec)
	{
		std::ofstream of(filename,std::ios::out|std::ios::trunc);
		if (of.is_open())
		{
			std::cout << "open file " + (std::string)filename + "successful!\n";
			for (vec_t& val : vec)
				of << val<<' ';
			of << '\n';

		}
		else
		{
			std::cout << "failed to open file " + (std::string)filename << '\n';
		}
		of.close();
	}

	template<typename T,typename... Args>
	static void LogPrint(const T& first,const Args&...args);

};


