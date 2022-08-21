#pragma once
#include <JuceHeader.h>
#include "libJModule/ModuleInfo.h"

//模块信息列表，用于保存模块目录中存在的模块信息

class ModuleList final
{
public:
	ModuleList();
	~ModuleList();

	void refresh(const juce::String& moduleDir, const juce::String& product);
	void clear();
	bool exists(const juce::String& moduleId);
	const jmadf::ModuleInfo* find(const juce::String& moduleId);
	const juce::StringArray getList();
	const juce::StringArray getListByGroup(const juce::String& groupName);
	
private:
	juce::HashMap<juce::String, jmadf::ModuleInfo> moduleList;
	juce::ReadWriteLock listLock;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModuleList)
};

