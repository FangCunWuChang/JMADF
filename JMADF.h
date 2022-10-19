#pragma once
#include <JuceHeader.h>
#include "ModuleList.h"
#include "ModulePool.h"
#include "libJModule/StaticInterface.h"
#include "libJModule/JInterface.h"
#include "libJModule/ModuleInfo.h"
#include <stack>

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
	
	static bool loadFromLoader(const juce::String& moduleId);
	static void unloadFromLoader(const juce::String& moduleId);
	
	static bool isLoaded(const juce::String& moduleId);
	static bool isExists(const juce::String& moduleId);

	static const juce::StringArray getAll();
	static const juce::StringArray getAllInGroup(const juce::String& groupName);
	
	static const jmadf::ModuleInfo* find(const juce::String& moduleId);
	
	static void raiseException(const juce::String& exception);
	static const juce::String getException();
	static void clearException();

public:
	static jmadf::JInterface* getInterface(const juce::String& moduleId);

	static bool load(const juce::String& moduleId);
	static bool loadInside(const juce::String& loader, const juce::String& moduleId);
	static void unload(const juce::String& moduleId);
	static void unloadInside(const juce::String& loader, const juce::String& moduleId);

	static void setLoadHook(const juce::String& moduleId, const jmadf::HookFunction& hook);
	static void setUnloadHook(const juce::String& moduleId, const jmadf::HookFunction& hook);
	static void setLoadCallback(const juce::String& moduleId, const jmadf::HookFunction& hook);
	static void setUnloadCallback(const juce::String& moduleId, const jmadf::HookFunction& hook);
	
private:
	std::unique_ptr<ModuleList> _moduleList;
	std::unique_ptr<ModulePool> _modulePool;
	std::unique_ptr<jmadf::StaticInterface> _staticInterface;
	juce::StringArray _exceptions;
	juce::ReadWriteLock _exceptionLock;

	juce::String moduleDir;
	juce::String product;
	
	std::stack<juce::String> LLList;
	juce::SpinLock llListLock;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JMADF)
};

template<typename ...T>
class InterfaceDao final
{
	InterfaceDao() = delete;
	~InterfaceDao() = delete;

public:
	static void callFromLoader(
		const juce::String& moduleId, const juce::String& key, T... args
	)
	{
		jmadf::JInterface* pInterface = JMADF::getInterface(moduleId);
		if (!pInterface) {
			return;
		}
		jmadf::JInterfaceDao<T...>::call(pInterface, "", key, std::forward<T>(args)...);
	};

	static const std::function<void(T...)> getFromLoader(
		const juce::String& moduleId, const juce::String& key
	)
	{
		jmadf::JInterface* pInterface = JMADF::getInterface(moduleId);
		if (!pInterface) {
			return [](T...) {};
		}
		return jmadf::JInterfaceDao<T...>::get(pInterface, "", key);
	};
	static bool checkFromLoader(
		const juce::String& moduleId, const juce::String& key
	)
	{
		jmadf::JInterface* pInterface = JMADF::getInterface(moduleId);
		if (!pInterface) {
			return false;
		}
		return jmadf::JInterfaceDao<T...>::check(pInterface, key);
	};
};

template<>
class InterfaceDao<void> final
{
	InterfaceDao() = delete;
	~InterfaceDao() = delete;

public:
	static void callFromLoader(
		const juce::String& moduleId, const juce::String& key
	)
	{
		jmadf::JInterface* pInterface = JMADF::getInterface(moduleId);
		if (!pInterface) {
			return;
		}
		jmadf::JInterfaceDao<void>::call(pInterface, "", key);
	};

	static const std::function<void(void)> getFromLoader(
		const juce::String& moduleId, const juce::String& key
	)
	{
		jmadf::JInterface* pInterface = JMADF::getInterface(moduleId);
		if (!pInterface) {
			return [] {};
		}
		return jmadf::JInterfaceDao<void>::get(pInterface, "", key);
	}
	static bool checkFromLoader(
		const juce::String& moduleId, const juce::String& key
	)
	{
		jmadf::JInterface* pInterface = JMADF::getInterface(moduleId);
		if (!pInterface) {
			return false;
		}
		return jmadf::JInterfaceDao<void>::check(pInterface, key);
	};
};
