#pragma once
#include <JuceHeader.h>
#include "JModule.h"
#include "libJModule/ModuleInfo.h"
#include "libJModule/StaticInterface.h"

//模块池，用于储存各加载后的模块

class ModulePool final
{
public:
	ModulePool();
	~ModulePool();

	bool load(const jmadf::ModuleInfo* info, const jmadf::StaticInterface* staticInterface);
	void unload(const juce::String& moduleId);
	bool isLoaded(const juce::String& moduleId);
	void closeAll();
	
private:
	juce::HashMap<juce::String, JModule*> moduleList;
	juce::ReadWriteLock listLock;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulePool)
};

