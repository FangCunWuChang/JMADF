#pragma once
#include <JuceHeader.h>
#include "ModuleInfo.h"

//ģ�飬���ڱ����Ѽ��ص�ģ����

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

