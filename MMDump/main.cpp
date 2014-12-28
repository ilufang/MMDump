//
//  main.cpp
//  MMDump
//
//  Created by ilufang on 12/27/14.
//  Copyright (c) 2014 ilufang. All rights reserved.
//

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <pcap/pcap.h>

#define SERVERIP_TXT "/data/data/com.ilufang.mmdans/files/serverip.txt"

using namespace std;

// Forward Declaration
void pcapCallback(unsigned char *arg);
void pcapListen(string ip);

// ip:220.181.56.102

int main(int argc, const char * argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1], "-ver")==0) {
			cout<<"1.0.0-alpha"<<endl;
			return EXIT_SUCCESS;
		}
		if (strcmp(argv[1], "-getip")==0)
		{
			fstream file;
			file.open(SERVERIP_TXT,ios::in);
			if (!file) {
				file.clear();
				file.open(SERVERIP_TXT,ios::in);
				if (!file) {
					cout<<"220.181.56.102(File Write Error)"<<endl;
					return EXIT_SUCCESS;
				}
				file<<"220.181.56.102"<<endl;
				file.close();
				cout<<"220.181.56.102"<<endl;
				return EXIT_SUCCESS;
			}
			string ip;
			file>>ip;
			cout<<ip<<endl;
			file.close();
			return EXIT_SUCCESS;
		}
		if (strcmp(argv[1], "-listen")==0) {
			string ip = "220.181.56.102";
			if (argc > 2) {
				// set from arg
				ip = argv[2];
				fstream file;
				file.open(SERVERIP_TXT,ios::out);
				file<<ip<<endl;
				file.close();
			} else
			{
				// set from file
				fstream file;
				file.open(SERVERIP_TXT, ios::in);
				if (file) {
					file>>ip;
					file.close();
				}
			}
			pcapListen(ip);
			return EXIT_SUCCESS;
		}
	}
    return EXIT_FAILURE;
}


void pcapCallback(unsigned char *arg)
{
	
}

void pcapListen(string ip)
{
	cout<<"TCP Listening Started on "<<ip<<endl;
}
