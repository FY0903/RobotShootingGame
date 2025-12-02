#include "Singleton.hpp"

namespace
{
	std::mutex g_Mutex;
	std::vector<SingletonFinalizer::FinalizerFunc> g_Finalizers;
}

void SingletonFinalizer::AddFinalizer(FinalizerFunc func)
{
	std::lock_guard<std::mutex> lock(g_Mutex);
	
	if (g_Finalizers.size() < g_Finalizers.max_size())
	{
		g_Finalizers.push_back(func);
	}
}

void SingletonFinalizer::Finalize()
{
	std::lock_guard<std::mutex> lock(g_Mutex);

	for (auto it = g_Finalizers.rbegin(); it != g_Finalizers.rend(); ++it)
	{
		if (*it)
		{
			(*it)();
		}
	}
}
