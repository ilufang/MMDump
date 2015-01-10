#include "forward.h"
#include <pcap/pcap.h>

#define DATA_OFFSET 66
#define BUFFER_SIZE	4096


void MMDump::getAnswer(string data)
{
	long pos = 0;
	string ans = "";
	while (true)
	{
		pos = data.find("\"answer\":", pos);
		if (pos == string::npos)
			break;
		ans += data[pos + 9];
		pos++; // Prevent find loop
	}
	if (!ans.empty())
	{
		if (output)
		{
			cout << "Ans: " << ans << endl;
		} else
		{
			// Also Process image questions:
			pos=0;
			vector<bool> is_image;
			while(true)
			{
				pos = data.find("\"imageUrl\":", pos);
				if (pos == string::npos)
					break;
				if (data[pos+12]=='h') //..."Http:...
				{
					is_image.push_back(true);
				} else
				{
					is_image.push_back(false);
				}
				pos++;
			}
			callback->gotAnswer(ans, is_image);
		}
	}
}
bool MMDump::isAnswer(string data)
{
	// Decides whether the data is json AND is for mmd

	// Phase 1: judge whether this data is json
	// These conditions are fatal that it must be extrememly careful
	if (data.find(",")==string::npos)
		return false;
	if (data.find(":")==string::npos)
		return false;
	if (data.find("\"")==string::npos)
		return false;
	// Now the data is very likely to be in json format
	// We may take a little (ignorable?) more efforts to judge that precisely
	
	// Phase 2: judge whether this json is for MMD
	int match_count = 0;
	if (data.find("\"answer\"")!=string::npos)
		return true;
	if (data.find("\"contributer\"")!=string::npos)
		match_count++;
	if (data.find("\"contributerName\"")!=string::npos)
		match_count++;
	if (data.find("\"imageUrl\"")!=string::npos)
		match_count++;
	if (data.find("\"options\"")!=string::npos)
		match_count++;
	if (data.find("\"subjectId\"")!=string::npos)
		match_count++;
	if (data.find("\"title\"")!=string::npos)
		match_count++;
	if (data.find("\"topicId\"")!=string::npos)
		match_count++;
	if (data.find("\"topicName\"")!=string::npos)
		match_count++;
	if (match_count>3) {
		return true;
	}
	return false;

}

void MMDump::processData(string data)
{
	if (isAnswer(data))
	{
		if (incomplete)
		{
			if (json(data).isHead() && json(data+str).isJson())
			{
				getAnswer(data+str);
			} else if (json(str).isHead() && json(str+data).isJson())
			{
				getAnswer(str+data);
			} else
			{
				cout << "Packet matching failure"<<endl;
				fstream logfile;
				logfile.open("/sdcard/mmdump.log",ios::app);
				logfile<<(long)time(NULL)<<endl
					<<"Packet Matching Failure:"<<endl
					<<"[str]"<<endl<<str<<endl<<endl
					<<"[data]"<<endl<<data<<endl<<endl;
				logfile.close();
			}
			incomplete = false;
		} else
		{
			if (json(data).isJson())
			{
				// But this is not very likely
				getAnswer(data);
			} else
			{
				if (!json(data).isHead())
				{
					cout << "Packet part loss"<<endl;
					fstream logfile;
					logfile.open("/sdcard/mmdump.log",ios::app);
					logfile<<(long)time(NULL)<<endl
						<<"Packet Part Loss:"<<endl
						<<"[data]"<<endl<<data<<endl<<endl;
					logfile.close();
				}
				incomplete = true;
				str = data;
			}
		}
	}
}

MMDump::MMDump(): incomplete(false), output(true)
{}

MMDump::MMDump(MMOperate* op_callback): incomplete(false), output(false), callback(op_callback)
{}

void MMDump::listen()
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
			processData(data);
		}
	}
}


