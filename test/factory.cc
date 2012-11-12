#include <gtest/gtest.h>
#include "fab/factory.h"


using namespace fab;

/// testing
struct Base {};

struct A : public Base {};
struct B : public Base {};

std::unique_ptr<Base> createB() { return std::unique_ptr<Base>(new B); }

TEST(Factory, Base)
{
	Factory<Base> factory;

	std::function<std::unique_ptr<Base>()> f =
		[](){return std::unique_ptr<Base>(new A);};

	factory.Register("A", f);
	factory.Register("B", &createB);
	factory.Register("narf", [](){return std::unique_ptr<Base>(new B);});

	auto a = factory.Create("A");

	ASSERT_THROW(factory.Create("A", 4), exception::BadArguments);
	ASSERT_THROW(factory.Create("hihi"), exception::UnknownKey);
}
