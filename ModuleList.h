#pragma once
#include <JuceHeader.h>
#include "libJModule/ModuleInfo.h"

//ģ����Ϣ�б����ڱ���ģ��Ŀ¼�д��ڵ�ģ����Ϣ

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

