#ifndef _CLASS_TRACE_INTERNALS_H_
#define _CLASS_TRACE_INTERNALS_H_

#include <ctime>
#include <string>
#include <sstream>
#include <fstream>

#if __cplusplus <= 199711L
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#else
#include <memory>
#include <mutex>
#endif


#ifndef CLASS_TRACE_OUT
#define CLASS_TRACE_OUT class.trace
#endif

#define QUOTE_MACRO(s) #s
#define EXPAND_AND_QUOTE(str) QUOTE_MACRO(str)


namespace class_trace
{

#if __cplusplus <= 199711L
using boost::shared_ptr;
using boost::make_shared;
using boost::mutex;
using boost::lock_guard;
#else
using std::shared_ptr;
using std::make_shared;
using std::mutex;
using std::lock_guard;
#endif

typedef enum
{
	SL_DISABLED = 0,
	EMERG,
	ALERT,
	CRIT,
	ERROR,
	WARN,
	NOTICE,
	INFO,
	DEBUG,
	SL_SCOPE = 13,
	SL_ALL = 14,
	SL_NOTINIT = 15
} SeverityLevel;

///
class Destination
{
public:
	Destination(const char * outFile)
	:	os_(outFile, std::ios_base::out | std::ios_base::app)
	{}

	void write(const char * className,
			const char * func,
			const char * file,
			unsigned line,
			const std::string& msg)
	{
		std::stringstream ss;
#if 1
		time_t now = time(NULL);
		struct tm ts;
		localtime_r(&now, &ts);

		char strDateTime[20];
		snprintf(strDateTime, 20, "%.2d.%.2d.%4d %.2d:%.2d:%.2d",
			ts.tm_mday, ts.tm_mon + 1, ts.tm_year + 1900,
			ts.tm_hour, ts.tm_min, ts.tm_sec);

		ss << strDateTime << ' ';
#endif
		ss << className << '.' << func
			<< " ["	//<< file << ':'
			<< line << "] "
			<< msg;

		lock_guard<mutex> lock(mtx_);
		os_ << ss.str() << std::endl;
	}

private:
	mutex mtx_;
	std::ofstream os_;
};

class Buffer;

///
class ClassTrace
{
public:
	ClassTrace(const char * className)
	:	level_(SL_NOTINIT),
		name_(className)
	{
		init();
	}

	SeverityLevel level() const { return level_; }

	Buffer trace(SeverityLevel level, const char * func, const char * file, unsigned line);

	void write(const char * func, const char * file, unsigned line, const std::string& msg) const
	{
		if (dst_)
			dst_->write(name_, func, file, line, msg);
	}

private:
	SeverityLevel level_;
	const char * name_;
	shared_ptr<Destination> dst_;

	void init();

	ClassTrace(const ClassTrace&);
	ClassTrace& operator = (const ClassTrace&);
};

// simple (all is on, one dst file) but header-only variant
#ifndef CLASS_TRACE_CONFIG
class Tracer
{
public:
	static Tracer& instance()
	{
		static Tracer t(EXPAND_AND_QUOTE(CLASS_TRACE_OUT));
		return t;
	}

	shared_ptr<Destination> dstDefault() const { return dst_; }

private:
	shared_ptr<Destination> dst_;

	Tracer(const char * outFile)
	{
		if (outFile)
			dst_ = make_shared<Destination>(outFile);
	}

	Tracer(const Tracer&);
	Tracer& operator = (const Tracer&);
};

inline void ClassTrace::init()
{
	dst_ = Tracer::instance().dstDefault();
	if (dst_)
		level_ = SL_ALL;
	else
		level_ = SL_DISABLED;
}
#endif

///
class Buffer
{
public:
	Buffer(SeverityLevel level,
			const char * func,
			const char * file,
			unsigned int line,
			const ClassTrace * cl)
	:	cl_(NULL)
	{
		if (level > cl->level())
			return;

		cl_ = cl;
		func_ = func;
		file_ = file;
		line_ = line;
	}

	Buffer(const Buffer& buf)
	:	cl_(buf.cl_)
	{
		if (cl_)
		{
			// Do not copy pss_. It's empty
			func_ = buf.func_;
			file_ = buf.file_;
			line_ = buf.line_;
		}
	}

	~Buffer()
	{
		if (! enabled())
			return;

		try
		{
			if (pss_)
				cl_->write(func_, file_, line_, pss_->str());
			else
				cl_->write(func_, file_, line_, std::string());
		}
		catch (...)
		{}
	}

	template <typename T>
	Buffer& operator << (const T& val)
	{
		if (enabled())
		{
			if (! pss_)
				pss_ = make_shared<std::ostringstream>();
			*pss_ << val;
		}
		return *this;
	}

	bool enabled() const { return cl_; }

private:
	const ClassTrace * cl_;
	shared_ptr<std::ostringstream> pss_;

	const char * func_;
	const char * file_;
	unsigned line_;

	Buffer& operator = (const Buffer&);
};

inline Buffer ClassTrace::trace(SeverityLevel level,
								const char * func,
								const char * file,
								unsigned int line)
{
	return Buffer(level, func, file, line, this);
}

///
class Scope
{
public:
	Scope(const char * func, const char * file, unsigned int line, ClassTrace& cl)
	:	w_(SL_SCOPE, func, file, line, &cl)
	{
		Buffer(SL_SCOPE, func, file, line, &cl) << "entry";
	}

	~Scope()
	{
		try
		{
			w_ << "return";
		}
		catch (...)
		{}
	}

private:
	Buffer w_;
};

}

#endif
