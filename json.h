#ifndef JSON_H
#define JSON_H

class json
{
private:
	string data;
public:
	json(string jsonData);
	json();
	bool isJson();
	bool isHead();
};

#endif
