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
	//判读信息表有效
	if (!info) {
		JMADF::raiseException("nullptr!");
		return false;
	}
	
	//锁模块表
	juce::ScopedWriteLock locker1(this->listLock);

	//如果模块已加载，则增加引用计数并记录在调用者调用栈中
	if (this->moduleList.contains(info->id)) {
		//如果调用者非主模块，则记录在调用者调用栈中
		if (!loader.isEmpty()) {
			auto* loaderPtr = this->moduleList[loader];
			juce::GenericScopedLock<juce::CriticalSection> locker(loaderPtr->llListLock);
			loaderPtr->LLList.push_front(info->id);
		}
		this->moduleList[info->id]->count++;
		return true;
	}
	
	//初始化新模块
	JModule* mod = new(std::nothrow) JModule;
	if (mod == nullptr) {
		JMADF::raiseException("Can't alloc memory!");
		return false;
	}
	
	//先将新模块添加至列表
	this->moduleList.insert(std::make_pair(info->id, mod));
	
	//调用模块加载钩子
	{
		juce::ScopedReadLock locker2(this->hookLock);
		for (auto& p : this->loadHookList) {
			p.second(info->id);
		}
	}

	//如果模块初始化失败，则从列表移除并清除钩子
	if (!mod->init(info, staticInterface)) {
		//抛异常
		JMADF::raiseException("Can't init module:" + info->id);

		//根据模块调用栈卸载模块
		{
			juce::GenericScopedLock<juce::CriticalSection> locker(mod->llListLock);
			for (auto& i : mod->LLList) {
				JMADF::unload(i);
			}
		}

		//移除钩子
		{
			juce::ScopedWriteLock locker2(this->hookLock);
			this->loadHookList.erase(info->id);
			this->unloadHookList.erase(info->id);
			this->loadCallbackList.erase(info->id);
			this->unloadCallbackList.erase(info->id);
		}

		//从模块列表中移除
		this->moduleList.erase(info->id);

		//销毁模块
		mod->destory();
		delete mod;
		return false;
	}

	//调用模块加载回调
	{
		juce::ScopedReadLock locker2(this->hookLock);
		for (auto& p : this->loadCallbackList) {
			if (p.first != info->id) {
				p.second(info->id);
			}
		}
	}
	
	//如果调用者非主模块，则记录在调用者调用栈中
	if (!loader.isEmpty()) {
		auto* loaderPtr = this->moduleList[loader];
		juce::GenericScopedLock<juce::CriticalSection> locker(loaderPtr->llListLock);
		loaderPtr->LLList.push_front(info->id);
	}

	return true;
}

void ModulePool::unload(const juce::String& loader, const juce::String& moduleId)
{
	//锁模块表
	juce::ScopedWriteLock locker(this->listLock);

	//如果模块未加载，则不做任何操作
	if (!this->moduleList.contains(moduleId)) {
		return;
	}

	//如果调用者非主模块，则从调用者调用栈中移除
	if (!loader.isEmpty()) {
		auto* loaderPtr = this->moduleList[loader];
		juce::GenericScopedLock<juce::CriticalSection> locker(
			loaderPtr->llListLock);
		loaderPtr->LLList.remove(moduleId);
	}
	
	//获取模块指针
	JModule* mod = this->moduleList[moduleId];
	if (mod->count == 0) {
		//如果引用计数为0（只有一次加载），则卸载模块

		//根据模块调用栈卸载模块
		{
			juce::GenericScopedLock<juce::CriticalSection> locker(mod->llListLock);
			for (auto& i : mod->LLList) {
				JMADF::unload(i);
			}
		}

		//清除被卸载模块的钩子
		{
			juce::ScopedWriteLock locker2(this->hookLock);
			this->loadHookList.erase(moduleId);
			this->unloadHookList.erase(moduleId);
			this->loadCallbackList.erase(moduleId);
			this->unloadCallbackList.erase(moduleId);
		}

		//从列表中移除模块
		this->moduleList.erase(moduleId);

		//销毁模块
		mod->destory(&(this->hookLock), &(this->unloadHookList), &(this->unloadCallbackList));
		delete mod;
	}
	else {
		//减少引用计数
		mod->count--;
	}
}

bool ModulePool::isLoaded(const juce::String& moduleId)
{
	juce::ScopedReadLock locker(this->listLock);
	return this->moduleList.contains(moduleId);
}

void ModulePool::setLoadHook(const juce::String& moduleId, const jmadf::HookFunction& hook)
{
	juce::ScopedWriteLock locker(this->hookLock);
	this->loadHookList.insert(std::make_pair(moduleId, hook));
}

void ModulePool::setUnloadHook(const juce::String& moduleId, const jmadf::HookFunction& hook)
{
	juce::ScopedWriteLock locker(this->hookLock);
	this->unloadHookList.insert(std::make_pair(moduleId, hook));
}

void ModulePool::setLoadCallback(const juce::String& moduleId, const jmadf::HookFunction& hook)
{
	juce::ScopedWriteLock locker(this->hookLock);
	this->loadCallbackList.insert(std::make_pair(moduleId, hook));
}

void ModulePool::setUnloadCallback(const juce::String& moduleId, const jmadf::HookFunction& hook)
{
	juce::ScopedWriteLock locker(this->hookLock);
	this->unloadCallbackList.insert(std::make_pair(moduleId, hook));
}

bool ModulePool::canRefresh()
{
	juce::ScopedReadLock locker(this->listLock);
	return !this->moduleList.size();
}

jmadf::JInterface* ModulePool::getInterface(const juce::String& moduleId)
{
	juce::ScopedReadLock locker(this->listLock);
	if (!this->moduleList.contains(moduleId)) {
		return nullptr;
	}
	return this->moduleList[moduleId]->getInterface();
}
