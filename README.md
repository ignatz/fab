`fab` - A Generic C++11 Factory Class
=====================================

A generic `C++11` factory pattern implementation with runtime registration for
polymorphic classes. Any function pointer, `std::function` or zero-argument
lambda can be registered for instantiation.

  #include "fab/fab.h"
  
	struct Base {};
	struct A : public Base {};
	
	// ...
	
	Factory<Base> factory;
	factory.Register("A", [](){return std::unique_ptr<Base>(new B);});
	
	auto a = factory.Create("A");
  
	// ...
