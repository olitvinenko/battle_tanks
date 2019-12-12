#pragma once

template<typename T>
class Singleton
{
public:
	static T* GetInstance();

protected:
	Singleton();
	virtual ~Singleton();

	static T* _instance;
};

template<typename T>
T* Singleton<T>::_instance = nullptr;

template<typename T>
T* Singleton<T>::GetInstance()
{
	assert(_instance != nullptr);
	return _instance;
}

template<typename T>
Singleton<T>::Singleton()
{
	_instance = (T*)this;
}

template<typename T>
Singleton<T>::~Singleton()
{
	_instance = nullptr;
}