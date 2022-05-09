#pragma once
#include <JuceHeader.h>
#include "JModule.h"
#include "ModuleInfo.h"

//ģ��أ����ڴ�������غ��ģ��

class ModulePool final
{
public:
	ModulePool();
	~ModulePool();

	bool load(const ModuleInfo* info);
	void unload(const juce::String& moduleId);
	bool isLoaded(const juce::String& moduleId);
	void closeAll();
	
private:
	juce::HashMap<juce::String, JModule*> moduleList;
	juce::ReadWriteLock listLock;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulePool)
};

