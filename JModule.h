#pragma once
#include <JuceHeader.h>
#include "CallBackManager.h"

//模块，用于保存已加载的模块句柄

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

