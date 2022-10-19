#include "JMADF.h"

std::unique_ptr<JMADF> JMADF::_jmadf;

JMADF::JMADF()
{
	this->_moduleList = std::make_unique<ModuleList>();
	this->_modulePool = std::make_unique<ModulePool>();
	this->_staticInterface = std::make_unique<jmadf::StaticInterface>();
}

void JMADF::init(const juce::String& moduleDir, const juce::String& product)
{
	if (JMADF::_jmadf.get()) {
		return;
	}
	JMADF::_jmadf = std::make_unique<JMADF>();
	JMADF::_jmadf->moduleDir = moduleDir;
	JMADF::_jmadf->product = product;
	JMADF::_jmadf->_staticInterface->loadFunc = &JMADF::loadInside;
	JMADF::_jmadf->_staticInterface->unloadFunc = &JMADF::unloadInside;
	JMADF::_jmadf->_staticInterface->isExistsFunc = &JMADF::isExists;
	JMADF::_jmadf->_staticInterface->isLoadedFunc = &JMADF::isLoaded;
	JMADF::_jmadf->_staticInterface->raiseExceptionFunc = &JMADF::raiseException;
	JMADF::_jmadf->_staticInterface->getExceptionFunc = &JMADF::getException;
	JMADF::_jmadf->_staticInterface->clearExceptionFunc = &JMADF::clearException;
	JMADF::_jmadf->_staticInterface->getInterfaceFunc = &JMADF::getInterface;
	JMADF::_jmadf->_staticInterface->getAllFunc = &JMADF::getAll;
	JMADF::_jmadf->_staticInterface->getAllInGroupFunc = &JMADF::getAllInGroup;
	JMADF::_jmadf->_staticInterface->findFunc = &JMADF::find;
	JMADF::_jmadf->_staticInterface->setLoadHookFunc = &JMADF::setLoadHook;
	JMADF::_jmadf->_staticInterface->setUnloadHookFunc = &JMADF::setUnloadHook;
	JMADF::_jmadf->_staticInterface->setLoadCallbackFunc = &JMADF::setLoadCallback;
	JMADF::_jmadf->_staticInterface->setUnloadCallbackFunc = &JMADF::setUnloadCallback;
	JMADF::refreshModule();
}

void JMADF::destroy()
{
	JMADF::_jmadf->llListLock.enter();
	while (!JMADF::_jmadf->LLList.empty()) {
		JMADF::unload(JMADF::_jmadf->LLList.top());
		JMADF::_jmadf->LLList.pop();
	}
	JMADF::_jmadf->llListLock.exit();
	JMADF::_jmadf = nullptr;
}

void JMADF::refreshModule()
{
	if (!JMADF::_jmadf->_modulePool->canRefresh()) {
		return;
	}
	JMADF::_jmadf->_moduleList->refresh(JMADF::_jmadf->moduleDir, JMADF::_jmadf->product);
}

bool JMADF::load(const juce::String& moduleId)
{
	if (moduleId.isEmpty())
	{
		JMADF::raiseException("Module id can't be empty!");
		return false;
	}
	const jmadf::ModuleInfo* info = JMADF::_jmadf->_moduleList->find(moduleId);
	if (!info) {
		JMADF::raiseException("Module " + moduleId + " isn't exists!");
		return false;
	}
	return JMADF::_jmadf->_modulePool->load(juce::String(), info, JMADF::_jmadf->_staticInterface.get());
}

bool JMADF::loadInside(const juce::String& loader, const juce::String& moduleId)
{
	if (loader.isEmpty())
	{
		jassertfalse;
		JMADF::raiseException("Loader id can't be empty!");
		return false;
	}
	if (moduleId.isEmpty())
	{
		JMADF::raiseException("Module id can't be empty!");
		return false;
	}
	const jmadf::ModuleInfo* info = JMADF::_jmadf->_moduleList->find(moduleId);
	if (!info) {
		JMADF::raiseException("Module " + moduleId + " isn't exists!");
		return false;
	}
	return JMADF::_jmadf->_modulePool->load(loader, info, JMADF::_jmadf->_staticInterface.get());
}

