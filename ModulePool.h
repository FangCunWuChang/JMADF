#pragma once
#include <JuceHeader.h>

//模块池，用于储存各加载后的模块

class ModulePool final
{
public:
	ModulePool();
	~ModulePool();
	
private:
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulePool)
};

