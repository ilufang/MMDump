#include "forward.h"

#include <stack>


json::json(string jsonData): data(jsonData)
{}
json::json(): data()
{}

bool json::isHead()
{
	int json_index = data.find("json");
	int brace_index = data.find("{");
	if (json_index == string::npos || brace_index == string::npos)
	{
		return false;
	}
	if (json_index < brace_index)
	{
		return true;
	}
	return false;
}

bool json::isJson()
{
	stack<char> stack;
	bool empty = true;
	for(int i=0; i<data.size(); i++)
	{
		switch(data[i])
		{
		case '{':
			empty = false;
			stack.push('{');
			break;
		case '}':
			if (!stack.empty())
				if(stack.top()=='{')
				{
					stack.pop();
				} else
				{
					return false;
				}
			break;
		case '[':
			stack.push('[');
			break;
		case ']':
			if (!stack.empty())
				if(stack.top()=='[')
				{
					stack.pop();
				} else
				{
					return false;
				}
			break;
		case '"':
			if (!stack.empty())
				if(stack.top()=='"')
				{
					stack.pop();
				} else
				{
					stack.push('"');
				}
			break;
		default:
			break;
		}
	}

	if (empty)
		return false;
	if (stack.empty())
	{
		return true;
	} else
	{
		return false;
	}
}

