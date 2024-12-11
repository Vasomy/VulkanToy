#include<numeric>
#include<atomic>
inline static uint32_t GetId()
{
	return Tools::UIdAllocator::GetInstance().GetId();
}
namespace Tools
{
	class UIdAllocator
	{
	public:
		static UIdAllocator& GetInstance()
		{
			return *this;
		}
		uint32_t GetUId()
		{
			uint32_t ret = mNextId;
			mNextId++;
			return ret;
		}
	private:
		std::atomic<uint32_t> mNextId = 0;

	};
}