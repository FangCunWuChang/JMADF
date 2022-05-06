#include "ModuleList.h"

ModuleList::ModuleList()
{
	
}

ModuleList::~ModuleList()
{
	
}

void ModuleList::refresh(const juce::String& moduleDir, const juce::String& product)
{
	this->moduleList.clear();
	
	juce::FileSearchPath moduleDirObject(moduleDir);
	juce::Array<juce::File> modulesObject = moduleDirObject.findChildFiles(juce::File::findDirectories, false);
	for (auto& f : modulesObject) {
		const juce::String& moduleName = f.getFileName();
		juce::File infoFile(f.getFullPathName() + "/" + moduleName + ".json");
		if (infoFile.existsAsFile()) {
			juce::var info = juce::JSON::parse(f);
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
				moduleInfo.entry = entry;
				moduleInfo.description = description;
				moduleInfo.dependencies = dependencies;
			}
		}
	}
}

bool ModuleList::exists(const juce::String& moduleId)
{
	return this->moduleList.contains(moduleId);
}

void ModuleList::clear()
{
	this->moduleList.clear();
}

const ModuleInfo* ModuleList::find(const juce::String& moduleId)
{
	if (this->moduleList.contains(moduleId)) {
		return &this->moduleList.getReference(moduleId);
	}
	return nullptr;
}

const juce::StringArray ModuleList::getList()
{
	juce::StringArray array;
	for (auto m : this->moduleList) {
		array.add(m.id);
	}
	return array;
}
