#include <forward.h>

#include <stack>

class json
{
private:
	string data;
public:
	json(string jsonData): data(jsonData)
	{}
	json(): data()
	{}

	bool isHead()
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

	bool isJson()
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
				if (stack.top()=='{')
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
				if (stack.top()=='[')
				{
					stack.pop();
				} else
				{
					return false;
				}
				break;
			case '"':
				if (stack.top()=='"')
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

	string data()
	{
		return data;
	}
}