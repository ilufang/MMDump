#ifndef MMDUMP_H
#define MMDUMP_H

class MMOperate;

class MMDump
{
private:
	string str;
	bool incomplete;
	bool output;
	MMOperate* callback;
private:
	void processData(string data);
	bool isAnswer(string data);
	void getAnswer(string data);
public:
	void listen();
	MMDump();
	MMDump(MMOperate* op_callback);
};

#endif
