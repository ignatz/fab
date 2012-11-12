#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace factory {

struct UnknownKey : public std::runtime_error
{
	UnknownKey() : std::runtime_error("Factory: unknown key") {}
	virtual ~UnknownKey() throw() {}
};

struct BadArguments : public std::runtime_error
{
	BadArguments() : std::runtime_error("Factory: non-matching arguments") {}
	virtual ~BadArguments() throw() {}
};


template<typename Base>
struct FuncBase
{
	typedef std::unique_ptr<Base> return_type;
	virtual ~FuncBase() {}
};

template<typename Base, typename ... Args>
struct FuncWrapper : public FuncBase<Base>
{
public:
	typedef typename FuncBase<Base>::return_type return_type;
	typedef std::function<return_type (Args...)> func_t;
	FuncWrapper(func_t func) : _func(func) {}

	return_type operator() (Args&& ... args) const
	{
		return _func(std::forward<Args>(args)...);
	}

private:
	func_t _func;
};

template<typename Base, typename Key = std::string>
struct Factory
{
private:
	typedef Key key_type;
	typedef std::unique_ptr<FuncBase<Base>> value_type;
	std::map<key_type, value_type> _map;

public:
	template<typename ... Args>
	typename FuncBase<Base>::return_type
	Create(
		Key const& key,
		Args&& ... args)
	{
		auto ret = _map.find(key);
		if (ret == _map.end())
			throw UnknownKey();

		typedef FuncWrapper<Base, Args...> wrapper_t;
		try {
			wrapper_t const& wrapper =
				dynamic_cast<wrapper_t const&>(*(ret->second));
			return wrapper(std::forward<Args>(args)...);
		} catch (std::bad_cast& e) {
			throw BadArguments();
		}
	}

	/// registers std::functions
	template<typename ... Args>
	void Register(
		Key const& key,
		std::function<typename FuncBase<Base>::return_type (Args ... args)> createFunc)
	{
		typedef FuncWrapper<Base, Args...> wrapper_t;
		_map[key] = value_type(new wrapper_t(createFunc));
	}

	/// registers function pointer
	template<typename ... Args>
	void Register(
		Key const& key,
		std::unique_ptr<Base> (*createFunc) (Args ... args))
	{
		typedef FuncWrapper<Base, Args...> wrapper_t;
		typedef std::function<std::unique_ptr<Base> (Args ... args)> func_t;
		_map[key] = value_type (new wrapper_t(func_t(createFunc)));
	}

	/// registers zero-argument lambdas like []() { return ...; }
	template<typename Lambda>
	void Register(
		Key const& key,
		Lambda const& lambda)
	{
		typedef FuncWrapper<Base> wrapper_t;
		typedef std::function<std::unique_ptr<Base> ()> func_t;
		_map[key] = value_type (new wrapper_t(func_t(lambda)));
	}
};

} // factory
