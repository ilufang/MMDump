#include <string>
#include <iostream>

using namespace std;

#include "dataparser.h"

bool incomplete = false;
string data;

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