void JMADF::unload(const juce::String& moduleId)
{
	if (moduleId.isEmpty())
	{
		return;
	}
	JMADF::_jmadf->_modulePool->unload(juce::String(), moduleId);
}

void JMADF::unloadInside(const juce::String& loader, const juce::String& moduleId)
{
	if (loader.isEmpty())
	{
		jassertfalse;
		return;
	}
	if (moduleId.isEmpty())
	{
		return;
	}
	JMADF::_jmadf->_modulePool->unload(loader, moduleId);
}

void JMADF::setLoadHook(const juce::String& moduleId, const jmadf::HookFunction& hook)
{
	JMADF::_jmadf->_modulePool->setLoadHook(moduleId, hook);
}

void JMADF::setUnloadHook(const juce::String& moduleId, const jmadf::HookFunction& hook)
{
	JMADF::_jmadf->_modulePool->setUnloadHook(moduleId, hook);
}

void JMADF::setLoadCallback(const juce::String& moduleId, const jmadf::HookFunction& hook)
{
	JMADF::_jmadf->_modulePool->setLoadCallback(moduleId, hook);
}

void JMADF::setUnloadCallback(const juce::String& moduleId, const jmadf::HookFunction& hook)
{
	JMADF::_jmadf->_modulePool->setUnloadCallback(moduleId, hook);
}

bool JMADF::isLoaded(const juce::String& moduleId)
{
	if (moduleId.isEmpty())
	{
		return false;
	}
	return JMADF::_jmadf->_modulePool->isLoaded(moduleId);
}

bool JMADF::isExists(const juce::String& moduleId)
{
	if (moduleId.isEmpty())
	{
		return false;
	}
	return JMADF::_jmadf->_moduleList->exists(moduleId);
}

const juce::StringArray JMADF::getAll()
{
	return JMADF::_jmadf->_moduleList->getList();
}

const juce::StringArray JMADF::getAllInGroup(const juce::String& groupName)
{
	if (groupName.isEmpty())
	{
		return juce::StringArray();
	}
	return JMADF::_jmadf->_moduleList->getListByGroup(groupName);
}

const jmadf::ModuleInfo* JMADF::find(const juce::String& moduleId)
{
	if (moduleId.isEmpty())
	{
		return nullptr;
	}
	return JMADF::_jmadf->_moduleList->find(moduleId);
}

void JMADF::raiseException(const juce::String& exception)
{
	JMADF::_jmadf->_exceptionLock.enterWrite();
	JMADF::_jmadf->_exceptions.add(exception);
	JMADF::_jmadf->_exceptionLock.exitWrite();
}

const juce::String JMADF::getException()
{
	JMADF::_jmadf->_exceptionLock.enterRead();
	const juce::String& str = JMADF::_jmadf->_exceptions.joinIntoString("\n");
	JMADF::_jmadf->_exceptionLock.exitRead();
	return str;
}

void JMADF::clearException()
{
	JMADF::_jmadf->_exceptionLock.enterWrite();
	JMADF::_jmadf->_exceptions.clearQuick();
	JMADF::_jmadf->_exceptionLock.exitWrite();
}

jmadf::JInterface* JMADF::getInterface(const juce::String& moduleId)
{
	if (moduleId.isEmpty())
	{
		return nullptr;
	}
	return JMADF::_jmadf->_modulePool->getInterface(moduleId);
}

bool JMADF::loadFromLoader(const juce::String& moduleId)
{
	juce::GenericScopedLock<juce::SpinLock> locker(JMADF::_jmadf->llListLock);
	if (!JMADF::load(moduleId)) {
		return false;
	}
	JMADF::_jmadf->LLList.push(moduleId);
	return true;
}

void JMADF::unloadFromLoader(const juce::String& moduleId)
{
	juce::GenericScopedLock<juce::SpinLock> locker(JMADF::_jmadf->llListLock);
	if (JMADF::_jmadf->LLList.top() == moduleId) {
		JMADF::unload(moduleId);
		JMADF::_jmadf->LLList.pop();
	}
}
