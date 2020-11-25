#include "mystring.h"

using namespace std;
///////////////////////////////////////////////////////////////////// 		
// Class��		
//		split
// Args:
//		const string& str
//		char ch
// Description:                                                       		
//		��ch�������ַ���
/////////////////////////////////////////////////////////////////////
vector<string> split(const string& str, char ch)	//�Կո��ַ���Ϊ�ָ�������ÿһ����Ǻŷֿ�
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