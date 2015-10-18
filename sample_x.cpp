#include "trace.h"

void bar(int depth);

class SomeThing
{
	TRACE_CLASS(SomeThing)

public:
	SomeThing()
	{
		TRACE_SCOPE;

		foo(10);
	}

	~SomeThing()
	{
		TRACE_SCOPE;

		foo(5);
	}

	static void foo(int depth)
	{
		TRACE_SCOPE_N(SomeThing);

		bar(depth);
	}
};

void bar(int depth)
{
	TRACE_SCOPE_F;

	--depth;
	if (!depth)
		return;

	SomeThing::foo(depth);
}

int main()
{
	TraceScope::reset(new TraceBuffer());

	SomeThing s;
	return 0;
}
