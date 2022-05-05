#pragma once
#include <JuceHeader.h>
#include "ModuleInfo.h"

//ģ����Ϣ�б����ڱ���ģ��Ŀ¼�д��ڵ�ģ����Ϣ

class ModuleList final
{
public:
	ModuleList();
	~ModuleList();

	void refresh(const juce::String& moduleDir, const juce::String& productId);
	
private:
	juce::HashMap<juce::String, ModuleInfo> moduleList;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModuleList)
};

