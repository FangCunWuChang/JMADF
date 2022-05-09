#include "ModuleList.h"

ModuleList::ModuleList()
{
	
}

ModuleList::~ModuleList()
{
	
}

void ModuleList::refresh(const juce::String& moduleDir, const juce::String& product)
{
	this->listLock.enterWrite();
	this->moduleList.clear();
	
	juce::FileSearchPath moduleDirObject(moduleDir);
	juce::Array<juce::File> modulesObject = moduleDirObject.findChildFiles(juce::File::findDirectories, false);
	for (auto& f : modulesObject) {
		const juce::String& moduleName = f.getFileName();
		const juce::String& modulePath = f.getFullPathName();
		juce::File infoFile(modulePath + "/" + moduleName + ".json");
		if (infoFile.existsAsFile()) {
			juce::var info = juce::JSON::parse(infoFile);
			if (info.isObject()) {
				const juce::String& id = info["id"].toString();
				const juce::String& version = info["version"].toString();
				const juce::String& group = info["group"].toString();
				const juce::String& productId = info["productId"].toString();
				const juce::String& entry = info["entry"].toString();
				const juce::String& description = info["description"].toString();
				juce::StringArray dependencies;
				if (info["dependencies"].isArray()) {
					juce::Array<juce::var>* dependenciesArray = info["dependencies"].getArray();
					for (auto& d : *dependenciesArray) {
						dependencies.add(d.toString());
					}
				}
				
				if (id != moduleName) {
					continue;
				}
				if (productId != product) {
					continue;
				}
				
				ModuleInfo& moduleInfo = this->moduleList.getReference(moduleName);
				moduleInfo.id = id;
				moduleInfo.version = version;
				moduleInfo.group = group;
				moduleInfo.productId = productId;
				moduleInfo.path = modulePath;
				moduleInfo.entry = entry;
				moduleInfo.infoList = infoFile.getFileName();
				moduleInfo.description = description;
				moduleInfo.dependencies = dependencies;
			}
		}
	}
	this->listLock.exitWrite();
}

bool ModuleList::exists(const juce::String& moduleId)
{
	this->listLock.enterRead();
	bool result = this->moduleList.contains(moduleId);
	this->listLock.exitRead();
	return result;
}

void ModuleList::clear()
{
	this->listLock.enterWrite();
	this->moduleList.clear();
	this->listLock.exitWrite();
}

const ModuleInfo* ModuleList::find(const juce::String& moduleId)
{
	const ModuleInfo* ptr = nullptr;
	this->listLock.enterRead();
	if (this->moduleList.contains(moduleId)) {
		ptr = &this->moduleList.getReference(moduleId);
	}
	this->listLock.exitRead();
	return ptr;
}

const juce::StringArray ModuleList::getList()
{
	juce::StringArray array;
	this->listLock.enterRead();
	for (auto m : this->moduleList) {
		array.add(m.id);
	}
	this->listLock.exitRead();
	return array;
}
