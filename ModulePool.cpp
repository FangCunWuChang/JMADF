#include "ModulePool.h"
#include "JMADF.h"

ModulePool::ModulePool()
{
	
}

ModulePool::~ModulePool()
{
	this->closeAll();
}

bool ModulePool::load(const jmadf::ModuleInfo* info, const jmadf::StaticInterface* staticInterface)
{
	if (!info) {
		JMADF::raiseException("nullptr!");
		return false;
	}
	
	this->listLock.enterWrite();
	if (this->moduleList.contains(info->id)) {
		this->listLock.exitWrite();
		return true;
	}
	
	JModule* mod = new(std::nothrow) JModule;
	if (mod == nullptr) {
		JMADF::raiseException("Can't alloc memory!");
		this->listLock.exitWrite();
		return false;
	}
	
	bool result = mod->init(info, staticInterface);
	
	if (result) {
		this->moduleList.set(info->id, mod);
	}
	else {
		JMADF::raiseException("Can't init module:" + info->id);
		mod->destory();
		delete mod;
	}
	
	this->listLock.exitWrite();
	
	return result;
}

void ModulePool::unload(const juce::String& moduleId)
{
	this->listLock.enterWrite();
	if (!this->moduleList.contains(moduleId)) {
		this->listLock.exitWrite();
		return;
	}
	
	JModule* mod = this->moduleList[moduleId];
	this->moduleList.remove(moduleId);
	
	mod->destory();
	delete mod;
	
	this->listLock.exitWrite();
}

bool ModulePool::isLoaded(const juce::String& moduleId)
{
	this->listLock.enterRead();
	bool result = this->moduleList.contains(moduleId);
	this->listLock.exitRead();
	return result;
}

void ModulePool::closeAll()
{
	this->listLock.enterWrite();
	for (auto m : this->moduleList) {
		m->destory();
		delete m;
	}
	this->moduleList.clear();
	this->listLock.exitWrite();
}