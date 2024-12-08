#include"Tools.h"
#include <direct.h>
#include <Windows.h>   
namespace tools
{
	std::string ReadBinFile(const std::string& file_name)
	{
		std::cout<<getcwd(NULL,0)<<'\n';
	std::ifstream file(file_name,std::ios::binary | std::ios::ate);

	if (!file.is_open())
	{
		CreateDirectory(file_name.data(),NULL);
		std::cout << "Failed to read " << file_name << '\n';
		return std::string();
	}
	auto size = file.tellg();
	std::string content;
	content.resize(size);
	file.seekg(0);
	file.read(content.data(), content.size());

	return content;

	}
}