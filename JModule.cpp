#include "JModule.h"

bool JModule::init(const ModuleInfo* info)
{
	if (!info) {
		return false;
	}
	this->info = info;

	this->library = std::make_unique<juce::DynamicLibrary>();
	if (!this->library->open(info->path + "/" + info->entry)) {
		return false;
	}
	
	this->createInstanceFunc
		= reinterpret_cast<JModuleBase * (*)(void)>(this->library->getFunction("JModuleCreateInstance"));
	this->destoryInstanceFunc
		= reinterpret_cast<void(*)(JModuleBase*)>(this->library->getFunction("JModuleDestroyInstance"));

	if (!this->createInstanceFunc || !this->destoryInstanceFunc) {
		return false;
	}
	
	this->moduleClass.reset(this->createInstanceFunc());
	
	if (!this->moduleClass) {
		return false;
	}
	
	if (this->moduleClass->getModuleName() != info->id) {
		return false;
	}
	if (this->moduleClass->getModuleVersion() != info->version) {
		return false;
	}
	
	return this->moduleClass->init();
}

void JModule::destory()
{
	if (this->moduleClass) {
		this->moduleClass->destory();
	}
	if (this->destoryInstanceFunc) {
		this->destoryInstanceFunc(this->moduleClass.release());
	}
	if(this->library) {
		this->library->close();
	}
}
