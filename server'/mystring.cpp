#include "mystring.h"

using namespace std;
///////////////////////////////////////////////////////////////////// 		
// Class：		
//		split
// Args:
//		const string& str
//		char ch
// Description:                                                       		
//		用ch来划分字符串
/////////////////////////////////////////////////////////////////////
vector<string> split(const string& str, char ch)	//以空格字符作为分隔符，将每一个标记号分开
{
	vector<string> result;
	string t;
	for (auto c : str)
	{
		if (c != ch)
		{
			t += c;
		}
		else
		{
			result.push_back(t);
			t = "";
		}
	}
	result.push_back(t);
	return result;
}