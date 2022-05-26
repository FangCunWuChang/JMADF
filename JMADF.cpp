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
	JMADF::_jmadf->_staticInterface->loadFunc = &JMADF::load;
	JMADF::_jmadf->_staticInterface->unloadFunc = &JMADF::unload;
	JMADF::_jmadf->_staticInterface->isExistsFunc = &JMADF::isExists;
	JMADF::_jmadf->_staticInterface->isLoadedFunc = &JMADF::isLoaded;
	JMADF::_jmadf->_staticInterface->raiseExceptionFunc = &JMADF::raiseException;
	JMADF::_jmadf->_staticInterface->getExceptionFunc = &JMADF::getException;
	JMADF::_jmadf->_staticInterface->clearExceptionFunc = &JMADF::clearException;
	JMADF::_jmadf->_staticInterface->getInterfaceFunc = &JMADF::getInterface;
	JMADF::_jmadf->_staticInterface->getAllFunc = &JMADF::getAll;
	JMADF::_jmadf->_staticInterface->getAllInGroupFunc = &JMADF::getAllInGroup;
	JMADF::_jmadf->_staticInterface->findFunc = &JMADF::find;
	JMADF::refreshModule();
}

void JMADF::destroy()
{
	JMADF::_jmadf->_modulePool->closeAll();
	JMADF::_jmadf = nullptr;
}

void JMADF::refreshModule()
{
	JMADF::_jmadf->_modulePool->closeAll();
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
	return JMADF::_jmadf->_modulePool->load(info, JMADF::_jmadf->_staticInterface.get());
}

void JMADF::unload(const juce::String& moduleId)
{
	if (moduleId.isEmpty())
	{
		return;
	}
	JMADF::_jmadf->_modulePool->unload(moduleId);
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
