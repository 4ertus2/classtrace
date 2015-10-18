#pragma once

#include <cstdio>
#include <cstdint>
#include <vector>
#include <memory>

#define QUOTE(s) #s
#define TRACE_SCOPE_N(class) TraceScope __scope(QUOTE(class), __func__, __LINE__);
#define TRACE_SCOPE_F TraceScope __scope(nullptr, __func__, __LINE__);
#define TRACE_SCOPE TraceScope __scope(this->className(), __func__, __LINE__);
#define TRACE_CLASS(name) static const char * className() { static const char * cn = #name; return cn; }

///
class TraceBuffer
{
public:
	///
	struct Entry
	{
		const char * className;
		const char * funcName;
		unsigned line;

		Entry(const char * cn = nullptr, const char * fn = nullptr, unsigned ln = 0)
		:	className(cn),
			funcName(fn),
			line(ln)
		{}

		bool empty() const { return !funcName; }
	};

	TraceBuffer()
	:	pushPos_(0)
	{
		static const unsigned SIZE = 64 * 1024; // uint16_t limit
		buf_.resize(SIZE);
	}

	virtual ~TraceBuffer()
	{
		for (auto it = buf_.begin() + pushPos_; it != buf_.end() && !it->empty(); ++it)
			printEntry(*it);

		buf_[pushPos_] = Entry();
		for (auto it = buf_.begin(); it != buf_.end() && !it->empty(); ++it)
			printEntry(*it);
	}

	void push(const Entry& entry)
	{
		buf_[pushPos_] = entry;
		++pushPos_; // use uint16_t overflow for cycle
	}

	virtual void printEntry(const Entry& entry) const
	{
		static unsigned padding = 0;
		if (padding && !entry.line)
			--padding;

		for (unsigned i=0; i<padding; ++i)
			fprintf(stdout, " ");
		if (entry.className)
			fprintf(stdout, "%s.%s [%u]\n", entry.className, entry.funcName, entry.line);
		else
			fprintf(stdout, "%s [%u]\n", entry.funcName, entry.line);

		if (entry.line)
			++padding;
	}

private:
	std::vector<Entry> buf_;
	uint16_t pushPos_;
};

///
class TraceScope
{
public:
	TraceScope(const char * className, const char * func, unsigned line)
	:	entry_(className, func, line)
	{
		TraceBuffer * tbuf = buffer();
		if (tbuf)
			tbuf->push(entry_);
	}

	~TraceScope()
	{
		TraceBuffer * tbuf = buffer();
		if (tbuf)
		{
			entry_.line = 0;
			tbuf->push(entry_);
		}
	}

	static void reset(TraceBuffer * tbuf = nullptr) { buffer(tbuf, true); }

private:
	TraceBuffer::Entry entry_;

	static TraceBuffer * buffer(TraceBuffer * tbuf = nullptr, bool needSet = false)
	{
		thread_local static std::unique_ptr<TraceBuffer> tbuf_;
		if (needSet)
			tbuf_.reset(tbuf);
		return tbuf_.get();
	}
};
