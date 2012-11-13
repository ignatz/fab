`fab` - A Generic C++11 Factory Class
=====================================

A generic `C++11` factory pattern implementation with runtime registration for
polymorphic classes. Any function pointer, `std::function` or lambda with
empty capture list can be registered for delegated construction.

  #include "fab/fab.h"
  
	struct Base {};
	struct A : public Base {};
	
	// ...
	
	Factory<Base> factory;
	factory.Register("A", [](int){return std::unique_ptr<Base>(new B);});
	
	auto a = factory.Create("A", 42 /* lambda int argument */);
  
	// ...
