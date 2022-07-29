#include "ModulePool.h"
#include "JMADF.h"

ModulePool::ModulePool()
{
	
}

ModulePool::~ModulePool()
{
	
}

bool ModulePool::load(const juce::String& loader, const jmadf::ModuleInfo* info, const jmadf::StaticInterface* staticInterface)
{
	if (!info) {
		JMADF::raiseException("nullptr!");
		return false;
	}
	
	juce::ScopedWriteLock locker(this->listLock);
	if (this->moduleList.contains(info->id)) {
		if (!loader.isEmpty()) {
			auto* loaderPtr = this->moduleList[loader];
			juce::GenericScopedLock<juce::SpinLock> locker(loaderPtr->llListLock);
			loaderPtr->LLList.push_front(info->id);
		}
		this->moduleList[info->id]->count++;
		return true;
	}
	
	JModule* mod = new(std::nothrow) JModule;
	if (mod == nullptr) {
		JMADF::raiseException("Can't alloc memory!");
		return false;
	}
	
	this->moduleList.set(info->id, mod);
	
	if (!mod->init(info, staticInterface)) {
		JMADF::raiseException("Can't init module:" + info->id);
		this->moduleList.remove(info->id);
		mod->destory();
		delete mod;
		return false;
	}
	
	if (!loader.isEmpty()) {
		auto* loaderPtr = this->moduleList[loader];
		juce::GenericScopedLock<juce::SpinLock> locker(loaderPtr->llListLock);
		loaderPtr->LLList.push_front(info->id);
	}
	
	return true;
}

void ModulePool::unload(const juce::String& loader, const juce::String& moduleId)
{
	juce::ScopedWriteLock locker(this->listLock);
	if (!this->moduleList.contains(moduleId)) {
		return;
	}
	
	JModule* mod = this->moduleList[moduleId];
	if (!mod->count) {
		this->moduleList.remove(moduleId);
		mod->destory();
		delete mod;
	}
	else {
		mod->count--;
	}

	if (!loader.isEmpty()) {
		auto* loaderPtr = this->moduleList[loader];
		juce::GenericScopedLock<juce::SpinLock> locker(
			loaderPtr->llListLock);
		for (auto it = loaderPtr->LLList.begin(); it != loaderPtr->LLList.end(); it++) {
			if (*it == moduleId) {
				loaderPtr->LLList.erase(it);
			}
		}
	}
}

bool ModulePool::isLoaded(const juce::String& moduleId)
{
	this->listLock.enterRead();
	bool result = this->moduleList.contains(moduleId);
	this->listLock.exitRead();
	return result;
}

bool ModulePool::canRefresh()
{
	return !this->moduleList.size();
}

jmadf::JInterface* ModulePool::getInterface(const juce::String& moduleId)
{
	if (!this->moduleList.contains(moduleId)) {
		return nullptr;
	}
	return this->moduleList[moduleId]->getInterface();
}
