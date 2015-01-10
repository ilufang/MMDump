#include "forward.h"

#include <csignal>

void version()
{
	cout<<"1.2.0-alpha"<<endl;
}

void help()
{
	cout<<"MMDump - A MeMeDa Answering Engine"<<endl
		<<"(c) 2014-2015 ilufang"<<endl<<endl
		<<"Usage: mmdump -<auto|h|listen|ver> [config]"<<endl
		<<"-auto:\tOperate MeMeDa automatically"<<endl
		<<"-h:\tHelp (This message)"<<endl
		<<"-listen:\tCapture Answer and send to stdout"<<endl
		<<"-ver:\tGet binary version"<<endl
		<<"config:\tFile containing configs"<<endl;
}

void listen()
{
	MMDump().listen();
}

void operate(char* confile)
{
	MMOperate(confile).start();
}

void operate()
{
	MMOperate().start();
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

	if (argv>1)
	{
		if (strcmp(argv[i],"-ver")==0)
		{
			version();
			return 0;
		}
		if (strcmp(argv[i],"-list")==0)
		{
			listDev();
			return 0;
		}
		if (strcmp(argv[i],"-h")==0)
		{
			help();
			return 0;
		}
		if (strcmp(argv[i],"-listen")==0)
		{
			listen();
			return 0;
		}
		if (strcmp(argv[i],"-auto")==0)
		{
			if (argc>2)
			{
				operate(argv[i+1]);
			} else
			{
				operate();
			}
			return 0;
		}
	}
	cout<<"No Parameters. (-h for help)"<<endl;
	return -1;
}