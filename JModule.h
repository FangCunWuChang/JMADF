#pragma once
#include <JuceHeader.h>
#include "libJModule/ModuleInfo.h"
#include "libJModule/JModuleBase.h"
#include "libJModule/StaticInterface.h"
#include "libJModule/JInterface.h"

//ģ�飬���ڱ����Ѽ��ص�ģ����
class JModule final
{
public:
	JModule() = default;
	~JModule() = default;
	
	bool init(const jmadf::ModuleInfo* info, const jmadf::StaticInterface* staticInterface);
	void destory();
	
public:
	jmadf::JInterface* getInterface();

private:
	std::unique_ptr<juce::DynamicLibrary> library;
	std::unique_ptr<jmadf::JModuleBase> moduleClass;
	
	std::function<jmadf::JModuleBase* (void)> createInstanceFunc;
	std::function<void(const jmadf::JModuleBase*)> destoryInstanceFunc;
	const jmadf::ModuleInfo* info = nullptr;

	jmadf::JInterface* interfaces = nullptr;

	friend class ModulePool;
	uint64_t count = 0;
	
	JUCE_LEAK_DETECTOR(JModule)
};

