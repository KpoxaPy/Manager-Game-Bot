#ifndef _PROGRAM_HPP_
#define _PROGRAM_HPP_

#include <stdlib.h>
#include "log.hpp"

class Program {
	static const char * Version;
	static const char * Name;

	bool help,
		version,
		debug;
	int debugLevel;

	int port;
	char * host;

	int argc;
	char ** argv;
	char ** envp;

	static bool programInitialised;
	static Program * pobj;
	static Log * plog;

	Program();
	~Program();
	Program(const Program &) {}
	void operator=(const Program &) {}

	void initOptions();
	void initLogging();

	void shutdownLogging();

public:
	static Program& get();
	static Log & log();
	static void endWork(int, bool = true);

	void init(int argc, char * argv[], char * envp[]);

	const char * getProgramName() const;
	const char * getRunName() const;

	int getPort() const;

	void printUsage() const;
	void printManual() const;
	void printAbout() const;
	void printDebug() const;
	void printHelp() const;
	void printVersion() const;
};

#endif
