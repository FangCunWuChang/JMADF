#pragma once
#include <JuceHeader.h>
#include "CallBackManager.h"

//ģ�飬���ڱ����Ѽ��ص�ģ����

class JModule final
{
public:
	JModule();
	~JModule();
	
private:
	std::unique_ptr<juce::DynamicLibrary> library;
	std::unique_ptr<CallBackManager> messages;

	const juce::String id;
	
	JUCE_LEAK_DETECTOR(JModule)
};

