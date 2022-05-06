#pragma once
#include <JuceHeader.h>

//模块信息

class ModuleInfo final
{
public:
	ModuleInfo() = default;
	~ModuleInfo() = default;
	
	juce::String id;//"developer.moduleName"
	juce::String version;//"x.x.x"
	juce::String group;//"productName.groupName"
	juce::String productId;//"{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}"
	juce::String path;//"D://xxxxxx//module//developer.moduleName//"
	juce::String entry;//"developer.moduleName.dll"
	juce::String infoList;//"developer.moduleName.json"
	juce::StringArray dependencies;//依赖的id
	juce::String description;//描述

private:
	JUCE_LEAK_DETECTOR(ModuleInfo)
};

