#include "JModule.h"
#include "JMADF.h"
#include "libJModule/Utils.h"

bool JModule::init(const jmadf::ModuleInfo* info, const jmadf::StaticInterface* staticInterface)
{
	//如果信息表无效，则失败，否则记录
	if (!info) {
		JMADF::raiseException("nullptr!");
		return false;
	}
	this->info = info;
	
	//根据平台选择扩展名
#if JUCE_WINDOWS
	const char* entryExtension = ".dll";
#endif
#if JUCE_LINUX || JUCE_ANDROID
	const char* entryExtension = ".so";
#endif
#if JUCE_MAC || JUCE_IOS
	const char* entryExtension = ".dylib";
#endif
	
	//动态加载动态库
	this->library = std::make_unique<juce::DynamicLibrary>();
	if (!this->library->open(info->path + "/" + info->entry + entryExtension)) {
		JMADF::raiseException("Can't open library:" + info->path + "/" + info->entry + entryExtension);
		return false;
	}
	
	//获取模块初始化与销毁方法
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

	//判断模块初始化与销毁方法有效性
	if (!this->createInstanceFunc || !this->destoryInstanceFunc) {
		JMADF::raiseException(
			"Can't find the functions:\n[createInstanceFunc]<" +
			jmadf::ptrToString(ptrCIF) + ">\n[destoryInstanceFunc]<" +
			jmadf::ptrToString(ptrDIF) + ">"
		);
		return false;
	}
	
	//获取模块实例与实例信息
	this->moduleClass.reset(this->createInstanceFunc());
	if (!this->moduleClass) {
		JMADF::raiseException("Can't create the instance");
		return false;
	}
	this->moduleClass->initInfo(info);
	this->moduleClass->initLoader(staticInterface);
	
	//参考信息表对比模块实例信息
	if (this->moduleClass->getModuleName() != info->id) {
		JMADF::raiseException("Unexpected module id!");
		return false;
	}
	if (this->moduleClass->getModuleVersion() != info->version) {
		JMADF::raiseException("Unexpected module version!");
		return false;
	}
	
	//建立模块接口表
	this->interfaces = new jmadf::JInterface();

	//初始化模块
	return this->moduleClass->init();
}

void JModule::destory(
	juce::ReadWriteLock* hookLock,
	std::map<juce::String, jmadf::HookFunction>* hooks,
	std::map<juce::String, jmadf::HookFunction>* callbacks)
{
	//如果模块实例有效
	if (this->moduleClass) {
		//执行钩子
		if (hookLock && hooks && this->info) {
			juce::ScopedReadLock locker(*hookLock);
			for (auto& p : (*hooks)) {
				p.second(this->info->id);
			}
		}

		//销毁模块
		this->moduleClass->destory();

		//执行回调
		if (hookLock && callbacks && this->info) {
			juce::ScopedReadLock locker(*hookLock);
			for (auto& p : (*callbacks)) {
				p.second(this->info->id);
			}
		}

		//销毁接口
		this->moduleClass->destoryInterfaces(this->interfaces);
		this->interfaces = nullptr;//将接口释放延伸至模块中避免模块间内存权限问题
		this->moduleClass->destoryLoader();
		this->moduleClass->destoryInfo();
	}
	//销毁模块实例
	if (this->destoryInstanceFunc) {
		this->destoryInstanceFunc(this->moduleClass.release());
	}
	//关闭动态库
	if(this->library) {
		this->library->close();
	}
}

jmadf::JInterface* JModule::getInterface()
{
	return this->interfaces;
}
