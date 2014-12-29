// 
// mmdump.cpp
// MMDump
// 
// Created by ilufang on 12/27/14.
// Copyright (c) 2014 ilufang. All rights reserved.
// 

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <pcap/pcap.h>

#define SERVERIP_TXT "/data/data/com.ilufang.mmdans/files/serverip.txt"


using namespace std;

// Forward Declaration
void pcapListen(string ip);

void parseData(string str);
void parseJson(string str);
bool dataComplete(string str);
bool isJson(string str);


// ip:220.181.56.102

int main(int argc, const char *argv[])
{
	if (argc > 1)
	{
		if (strcmp(argv[1], "-ver") == 0)
		{
			cout << "1.0.0-alpha" << endl;
			return EXIT_SUCCESS;
		}
		if (strcmp(argv[1], "-getip") == 0)
		{
			fstream file;
			file.open(SERVERIP_TXT, ios::in);
			if (!file)
			{
				file.clear();
				file.open(SERVERIP_TXT, ios::in);
				if (!file)
				{
					cout << "220.181.56.102 (File Write Error)" << endl;
					return EXIT_SUCCESS;
				}
				file << "220.181.56.102" << endl;
				file.close();
				cout << "220.181.56.102" << endl;
				return EXIT_SUCCESS;
			}
			string ip;
			file >> ip;
			cout << ip << endl;
			file.close();
			return EXIT_SUCCESS;
		}
		if (strcmp(argv[1], "-listen") == 0)
		{
			string ip = "220.181.56.102";
			if (argc > 2)
			{
				// set from arg
				ip = argv[2];
				fstream file;
				file.open(SERVERIP_TXT, ios::out);
				file << ip << endl;
				file.close();
			}
			else
			{
				// set from file
				fstream file;
				file.open(SERVERIP_TXT, ios::in);
				if (file)
				{
					file >> ip;
					file.close();
				}
			}
			pcapListen(ip);
			return EXIT_SUCCESS;
		}
	}
	return EXIT_FAILURE;
}

string data;
bool incomplete = false;

void parseData(string str)
{
	cout<<"Parsing data:"<<str<<endl;
	if (incomplete)
	{
		data += str;
		if (dataComplete(data))
		{
			parseJson(data);
		}
	}
	else
	{
		if (dataComplete(str))
		{
			parseJson(str);
		}
		else
		{
			data = str;
		}
	}
}

void parseJson(string str)
{
	// Parse a complete json for answer
	// Print if found
	cout<<"Parsing json:"<<str<<endl;
	long pos = 0;
	string ans = "Ans:";
	while (true)
	{
		pos = str.find("\"answer\":", pos);
		if (pos == string::npos)
			break;
		ans += str[pos + 9];
		pos++;					// Prevent find loop
	}
	if (ans.length() > 4)
	{
		cout << ans << endl;
	}
}

bool dataComplete(string str)
{
	// Checks if a json string is complete
	// Updates global variable
	int lbrace = 0, rbrace = 0;
	for (int i = 0; i < str.length(); i++)
	{
		switch (str[i])
		{
		case '{':
			lbrace++;
			break;
		case '}':
			rbrace++;
			break;
		}

	}
	if (lbrace == rbrace)
	{
		incomplete = false;
		cout<<"complete!"<<endl;
		return true;
	}
	cout<<"Imcomplete~"<<endl;
	incomplete = true;
	return false;
}

bool isJson(string str)
{
	bool lbrace = false, rbrace = false, colon = false, comma = false;
	for (int i = 0; i < str.length(); i++)
	{
		switch (str[i])
		{
		case ',':
			comma = true;
			break;
		case ':':
			colon = true;
			break;
		case '{':
			lbrace = true;
			break;
		case '}':
			rbrace = true;
			break;
		}
	}
	if (colon & comma & (lbrace || rbrace))
		return true;
	return false;
}

void pcapListen(string ip)
{
	pcap_t *pcap;
	char* dev;
	char errbuf[PCAP_ERRBUF_SIZE];
	string filter = "tcp";
	pcap_pkthdr *header;
	dev = pcap_lookupdev(errbuf);
	if (dev==NULL) {
		cout<<"pcap_lookupdev error:"<<errbuf<<endl;
		return;
	}
	// Begin init pcap handler
	pcap = pcap_open_live(dev,
						  BUFSIZ,
						  1, // promisc mode
						  -1, // don't time out
						  errbuf);
	// filter
	// pcap_setdirection(pcap, PCAP_D_IN); // Downloads only
	
	cout << "TCP Listening Started on " << dev << endl;
	while (true) {
		const u_char* packet;
		int err = pcap_next_ex(pcap, &header, &packet);
		if (err==1)
		{
			// Process that!
			string data;
			for (int i=0; i<header->len; i++) {
				data+=packet[i];
			}
			if (isJson(data)) {
				parseData(data);
			}
		}
	}
}