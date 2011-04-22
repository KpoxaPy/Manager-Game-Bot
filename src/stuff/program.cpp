#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/resource.h>
#include <stdio.h>
#include <string.h>
#include "program.hpp"
#include "exceptions.hpp"

const char * Program::Version = "0.0.1";
const char * Program::Name = "Manager game robot";

bool Program::programInitialised = false;
Program * Program::pobj = 0;
Log * Program::plog = 0;

void mySignal(int sig);

Program& Program::get()
{
	if (pobj == 0)
		pobj = new Program();

	return *pobj;
}

Log & Program::log()
{
	if (pobj != 0)
		return *plog;
	else
		throw Bug("Trying to get logger without its initialisation.");
}

Program::Program()
{
	argc = 0;
	argv = 0;
	envp = 0;
	help = false;
	debug = false;
	version = false;
	debugLevel = LOG_DEBUG_NONE;

	port = 0;
	host = 0;

	plog = new Log(debug, debugLevel);
}

Program::~Program()
{
	shutdownLogging();

	if (host != 0)
		delete [] host;
}

void Program::init(int ac, char * av[], char * ep[])
{
	argc = ac;
	argv = av;
	envp = ep;

	initOptions();
	initLogging();

	printDebug();
	printHelp();
	printVersion();

	signal(SIGINT, mySignal);

	programInitialised = true;
}

void Program::initOptions()
{
	int opt, longOptInd, num;
	struct option long_options[] = {
		{"help", 0, 0, 'h'},
		{"dlevel", 1, 0, 'l'},
		{0, 0, 0, 0}
	};

	while (1)
	{
		opt = getopt_long(argc, argv, ":dl:hv", long_options, &longOptInd);

		if (opt == -1)
			break;

		switch (opt)
		{
			case 0:
				//switch (longOptInd)
				//{
					//case 2: [>  --syslog  <]
						//syslog = true;
						//break;
					//case 3: [> --key  <]
						//passkey = optarg;
				//}
				break;

			case 'd':
				debug = true;
				break;

			case 'h':
				help = true;
				break;

			case 'v':
				version = true;
				break;

			case 'l':
				num = atoi(optarg);
				if (num >= LOG_DEBUG_NONE && num <= LOG_DEBUG_ALL)
					debugLevel = num;
				break;

			case '?':
				throw UserError("unknown option");
				//port = atoi(optarg);
				//if (port == 0)
					//port = -1;
				break;

			case ':':
				throw UserError("option without argument");
				break;

			default:
				throw Bug("Unknown case at Program::initOptions");
		}
	}

	if (optind < argc) {
		// Getting port
		char * endcp;
		port = strtol(argv[optind], &endcp, 10);
		if (*endcp != '\0')
			throw UserError("first argument would be port");
		optind++;
	}
	else
		throw UserError("need two arguments");

	if (optind < argc) {
		// Getting host
		host = new char[strlen(argv[optind])];
		strcpy(host, argv[optind]);
		optind++;
	}
	else
		throw UserError("need two arguments");

	if (optind < argc)
		throw UserError("too many arguments");

	/* When used information argument which leads just echoing
	 * we don't need in strong testing */
	if (help || version)
		return;

	if (port <= 0)
	{
		if (port < 0)
			throw UserError("port is wrong, aborted");
		else
			throw UserError("port is unset, aborted");
	}
}

void Program::initLogging()
{
	if (debug)
		plog->enableGeneralDebug();
	else
		plog->disableGeneralDebug();

	plog->setDebugLevel(debugLevel);
}

void mySignal(int sig)
{
	Program & prog = Program::get();

	switch (sig)
	{
		case SIGINT:
			prog.log().info("Server got SIGINT.\n");
			Program::endWork(EXIT_SUCCESS);
			break;
	}
}

const char * Program::getProgramName() const
{
	return Name;
}

const char * Program::getRunName() const
{
	if (argv != 0)
		return argv[0];
	else
		return Name;
}

int Program::getPort() const
{
	return port;
}

void Program::printUsage() const
{
	log().info("Usage: %s [-dhv] [4m[22mhost[0m [4m[22mport[0m [-l [4m[22mdebugLevel[0m]\n", argv[0]);
}

void Program::printManual() const
{
	static const char help[] =
		"Robot for Manager game.\n\n"
		"There're flags to use:\n"
		"  -d			     debug mode -- more debugging output\n"
		"  -l, --dlevel		     set debug level (between 0 and 9)\n"
		"  -h, --help		     just output this message\n"
		"  -v, --version		     print version\n"
		;


	printUsage();
	log().info(help);
	log().info("\n");
}

void Program::printAbout() const
{
	static const char about[] =
		"Robot for Manager game.\n"
		"Version v%s\n"
		"Written by Andrew Ozmitel (aozmitel@gmail.com) in 2011.\n";

	log().info(about, Version);
}

void Program::printDebug() const
{
	if (debug)
	{
		log().debug("Used debug option with level: %d\n", debugLevel);

		log().debug("Server port: %d\n", port);

		log().debug("Server host: %s\n", host);
	}
}

void Program::printHelp() const
{
	if (help)
	{
		printManual();
		endWork(EXIT_SUCCESS);
	}
}

void Program::printVersion() const
{
	if (version)
	{
		printAbout();
		endWork(EXIT_SUCCESS);
	}
}

void Program::endWork(int status, bool ok)
{
	Program & prog = Program::get();
	if (status == EXIT_SUCCESS && !ok)
		prog.log().info("Robot is switching off. Thanks for using!\n");
	else if (!ok)
		prog.log().error("Program made fatal error. Aborting by problem.\n");

	if (pobj != 0)
		delete pobj;

	exit(status);
}

void Program::shutdownLogging()
{
	if (plog != 0)
		delete plog;
}
