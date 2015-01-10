#include "forward.h"
#include <pcap/pcap.h>

class MMDump
{
private:
	string str;
	bool incomplete;
	bool output;
	MMOperate& callback;
private:
	void getAnswer(string data)
	{
		long pos = 0;
		string ans = "";
		while (true)
		{
			pos = str.find("\"answer\":", pos);
			if (pos == string::npos)
				break;
			ans += str[pos + 9];
			pos++; // Prevent find loop
		}
		if (!ans.empty())
		{
			if (output)
			{
				cout << "Ans: " << ans << endl;
			} else
			{
				//callback.gotAnswer(ans);
			}
		}
	}
	bool isAnswer(string data)
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

	void processData(string data)
	{
		if (isAnswer(data))
		{
			if (incomplete)
			{
				if (json(data).isHead())
				{
					getAnswer(data+str);
				} else if (json(str).isHead())
				{
					getAnswer(str+data);
				} else
				{
					cout << "Packet matching failure, clearing..."<<endl;
					fstream logfile;
					logfile.open("/sdcard/mmdump.log",ios::app);
					logfile<<time()<<endl
						<<"Packet Matching Failure:"<<endl
						<<"[str]"<<endl<<str<<endl<<endl
						<<"[data]"<<endl<<data<<endl<<endl;
					logfile.close();
				}
			} else
			{
				if (json(data).isJson())
				{
					getAnswer(data);
				} else
				{
					incomplete = true;
					str = data;
				}
			}
		}
	}
public:
	MMDump(): incomplete(false), output(true)
	{}

	MMDump(MMOperate& op_callback): incomplete(false), output(true), callback(op_callback)
	{}

	void listen()
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

};
