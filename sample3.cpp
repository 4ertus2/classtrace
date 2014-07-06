#include "class_trace.h"

TRACE_LOCAL(MyProgram)

static void f()
{
	TRACE_SCOPE;
}

int main()
{
	TRACE_SCOPE;
	TRACE_INFO << "start";

	f();

	TRACE_INFO << "stop";
	return 0;
}
