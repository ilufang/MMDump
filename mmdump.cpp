#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <iostream>

// For WinPcap
#define WPCAP
#define HAVE_REMOTE


#include <pcap/pcap.h>

#define LINE_LEN 16
#define DATA_OFFSET 66

using namespace std;

#include "dataparser.h" 

int main(int argc, char **argv)
{   
	cout<<"MMDump for Windows"<<endl<<endl
		<<"Ver.1.0.0-alpha"<<endl
		<<"切大亏(c)2014-2015 ilufang"<<endl;
	pcap_if_t *alldevs, *d;
	pcap_t *fp;
	u_int inum, i=0;
	char errbuf[PCAP_ERRBUF_SIZE];
	int res;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

    cout<<"\n网络设备列表:"<<endl;
    /* The user didn't provide a packet source: Retrieve the local device list */
    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
    {
        cerr<<"pcap_findalldevs_ex() 错误: "<<errbuf<<endl;
        return -1;
    }
    
    /* Print the list */
    for(d=alldevs; d; d=d->next)
    {
        printf("%d. %s\n    ", ++i, d->name);

        if (d->description)
            printf(" (%s)\n", d->description);
        else
            printf(" (No description available)\n");
    }
    
    if (i==0)
    {
        cerr<<"未找到网络设备（权限不足?）"<<endl; 
        return -1;
    }
    
    cout<<"请选择监听的网络设备(每个都试一遍就可以~):1-"<<i<<":";
	cout.flush(); 
    cin>>inum;
    
    if (inum < 1 || inum > i)
    {
        cerr<<"正常点!!~"<<endl;
        /* Free the device list */
        pcap_freealldevs(alldevs);
        return -1;
    }
    
    /* Jump to the selected adapter */
    for (d=alldevs, i=0; i< inum-1 ;d=d->next, i++);
    
    /* Open the device */
    if ( (fp= pcap_open(d->name,
                        4096 /*snaplen, enough for MMD*/,
                        PCAP_OPENFLAG_PROMISCUOUS /*flags*/,
                        -1 /*read timeout*/,
                        NULL /* remote authentication */,
                        errbuf)
                        ) == NULL)
    {
        cerr<<"监听失败:pcap_open()无法打开网络设备"<<endl;
        return -1;
    }
	
	// filtering optimization
	bpf_program filter;
	pcap_compile(fp, // the stream
				 &filter, // filter to compile to
				 "tcp", // filter string: tcp only
				 1,	// optimize expression
				 0); // Netmask (?)
	pcap_setfilter(fp, &filter);
	pcap_setdirection(fp, PCAP_D_IN); // Downloads only
	

	
    /* Read the packets */
    
    cout<<"开始尝试监听..."<<endl; 
    
    bool gotpack = false;
    
    while((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0)
    {

        if(res == 0)
            /* Timeout elapsed */
            continue;
		
		if (!gotpack)
		{
			cout<<"FIRST packet!!!"<<endl;
			gotpack = true;
		}        
        
		string data;
		for (int i=DATA_OFFSET; i<header->caplen; i++) {
			data+=pkt_data[i];
		}
		
		if (isJson(data)) {
			parseData(data);
		}
 
    }

    return 0;
}

