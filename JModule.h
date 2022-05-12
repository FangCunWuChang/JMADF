#pragma once
#include <JuceHeader.h>
#include "ModuleInfo.h"
#include "libJModule/JModuleBase.h"

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
	std::unique_ptr<JModuleBase> moduleClass;
	
	std::function<JModuleBase* (void)> createInstanceFunc;
	std::function<void(JModuleBase*)> destoryInstanceFunc;
	const ModuleInfo* info = nullptr;
	
	JUCE_LEAK_DETECTOR(JModule)
};

