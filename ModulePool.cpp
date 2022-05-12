#include "ModulePool.h"

ModulePool::ModulePool()
{
	
}

ModulePool::~ModulePool()
{
	this->closeAll();
}

bool ModulePool::load(const ModuleInfo* info)
{
	if (!info) {
		return false;
	}
	
	this->listLock.enterWrite();
	if (this->moduleList.contains(info->id)) {
		this->listLock.exitWrite();
		return true;
	}
	
	JModule* mod = new(std::nothrow) JModule;
	if (mod == nullptr) {
		this->listLock.exitWrite();
		return false;
	}
	
	bool result = mod->init(info);
	
	if (result) {
		this->moduleList.set(info->id, mod);
	}
	else {
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