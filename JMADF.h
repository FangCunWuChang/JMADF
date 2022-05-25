#pragma once
#include <JuceHeader.h>
#include "ModuleList.h"
#include "ModulePool.h"
#include "libJModule/StaticInterface.h"
#include "libJModule/JInterface.h"

//Juce模块化应用开发框架

class JMADF final
{
	static std::unique_ptr<JMADF> _jmadf;
public:
	JMADF();
	~JMADF() = default;
	
public:
	static void init(const juce::String& moduleDir, const juce::String& product);
	static void destroy();

	static void refreshModule();
	
	static bool load(const juce::String& moduleId);
	static void unload(const juce::String& moduleId);

	static bool isLoaded(const juce::String& moduleId);
	static bool isExists(const juce::String& moduleId);
	
	static void raiseException(const juce::String& exception);
	static const juce::String getException();
	static void clearException();

public:
	template<typename ...T>
	static void callInterfaceFromLoader(
		const juce::String& moduleId, T... args
	)
	{
		jmadf::JInterface* pInterface = JMADF::getInterface(moduleId);
		if (!pInterface) {
			return;
		}
		pInterface->call<T...>("", moduleId, args...);
	};

private:
	static jmadf::JInterface* getInterface(const juce::String& moduleId);
	
private:
	std::unique_ptr<ModuleList> _moduleList;
	std::unique_ptr<ModulePool> _modulePool;
	std::unique_ptr<jmadf::StaticInterface> _staticInterface;
	juce::StringArray _exceptions;
	juce::ReadWriteLock _exceptionLock;

	juce::String moduleDir;
	juce::String product;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JMADF)
};