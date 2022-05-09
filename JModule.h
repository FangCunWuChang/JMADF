#pragma once
#include <JuceHeader.h>
#include "ModuleInfo.h"

//模块，用于保存已加载的模块句柄

class JModule final
{
public:
	JModule() = default;
	~JModule() = default;
	
	bool init(const ModuleInfo* info);
	void destory();
	
private:
	std::unique_ptr<juce::DynamicLibrary> library;
	const ModuleInfo* info = nullptr;
	
	JUCE_LEAK_DETECTOR(JModule)
};

