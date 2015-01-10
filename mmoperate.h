#ifndef MMOPERATE_H
#define MMOPERATE_H

class MMOperate
{
private:
	int accuracy;
	MMDump dump;
	int cx, cy;
	int count;
private:
	bool selectAnswer(int answer, bool isImage);
	void tap(int x, int y);
	bool dismissAd();
	bool startSession();
public:
	MMOperate(int success_rate);
	void start();
	void gotAnswer(string ans, vector<bool> is_image);
};

#endif