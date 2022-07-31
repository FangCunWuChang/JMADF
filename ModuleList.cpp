#include "ModuleList.h"

ModuleList::ModuleList()
{
	
}

ModuleList::~ModuleList()
{
	
}

void ModuleList::refresh(const juce::String& moduleDir, const juce::String& product)
{
	juce::ScopedWriteLock locker(this->listLock);
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
				//const juce::String& group = info["group"].toString();
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
				
				jmadf::ModuleInfo& moduleInfo = this->moduleList.getReference(moduleName);
				moduleInfo.id = id;
				moduleInfo.version = version;
				//moduleInfo.group = group;
				if (info["group"].isString()) {
					moduleInfo.group.add(info["group"].toString());
				}
				else if (info["group"].isArray()) {
					juce::Array<juce::var>* groupArray = info["group"].getArray();
					for (auto& g : *groupArray) {
						moduleInfo.group.add(g.toString());
					}
				}
				moduleInfo.productId = productId;
				moduleInfo.path = modulePath;
				moduleInfo.entry = entry;
				moduleInfo.infoList = infoFile.getFileName();
				moduleInfo.description = description;
				moduleInfo.dependencies = dependencies;
			}
		}
	}
}

bool ModuleList::exists(const juce::String& moduleId)
{
	juce::ScopedReadLock locker(this->listLock);
	return this->moduleList.contains(moduleId);
}

void ModuleList::clear()
{
	juce::ScopedWriteLock locker(this->listLock);
	this->moduleList.clear();
}

const jmadf::ModuleInfo* ModuleList::find(const juce::String& moduleId)
{
	juce::ScopedReadLock locker(this->listLock);
	if (this->moduleList.contains(moduleId)) {
		return &this->moduleList.getReference(moduleId);
	}
	return nullptr;
}

const juce::StringArray ModuleList::getList()
{
	juce::StringArray array;
	juce::ScopedReadLock locker(this->listLock);
	for (auto m : this->moduleList) {
		array.add(m.id);
	}
	return array;
}

const juce::StringArray ModuleList::getListByGroup(const juce::String& groupName)
{
	juce::StringArray array;
	juce::ScopedReadLock locker(this->listLock);
	for (auto m : this->moduleList) {
		for (auto& g : m.group) {
			if (g == groupName) {
				array.add(m.id);
			}
		}
	}
	return array;
}
