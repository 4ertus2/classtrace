ClassTrace is an open-source library for tracing C++ programs.

## Getting Started

First you need is to download and include these files to your project:
- class_trace.h (public interface)
- class_trace_int.h (internals)
- trace_config.cpp (trace configuration, optional)

Second you include class_trace.h and declare class you need to trace using TRACE_CLASS macro
```
#include "class_trace.h"

class SomeThing
{
  TRACE_CLASS(SomeThing);

  // ...
};
```
After that you can use any of these macros in the class's methods as C++ streams that writes their's out to class.trace file (or a file you can set by CLASS_TRACE_OUT compiler definition):
- TRACE_EMERG
- TRACE_ALERT
- TRACE_CRIT
- TRACE_ERROR
- TRACE_WARN
- TRACE_NOTICE
- TRACE_INFO
- TRACE_DEBUG

```
// sample1.cpp
#include "class_trace.h"

class SomeThing
{
	TRACE_CLASS(SomeThing);

public:
	void foo() const
	{
		TRACE_SCOPE;

		for (unsigned i = 0; i < 20; ++i)
		{
			if ((i % 4) == 0)
				TRACE_INFO << "another 4: " << i;
			bar(i);
		}
	}

	void bar(unsigned x) const
	{
		TRACE_DEBUG << x;
	}
};

int main()
{
	SomeThing s;
	s.foo();
	return 0;
}
```
After compilation and run you'll get a file class.trace with such content
```
03.07.2014 19:41:39 SomeThing.foo [10] entry
03.07.2014 19:41:39 SomeThing.foo [15] another 4: 0
03.07.2014 19:41:39 SomeThing.bar [22] 0
03.07.2014 19:41:39 SomeThing.bar [22] 1
03.07.2014 19:41:39 SomeThing.bar [22] 2
03.07.2014 19:41:39 SomeThing.bar [22] 3
03.07.2014 19:41:39 SomeThing.foo [15] another 4: 4
03.07.2014 19:41:39 SomeThing.bar [22] 4
03.07.2014 19:41:39 SomeThing.bar [22] 5
03.07.2014 19:41:39 SomeThing.bar [22] 6
03.07.2014 19:41:39 SomeThing.bar [22] 7
03.07.2014 19:41:39 SomeThing.foo [15] another 4: 8
03.07.2014 19:41:39 SomeThing.bar [22] 8
03.07.2014 19:41:39 SomeThing.bar [22] 9
03.07.2014 19:41:39 SomeThing.bar [22] 10
03.07.2014 19:41:39 SomeThing.bar [22] 11
03.07.2014 19:41:39 SomeThing.foo [15] another 4: 12
03.07.2014 19:41:39 SomeThing.bar [22] 12
03.07.2014 19:41:39 SomeThing.bar [22] 13
03.07.2014 19:41:39 SomeThing.bar [22] 14
03.07.2014 19:41:39 SomeThing.bar [22] 15
03.07.2014 19:41:39 SomeThing.foo [15] another 4: 16
03.07.2014 19:41:39 SomeThing.bar [22] 16
03.07.2014 19:41:39 SomeThing.bar [22] 17
03.07.2014 19:41:39 SomeThing.bar [22] 18
03.07.2014 19:41:39 SomeThing.bar [22] 19
03.07.2014 19:41:39 SomeThing.foo [10] return
```
Another macro is TRACE_SCOPE that traces scope entry and return.

## Configuration
To disable trace for some classes you need build classtrace library and define CLASS_TRACE_CONFIG for compiler.
In this case you will can configure trace for the specified classes using configuration file.
The trace config file sets by CLASS_TRACE_CONFIG enviroment variable.
The same prorgam (sample1.cpp) but with this config leads to another thrace file
```
# trace.init
SomeThing INFO
```
```
03.07.2014 19:41:39 SomeThing.foo [15] another 4: 0
03.07.2014 19:41:39 SomeThing.foo [15] another 4: 4
03.07.2014 19:41:39 SomeThing.foo [15] another 4: 8
03.07.2014 19:41:39 SomeThing.foo [15] another 4: 12
03.07.2014 19:41:39 SomeThing.foo [15] another 4: 16
```
It doesn't trace TRACE_DEBUG and TRACE_SCOPE couse the least trace level for SomeThing is INFO.

In configurable mode ClassTrace you may set different output files for the classes.
```
# trace.init
.destination DEFAULT	./class.trace
.destination DST_1	  ./other.trace

Foo				  SCOPE
Bar				  SCOPE	DEFAULT
SomeThing		INFO	DST_1
```

## Optimisation
To prevent drop in performance in release version of you program you can disable ClassTrace entirely by defining DISABLE_CLASS_TRACE or disable only low trace levels by defining CLASS_TRACE_LEVEL to some value. Ex. setting CLASS_TRACE_LEVEL to 2 will trace TRACE_CRIT and TRACE_ALERT messages only.
