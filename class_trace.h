#ifndef _CLASS_TRACE_H_
#define _CLASS_TRACE_H_

#ifndef DISABLE_CLASS_TRACE

#ifdef _WIN32_
#define __func__ __FUNCTION__
#endif

#include "class_trace_int.h"

#define TRACE_CLASS(name)	static class_trace::ClassTrace& classTrace() {\
		static class_trace::ClassTrace ct(#name);\
		return ct; }

#define TRACE_LOCAL(name)	namespace { TRACE_CLASS(name) }

#define MACRO_CONCAT_IMPL(x, y)	x##y
#define MACRO_CONCAT(x, y)		MACRO_CONCAT_IMPL(x, y)
#define TRACE_SCOPE		class_trace::Scope MACRO_CONCAT(scopeTrace_, __COUNTER__)(__func__, __FILE__, __LINE__, classTrace())

#define TRACE_EMERG		classTrace().trace(class_trace::EMERG,	__func__, __FILE__, __LINE__)
#define TRACE_ALERT		classTrace().trace(class_trace::ALERT,	__func__, __FILE__, __LINE__)
#define TRACE_CRIT		classTrace().trace(class_trace::CRIT,	__func__, __FILE__, __LINE__)
#define TRACE_ERROR		classTrace().trace(class_trace::ERROR,	__func__, __FILE__, __LINE__)
#define TRACE_WARN		classTrace().trace(class_trace::WARN,	__func__, __FILE__, __LINE__)
#define TRACE_NOTICE	classTrace().trace(class_trace::NOTICE,	__func__, __FILE__, __LINE__)
#define TRACE_INFO		classTrace().trace(class_trace::INFO,	__func__, __FILE__, __LINE__)
#define TRACE_DEBUG		classTrace().trace(class_trace::DEBUG,	__func__, __FILE__, __LINE__)

#ifdef CLASS_TRACE_LEVEL
# undef TRACE_SCOPE
# define TRACE_SCOPE
# if CLASS_TRACE_LEVEL < 7
#  undef TRACE_DEBUG
# endif
# if CLASS_TRACE_LEVEL < 6
#  undef TRACE_INFO
# endif
# if CLASS_TRACE_LEVEL < 5
#  undef TRACE_NOTICE
# endif
# if CLASS_TRACE_LEVEL < 4
#  undef TRACE_WARN
# endif
# if CLASS_TRACE_LEVEL < 3
#  undef TRACE_ERROR
# endif
# if CLASS_TRACE_LEVEL < 2
#  undef TRACE_CRIT
# endif
# if CLASS_TRACE_LEVEL < 1
#  undef TRACE_ALERT
# endif
#endif // CLASS_TRACE_LEVEL

#else // DISABLE_CLASS_TRACE

#define TRACE_CLASS(name)
#define TRACE_LOCAL(name)
#define TRACE_SCOPE

#endif // DISABLE_CLASS_TRACE


#ifndef TRACE_EMERG
#define TRACE_EMERG		class_trace::DummyWriter()
#endif

#ifndef TRACE_ALERT
#define TRACE_ALERT		class_trace::DummyWriter()
#endif

#ifndef TRACE_CRIT
#define TRACE_CRIT		class_trace::DummyWriter()
#endif

#ifndef TRACE_ERROR
#define TRACE_ERROR		class_trace::DummyWriter()
#endif

#ifndef TRACE_WARN
#define TRACE_WARN		class_trace::DummyWriter()
#endif

#ifndef TRACE_NOTICE
#define TRACE_NOTICE	class_trace::DummyWriter()
#endif

#ifndef TRACE_INFO
#define TRACE_INFO		class_trace::DummyWriter()
#endif

#ifndef TRACE_DEBUG
#define TRACE_DEBUG		class_trace::DummyWriter()
#endif

namespace class_trace
{
	class DummyWriter
	{
	public:
		template<typename T>
		inline DummyWriter& operator << (const T&)
		{
			return *this;
		}
	};
}

#endif
