#include "JMADF.h"

std::unique_ptr<JMADF> JMADF::_jmadf;

JMADF::JMADF()
{
	this->_moduleList = std::make_unique<ModuleList>();
	this->_modulePool = std::make_unique<ModulePool>();
}

void JMADF::init(const juce::String& moduleDir, const juce::String& product)
{
	if (JMADF::_jmadf.get()) {
		return;
	}
	JMADF::_jmadf = std::make_unique<JMADF>();
	JMADF::_jmadf->moduleDir = moduleDir;
	JMADF::_jmadf->product = product;
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
	
}

void JMADF::unload(const juce::String& moduleId)
{
	
}

bool JMADF::isLoaded(const juce::String& moduleId)
{
	
}

bool JMADF::isExists(const juce::String& moduleId)
{
	return JMADF::_jmadf->_moduleList->find(moduleId);
}
