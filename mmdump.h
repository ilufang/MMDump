#ifndef MMDUMP_H
#define MMDUMP_H

class MMDump()
{
private:
	string data;
	bool incomplete;
private:
	void processData(string data);
	bool isAnswer(string data);
	void getAnswer(string data);
	void dump(json data);
public:
	void listen();
	MMDump();
};

#endif
