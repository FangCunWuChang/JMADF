#include "JModule.h"

bool JModule::init(const jmadf::ModuleInfo* info, const jmadf::StaticInterface* staticInterface)
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
		= reinterpret_cast<jmadf::JModuleBase * (*)(void)>(
			this->library->getFunction("JModuleCreateInstance")
			);
	this->destoryInstanceFunc
		= reinterpret_cast<void(*)(const jmadf::JModuleBase*)>(
			this->library->getFunction("JModuleDestroyInstance")
			);

	if (!this->createInstanceFunc || !this->destoryInstanceFunc) {
		return false;
	}
	
	this->moduleClass.reset(this->createInstanceFunc());
	if (!this->moduleClass) {
		return false;
	}
	this->moduleClass->initInfo(info);
	this->moduleClass->initLoader(staticInterface);
	
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
		this->moduleClass->destoryLoader();
		this->moduleClass->destoryInfo();
	}
	if (this->destoryInstanceFunc) {
		this->destoryInstanceFunc(this->moduleClass.release());
	}
	if(this->library) {
		this->library->close();
	}
}
