// 
// mmdump.cpp
// MMDump
// 
// Created by ilufang on 12/27/14.
// Copyright (c) 2014 ilufang. All rights reserved.
// 

#include <csignal>
#include <iostream>
#include <string>
#include <pcap/pcap.h>

using namespace std;

#define DATA_OFFSET 66
#define BUFFER_SIZE	4096

// Forward Declarations
void pcapListen();
void parseData(string str);
void parseJson(string str);
bool dataComplete(string str);
bool isJson(string str);



void terminate(int n)
{
	// Display a message to be shown as toast to indicate termination success
	cout<<"Process Terminated."<<endl;
	exit(0);
}

int main(int argc, const char *argv[])
{
	if (argc > 1)
	{
		if (strcmp(argv[1], "-ver") == 0)
		{
			cout << "1.0.0-beta" << endl;
			return 0;
		}
		if (strcmp(argv[1], "-listen") == 0)
		{
			// register termination signals
			signal(SIGINT, terminate); // Ctrl-C
			signal(SIGKILL, terminate); // kill()
			signal(SIGHUP, terminate); // Terminal
			signal(SIGTERM, terminate); // # busybox killall
			signal(SIGSTOP, terminate); // Ctrl-Z or debugger
			pcapListen();
			return 0;
		}
	}
	return 1;
}

string data;
bool incomplete = false;

void parseData(string str)
{
	// Parse the json data packet
	// WARNING: data passed to this function must be a valid MMD Json
	if (incomplete)
	{
		// While there is chance that some mismatch may happen
		// 2 packets is usually the limit
		// Simply parse for answers
		parseJson(data+str);
		incomplete = false;
	}
	else
	{
		if (dataComplete(str))
		{
			parseJson(str);
		}
		else
		{
			// The packet is usually divided into 2 packets
			// Concate to increases recognition accuracy
			data = str;
		}
	}
}

void parseJson(string str)
{
	// Parse a complete json for answer
	// Print if found
	
	long pos = 0;
	string ans = "Ans:";
	while (true)
	{
		pos = str.find("\"answer\":", pos);
		if (pos == string::npos)
			break;
		ans += str[pos + 9];
		pos++; // Prevent find loop
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
		return true;
	}
	incomplete = true;
	return false;
}

bool isJson(string str)
{
	// Decides whether the data is json AND is for mmd
	
	// Phase 1: judge whether this data is json
	// These conditions are fatal that it must be extrememly careful
	if (str.find(",")==string::npos)
		return false;
	if (str.find(":")==string::npos)
		return false;
	if (str.find("\"")==string::npos)
		return false;
	// Now the data is very likely to be in json format
	// We may take a little (ignorable?) more efforts to judge that precisely
	
	// Phase 2: judge whether this json is for MMD
	int match_count = 0;
	if (str.find("\"answer\"")!=string::npos)
		return true;
	if (str.find("\"contributer\"")!=string::npos)
		match_count++;
	if (str.find("\"contributerName\"")!=string::npos)
		match_count++;
	if (str.find("\"imageUrl\"")!=string::npos)
		match_count++;
	if (str.find("\"options\"")!=string::npos)
		match_count++;
	if (str.find("\"subjectId\"")!=string::npos)
		match_count++;
	if (str.find("\"title\"")!=string::npos)
		match_count++;
	if (str.find("\"topicId\"")!=string::npos)
		match_count++;
	if (str.find("\"topicName\"")!=string::npos)
		match_count++;
	if (match_count>3) {
		return true;
	}
	return false;
}

void pcapListen()
{
	// Main Packet Capturing & Processing function
	
	pcap_t *pcap;
	char* dev;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_pkthdr *header;
	
	// Lookup device to listen
	dev = pcap_lookupdev(errbuf);
	if (dev==NULL) {
		cout<<"pcap_lookupdev error:"<<errbuf<<endl;
		return;
	}
	// Begin init pcap handler
	pcap = pcap_open_live(dev,
						  BUFFER_SIZE, // buffer size, 4096 enough for MMD
						  1, // promisc mode
						  -1, // don't time out
						  errbuf);
	
	// filtering optimization
	bpf_program filter;
	pcap_compile(pcap, // the stream
				 &filter, // filter to compile to
				 "tcp", // filter string: tcp only
				 1,	// optimize expression
				 0); // Netmask (?)
	pcap_setfilter(pcap, &filter);
	pcap_setdirection(pcap, PCAP_D_IN); // Downloads only
	
	cout << "TCP Listening Started on " << dev << endl;
	
	// Constantly constantly sniff for packets
	while (true) {
		const u_char* packet;
		int err = pcap_next_ex(pcap, &header, &packet);
		if (err==1 && header->caplen > DATA_OFFSET)
		{
			// Process that!
			string data;
			for (int i=DATA_OFFSET; i<header->caplen; i++) {
				data+=packet[i];
			}
			if (isJson(data)) {
				parseData(data);
			}
		}
	}
}