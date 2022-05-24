#include "JModule.h"
#include "JMADF.h"
#include "libJModule/Utils.h"

bool JModule::init(const jmadf::ModuleInfo* info, const jmadf::StaticInterface* staticInterface)
{
	if (!info) {
		JMADF::raiseException("nullptr!");
		return false;
	}
	this->info = info;

	this->library = std::make_unique<juce::DynamicLibrary>();
	if (!this->library->open(info->path + "/" + info->entry)) {
		JMADF::raiseException("Can't open library:" + info->path + "/" + info->entry);
		return false;
	}
	
	void* ptrCIF = this->library->getFunction("JModuleCreateInstance");
	void* ptrDIF = this->library->getFunction("JModuleDestroyInstance");
	this->createInstanceFunc
		= reinterpret_cast<jmadf::JModuleBase * (*)(void)>(
			ptrCIF
			);
	this->destoryInstanceFunc
		= reinterpret_cast<void(*)(const jmadf::JModuleBase*)>(
			ptrDIF
			);

	if (!this->createInstanceFunc || !this->destoryInstanceFunc) {
		JMADF::raiseException(
			"Can't find the functions:\n[createInstanceFunc]<" +
			jmadf::ptrToString(ptrCIF) + ">\n[destoryInstanceFunc]<" +
			jmadf::ptrToString(ptrDIF) + ">"
		);
		return false;
	}
	
	this->moduleClass.reset(this->createInstanceFunc());
	if (!this->moduleClass) {
		JMADF::raiseException("Can't create the instance");
		return false;
	}
	this->moduleClass->initInfo(info);
	this->moduleClass->initLoader(staticInterface);
	
	if (this->moduleClass->getModuleName() != info->id) {
		JMADF::raiseException("Unexpected module id!");
		return false;
	}
	if (this->moduleClass->getModuleVersion() != info->version) {
		JMADF::raiseException("Unexpected module version!");
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
