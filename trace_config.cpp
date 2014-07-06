#include <cstdlib>

#include <map>
#include <string>
#include <fstream>
#include <iostream>

#include "class_trace_int.h"


#ifdef CLASS_TRACE_CONFIG

using std::string;

namespace class_trace
{

///
class Tracer
{
public:
	typedef shared_ptr<Destination> DestinationPtr;

	struct Source
	{
		SeverityLevel level;
		DestinationPtr dst;

		Source()
		:	level(SL_DISABLED)
		{}

		Source(SeverityLevel l, DestinationPtr d)
		:	level(l), dst(d)
		{}
	};

	constexpr static const char * ENV_VAR_NAME = "CLASS_TRACE_CONFIG";
	constexpr static const char * DEFAULT_DST = "DEFAULT";

	static Tracer& instance()
	{
		static Tracer t(EXPAND_AND_QUOTE(CLASS_TRACE_OUT));
		return t;
	}

	Source src(const std::string& name) const
	{
		if (! enabled_)
			return Source();

		SrcCont::const_iterator it = sources_.find(name);
		if (it != sources_.end())
			return it->second;

		return Source();
	}

private:
	typedef std::map<std::string, Source>			SrcCont;
	typedef std::map<std::string, DestinationPtr>	DstCont;

	bool enabled_;
	std::string defDst_;
	SrcCont sources_;
	DstCont destinations_;

	Tracer(const char * outFile)
	:	enabled_(true),
		defDst_(DEFAULT_DST)
	{
		init();

		if (outFile && destinations_.find(defDst_) == destinations_.end())
			destinations_[defDst_] = make_shared<Destination>(outFile);
	}

	DestinationPtr dst(const std::string& name) const
	{
		DstCont::const_iterator it = destinations_.find(name);
		if (it != destinations_.end())
			return it->second;

		return DestinationPtr();
	}

	DestinationPtr dstDefault() const
	{
		DstCont::const_iterator it = destinations_.find(defDst_);
		if (it != destinations_.end())
			return it->second;

		return DestinationPtr();
	}

	void init();
	SeverityLevel getSeverity(const string& s) const;

	Tracer(const Tracer&);
	Tracer& operator = (const Tracer&);
};

void Tracer::init()
{
	using std::cerr;
	using std::endl;

	static const char * strPrefix = "ClassTrace: ";

	try
	{
		const char * cfgName = getenv(ENV_VAR_NAME);
		if (! cfgName)
		{
			cerr << strPrefix << "disabled. To enable it set environment '" << ENV_VAR_NAME << "'" << endl;
			return;
		}

		std::ifstream config(cfgName);

		if (! config.is_open())
		{
			cerr << strPrefix << "could not open config file <" << cfgName << ">" << endl;
			return;
		}

		while (! config.eof())
		{
			string line;
			std::getline(config, line);
			std::istringstream iss(line);
			string word;

			iss >> word;
			if (word.empty() || word[0] == '#')
				continue;

			if (word == string(".trace"))
			{
				iss >> word;
				enabled_ = (word == string("on"));
			}
			else if (word == string(".destination"))
			{
				string name;
				string fileName;
				iss >> name >> fileName;

				destinations_[name] = make_shared<Destination>(fileName.c_str());
			}
			else
			{
				string severity;
				string destination;
				iss >> severity >> destination;

				DestinationPtr d;
				if (! destination.empty())
					d = dst(destination);
				if (! d)
					d = dstDefault();

				SeverityLevel level = getSeverity(severity);
				sources_[word] = Source(level, d);
			}
		}
	}
	catch (const std::exception& e)
	{
		cerr << strPrefix << e.what() << endl;
	}
	catch (...)
	{
		cerr << strPrefix << "unhandled exception" << endl;
	}
}

SeverityLevel Tracer::getSeverity(const string& s) const
{
	if (s == "EMERG")	return EMERG;
	if (s == "ALERT")	return ALERT;
	if (s == "CRIT")	return CRIT;
	if (s == "ERROR")	return ERROR;
	if (s == "WARN")	return WARN;
	if (s == "NOTICE")	return NOTICE;
	if (s == "INFO")	return INFO;
	if (s == "DEBUG")	return DEBUG;

	if (s == "SCOPE")	return SL_SCOPE;
	return SL_DISABLED;
}

void ClassTrace::init()
{
	Tracer::Source src = Tracer::instance().src(name_);
	level_ = src.level;
	dst_ = src.dst;

	if (! dst_)
		level_ = SL_DISABLED;
}

}

#endif
