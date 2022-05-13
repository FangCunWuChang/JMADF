#pragma once
#include <JuceHeader.h>
#include "ModuleInfo.h"
#include "libJModule/JModuleBase.h"
#include "libJModule/StaticInterface.h"

//ģ�飬���ڱ����Ѽ��ص�ģ����

class JModule final
{
public:
	JModule() = default;
	~JModule() = default;
	
	bool init(const ModuleInfo* info, const jmadf::StaticInterface* staticInterface);
	void destory();

private:
	std::unique_ptr<juce::DynamicLibrary> library;
	std::unique_ptr<jmadf::JModuleBase> moduleClass;
	
	std::function<jmadf::JModuleBase* (void)> createInstanceFunc;
	std::function<void(const jmadf::JModuleBase*)> destoryInstanceFunc;
	const ModuleInfo* info = nullptr;
	
	JUCE_LEAK_DETECTOR(JModule)
};

