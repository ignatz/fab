#pragma once

// Copyright (c) 2012, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the GPLv2 or newer

#include <map>
#include <string>
#include <memory>
#include <type_traits>

#include "fab/exception.h"

namespace fab {

template<typename Base>
class BaseFunction
{
public:
	typedef std::unique_ptr<Base> return_type;
	virtual ~BaseFunction() {}
};


template<typename Base, typename ... Args>
class Function : public BaseFunction<Base>
{
public:
	typedef typename BaseFunction<Base>::return_type return_type;
	typedef std::function<return_type (Args...)> func_t;
	Function(func_t const& func) : _func(func) {}

	return_type operator() (Args&& ... args) const
	{
		return _func(std::forward<Args>(args)...);
	}

protected:
	func_t const& _func;
};


template<typename Base, typename Key = std::string>
struct Factory
{
protected:
	typedef Key key_type;
	typedef std::unique_ptr<BaseFunction<Base>> value_type;
	typedef typename BaseFunction<Base>::return_type return_type;
	std::map<key_type, value_type> _map;

public:
	template<typename ... Args>
	return_type
	Create(
		Key const& key,
		Args&& ... args);

	/// registers std::functions
	template<typename ... Args>
	void Register(
		Key const& key,
		std::function<return_type (Args ... args)> createFunc);

	/// registers function pointer
	template<typename ... Args>
	void Register(
		Key const& key,
		return_type (*createFunc) (Args ... args));

	/// registers zero-argument lambdas like []() { return ...; }
	template<typename Lambda>
	void Register(
		Key const& key,
		Lambda const& lambda);
};


// implementations

template<typename Base, typename Key>
template<typename ... Args>
typename Factory<Base, Key>::return_type
Factory<Base, Key>::Create(
	Key const& key,
	Args&& ... args)
{
	auto ret = _map.find(key);
	if (ret == _map.end())
		throw exception::UnknownKey();

	typedef Function<Base, Args...> wrapper_t;
	try {
		wrapper_t const& wrapper =
			dynamic_cast<wrapper_t const&>(*(ret->second));
		return wrapper(std::forward<Args>(args)...);
	} catch (std::bad_cast& e) {
		throw exception::BadArguments();
	}
}

template<typename Base, typename Key>
template<typename ... Args>
void Factory<Base, Key>::Register(
	Key const& key,
	std::function<return_type (Args ... args)> createFunc)
{
	typedef Function<Base, Args...> wrapper_t;
	_map[key] = value_type(new wrapper_t(createFunc));
}

template<typename Base, typename Key>
template<typename ... Args>
void Factory<Base, Key>::Register(
	Key const& key,
	return_type (*createFunc) (Args ... args))
{
	typedef Function<Base, Args...> wrapper_t;
	typedef std::function<std::unique_ptr<Base> (Args ... args)> func_t;
	_map[key] = value_type (new wrapper_t(func_t(createFunc)));
}

template<typename Base, typename Key>
template<typename Lambda>
void Factory<Base, Key>::Register(
	Key const& key,
	Lambda const& lambda)
{
	typedef Function<Base> wrapper_t;
	typedef std::function<std::unique_ptr<Base> ()> func_t;
	_map[key] = value_type (new wrapper_t(func_t(lambda)));
}

} // fab
