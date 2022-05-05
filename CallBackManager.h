#pragma once
#include <JuceHeader.h>

//回调管理器，用于保存模块注册的回调

class CallBackManager final
{
	class CallBackObjectBase
	{
		JUCE_LEAK_DETECTOR(CallBackObjectBase)
	};

	template<typename T>
	class CallBackObject final :
		public CallBackObjectBase
	{
		using F = std::function<T>;
		F _data;
	public:
		CallBackObject(const F& data)
			:_data(data)
		{};

		const F& operator()()
		{
			return this->_data;
		};

	private:
		JUCE_LEAK_DETECTOR(CallBackObject)
	};

public:
	CallBackManager();
	~CallBackManager();

	template<typename T, class F = std::function<T>, class U = CallBackManager::CallBackObject<T>>
	void set(const juce::String& key, const F& func)
	{
		this->list.set(
			key,
			std::make_shared<U>(func)
		);
	};

	template<typename T, typename ...A, class F = std::function<T>, class U = CallBackManager::CallBackObject<T>>
	void call(const juce::String& key, A ...args)
	{
		if (this->list.contains(key)) {
			U* obj = reinterpret_cast<U*>(this->list.getReference(key).get());
			const F& func = (*obj)();
			func(args...);
		}
	};
private:
	juce::HashMap<juce::String, std::shared_ptr<CallBackObjectBase>> list;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CallBackManager)
};
