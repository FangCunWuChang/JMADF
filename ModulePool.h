#pragma once
#include <JuceHeader.h>
#include "JModule.h"
#include "libJModule/ModuleInfo.h"
#include "libJModule/StaticInterface.h"
#include "libJModule/JInterface.h"

//模块池，用于储存各加载后的模块

class ModulePool final
{
public:
	ModulePool();
	~ModulePool();

	bool load(const juce::String& loader, const jmadf::ModuleInfo* info, const jmadf::StaticInterface* staticInterface);
	void unload(const juce::String& loader, const juce::String& moduleId);
	bool isLoaded(const juce::String& moduleId);

	void setLoadHook(const juce::String& moduleId, const jmadf::HookFunction& hook);
	void setUnloadHook(const juce::String& moduleId, const jmadf::HookFunction& hook);
	void setLoadCallback(const juce::String& moduleId, const jmadf::HookFunction& hook);
	void setUnloadCallback(const juce::String& moduleId, const jmadf::HookFunction& hook);

	bool canRefresh();

public:
	jmadf::JInterface* getInterface(const juce::String& moduleId);
	
private:
	std::map<juce::String, JModule*> moduleList;
	std::map<juce::String, jmadf::HookFunction> loadHookList, unloadHookList, loadCallbackList, unloadCallbackList;
	juce::ReadWriteLock listLock, hookLock;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulePool)
};

