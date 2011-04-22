#ifndef _LOG_HPP_
#define _LOG_HPP_

#define LOG_DEBUG_NONE      0
#define LOG_DEBUG_IMPORTANT 3
#define LOG_DEBUG_DETAILS   6
#define LOG_DEBUG_ALL       9

#define LD_NONE LOG_DEBUG_NONE
#define LD_IMP  LOG_DEBUG_IMPORTANT
#define LD_DET  LOG_DEBUG_DETAILS
#define LD_ALL  LOG_DEBUG_ALL

class Log {
	friend class Program;

	int debugLevel;
	bool generalDebug;

	Log(bool = false, int = LOG_DEBUG_NONE);
	~Log();

public:
	void enableGeneralDebug();
	void disableGeneralDebug();

	void setDebugLevel(int level);
	int getDebugLevel() const;

	void error(const char *, ...);
	void merror(const char *, ...);
	void info(const char *, ...);
	void debug(const char *, ...);
	void debugl(int, const char *, ...);
};

#endif
