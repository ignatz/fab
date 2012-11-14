`fab` - A Generic C++11 Factory Class
=====================================

A generic `C++11` factory pattern implementation with runtime registration for
polymorphic classes. Any function pointer, `std::function` or non-capturing
lambda can be registered for delegated construction.

	#include "fab/fab.h"
	
	struct Base { virtual ~Base(){} };
	struct A : public Base {};
	
	// ...
	
	Factory<Base> factory;
	factory.Register("A", [](int i){ return new A;});
	
	auto ret = factory.Create("A", 42 /* lambda int argument */);
	A& a = cast<A>(ret);
	
	// ...
