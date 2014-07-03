#include "class_trace.h"

class SomeThing
{
	TRACE_CLASS(SomeThing); ///< enable trace for this class

public:
	void foo() const
	{
		TRACE_SCOPE; ///< trace this method

		for (unsigned i = 0; i < 20; ++i)
		{
			if ((i % 4) == 0)
				TRACE_INFO << "another 4: " << i; ///< trace with INFO severity
			bar(i);
		}
	}

	void bar(unsigned x) const
	{
		TRACE_DEBUG << x; ///< trace with DEBUG severity
	}
};

int main()
{
	SomeThing s;
	s.foo();
	return 0;
}
