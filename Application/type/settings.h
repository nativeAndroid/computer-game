#pragma once

#include "system.h"

#include <string>
#include <unordered_map>
#include <typeindex>
#include <sfml/System.hpp>


struct Settings
{
	template <typename T>
	void Set(std::string Key, T Value)
	{
		auto i = list.find(Key);
		auto entry = std::make_pair((void*)(new T(Value)), std::type_index(typeid(T)));
		if(i == list.end())
		{
			// create new
			list.insert(std::make_pair(Key, entry));
		}
		else
		{
			// change existing
			delete i->second.first;
			i->second = entry;
		}
	}
	template <typename T>
	T *Get(std::string Key)
	{
		auto i = list.find(Key);
		if(i == list.end())
		{
			// setting not found
			return new T();
		}
		if(i->second.second != std::type_index(typeid(T)))
		{
			// type doesn't match
			HelperDebug::Fail("settings", "cannot get because (" + Key + ") does not exist");
			return new T();
		}
		return static_cast<T*>(i->second.first);
	}
private:
	std::unordered_map<std::string, std::pair<void*, std::type_index>> list;
};