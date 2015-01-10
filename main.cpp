#include "forward.h"

#include <csignal>

void debug()
{
	// For experiments only
}

void version()
{
	cout<<"1.2.0-alpha"<<endl;
}

void help()
{
	cout<<"MMDump - A MeMeDa Answering Engine"<<endl
		<<"(c) 2014-2015 ilufang"<<endl<<endl
		<<"Usage: mmdump -<auto|h|listen|ver> [accuracy]"<<endl
		<<"-auto:\tOperate MeMeDa automatically"<<endl
		<<"-h:\tHelp (This message)"<<endl
		<<"-listen:\tCapture Answer and send to stdout"<<endl
		<<"-ver:\tGet binary version"<<endl
		<<"accuracy:\tSucess rate, for automation only."<<endl;
}

void listen()
{
	MMDump().listen();
}

void operate(int accuracy)
{
	MMOperate(accuracy).start();
}


void terminate(int n)
{
	// Display a message to be shown as toast to indicate termination success
	cout<<"Process Terminated."<<endl;
	exit(0);
}

int main(int argc, char* argv[])
{
	signal(SIGINT, terminate); // Ctrl-C
	signal(SIGKILL, terminate); // kill()
	signal(SIGHUP, terminate); // Terminal
	signal(SIGTERM, terminate); // # busybox killall
	signal(SIGSTOP, terminate); // Ctrl-Z or debugger

	if (argc>1)
	{
		if (strcmp(argv[1],"-ver")==0)
		{
			version();
			return 0;
		}
		if (strcmp(argv[1],"-h")==0)
		{
			help();
			return 0;
		}
		if (strcmp(argv[1],"-listen")==0)
		{
			listen();
			return 0;
		}
		if (strcmp(argv[1],"-auto")==0)
		{
			if (argc>2)
			{
				operate(atoi(argv[2]));
			} else
			{
				operate(100);
			}
			return 0;
		}
		if (strcmp(argv[1],"-debug")==0)
		{
			debug();
			return 0;
		}
	}
	cout<<"No Parameters. (-h for help)"<<endl;
	return -1;
}