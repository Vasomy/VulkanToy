#include"LogInfo.hpp"


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

void Debug::c()
{
	std::cout << "C\n";
}
