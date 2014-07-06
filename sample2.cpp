#include "class_trace.h"

class Foo
{
	TRACE_CLASS(Foo)

public:
	Foo()
	{
		TRACE_SCOPE;
	}

	static void f() { TRACE_SCOPE; }
};

class Bar
{
	TRACE_CLASS(Bar)

public:
	Bar()
	{
		TRACE_SCOPE;
		Foo::f();
	}
};

int main()
{
	Foo f;
	Bar b;

	return 0;
}
