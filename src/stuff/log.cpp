#include <alloca.h>
#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "exceptions.hpp"
#include "log.hpp"
#include "program.hpp"

Log::Log(bool genDebug, int level)
{
	setDebugLevel(level);

	if (genDebug)
		enableGeneralDebug();
	else
		disableGeneralDebug();
}

Log::~Log()
{
}

void Log::enableGeneralDebug()
{
	generalDebug = true;
}

void Log::disableGeneralDebug()
{
	generalDebug = false;
}

void Log::setDebugLevel(int level)
{
	debugLevel = level;

	if (level > LOG_DEBUG_NONE)
		enableGeneralDebug();
}

int Log::getDebugLevel() const
{
	return debugLevel;
}

void Log::error(const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

void Log::merror(const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ": %s\n", strerror(errno));
	va_end(ap);
}

void Log::info(const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

void Log::debug(const char * fmt, ...)
{
	va_list ap;

	if (!generalDebug)
		return;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

void Log::debugl(int level, const char * fmt, ...)
{
	va_list ap;

	if (debugLevel == LOG_DEBUG_NONE)
		return;

	if (level > debugLevel)
		return;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}
