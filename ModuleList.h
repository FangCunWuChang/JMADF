#pragma once
#include <JuceHeader.h>
#include "ModuleInfo.h"

//模块信息列表，用于保存模块目录中存在的模块信息

class ModuleList final
{
public:
	ModuleList();
	~ModuleList();

	void refresh(const juce::String& moduleDir, const juce::String& product);
	void clear();
	bool exists(const juce::String& moduleId);
	const ModuleInfo* find(const juce::String& moduleId);
	const juce::StringArray getList();
	
private:
	juce::HashMap<juce::String, ModuleInfo> moduleList;
	juce::ReadWriteLock listLock;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModuleList)
};

