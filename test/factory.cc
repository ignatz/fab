#include <gtest/gtest.h>
#include "fab/fab.h"

using namespace fab;

/// testing
struct Base { virtual ~Base(){} };
struct A : public Base {};
struct B : public Base {};

A* createA() { return new A; }

TEST(Factory, Base)
{
	Factory<Base> factory;

	testing::StaticAssertTypeEq<
		std::unique_ptr<Base>,
		Factory<Base>::return_type>();

	factory.Register("A", &createA);

	auto ret = factory.Create("A");
	auto& a  = cast<A>(ret);
	testing::StaticAssertTypeEq<decltype(a), A&>();
}

TEST(Factory, StdFunction)
{
	Factory<Base> factory;

	// explicit conversion to std::function
	std::function<Base* ()> f =
		[](){return new A;};

	factory.Register("A", f);

	ASSERT_NO_THROW(factory.Create("A"));
}

TEST(Factory, Lambda)
{
	Factory<Base> factory;

	factory.Register("lambdaA", [](int) { return new A; });

	ASSERT_NO_THROW(factory.Create("lambdaA", 42));
}

TEST(Factory, DefaultDelegate)
{
	Factory<Base> factory;

	factory.Register("B", &delegate<B>);
	ASSERT_NO_THROW(factory.Create("B"));
}

TEST(Factory, Exception)
{
	Factory<Base> factory;
	factory.Register("A", &createA);
	ASSERT_THROW(factory.Create("A", 4), exception::BadArguments);
	ASSERT_THROW(factory.Create("hihi"), exception::UnknownKey);
}
