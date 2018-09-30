#include "call_python_api.h"
#include <vector>
#include <string>
#include <iostream>

int main()
{
	andy::util::CCallPythonApi oApi;
	oApi.Initialize();
	
	std::vector<std::string> vsParam{"1", "2"};
	std::string sRetParam = "";
	int iRet = oApi.CallPython("D:\\code\\cpp_call_python", "get_sum", "get_sum", vsParam, sRetParam);
	if (iRet)
	{
		cout << "CallPython failed: " << oApi.GetErrMsg() << endl;
	}

	cout << "CallPython success, retParam:" << sRetParam << endl;
	system("pause");
}