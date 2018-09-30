#include "call_python_api.h"

namespace andy {
namespace util {

CCallPythonApi::CCallPythonApi()
{}

CCallPythonApi::~CCallPythonApi()
{
	// 释放模块资源
	for (auto itr = m_mapModuleImported.begin(); m_mapModuleImported.end() != itr; ++itr)
	{
		Py_DECREF(itr->second);
	}
}

int CCallPythonApi::Initialize()
{
	Py_Initialize();
	return 0;
}

int CCallPythonApi::CallPython
(
	const std::string &sPath,
	const std::string &sModuleName,
	const std::string &sFuncName,
	const std::vector<std::string> &vsParam,
	std::string &sRetParam
)
{
	if (sModuleName.empty() || sFuncName.empty())
	{
		m_sLastErrMsg = "module/func name is empty";
		return -1;
	}

	// 加载模块
	PyObject *pModule = ImportModule(sModuleName, sPath);
	if (NULL == pModule)
	{
		return -1;
	}

	
	// 加载函数
	PyObject *pFunc = ImportFunc(pModule, sModuleName, sFuncName);
	if (NULL == pFunc)
	{
		return -1;
	}

	// 参数转换
	PyObject *pArgs = ConvertArgs(vsParam);
	if (NULL == pArgs)
	{
		return -1;
	}

	//	执行函数
	PyObject * ppyobject = PyEval_CallObject(pFunc, pArgs);
	Py_DECREF(pFunc);
	Py_DECREF(pArgs);
	if (NULL == ppyobject)
	{
		m_sLastErrMsg = "pyeval_callobject failed";
		return -1;
	}

	// 获取返回参数
	sRetParam = PyString_AsString(ppyobject);

	return 0;
}

PyObject * CCallPythonApi::ImportModule
(
	const std::string &sModuleName,
	const std::string &sPath
)
{
	auto itr = m_mapModuleImported.find(sModuleName);
	if (m_mapModuleImported.end() != itr)
	{
		return itr->second;
	}

	if (0 != SetPathEnv(sPath))
	{
		return NULL;
	}

	PyObject *pModule = DoImportModule(sModuleName);
	if (NULL == pModule)
	{
		return NULL;
	}
	m_mapModuleImported[sModuleName] = pModule;
	return pModule;

}

PyObject * CCallPythonApi::ImportFunc
(
	PyObject *pModule,
	const std::string &sModuleName,
	const std::string &sFuncName
)
{
	PyObject *pFunc = PyObject_GetAttrString(pModule, sFuncName.c_str());
	if (NULL == pFunc || 0 == PyCallable_Check(pFunc))
	{
		m_sLastErrMsg = std::string("function [") + sFuncName + "] is not finded in  module [" + sModuleName + "]";
		if (NULL != pFunc)
		{
			Py_DECREF(pFunc);
		}
		return NULL;
	}
	return pFunc;
}

PyObject * CCallPythonApi::ConvertArgs(const std::vector<std::string> &vsParam)
{
	PyObject *pArgs = PyTuple_New(vsParam.size());
	int iIndex = 0;
	for (uint32_t i = 0; i < vsParam.size(); ++i)
	{
		PyObject *pParams = NULL;
		pParams = Py_BuildValue("s", vsParam[i].c_str());
		if (NULL != pParams)
		{
			PyTuple_SetItem(pArgs, iIndex++, pParams);
		}
		else
		{
			m_sLastErrMsg = "Py_BuildValue failed";
			return NULL;
		}
	}
	return pArgs;
}




int CCallPythonApi::SetPathEnv(const std::string &sPath)
{
	if(sPath.empty())
	{
		m_sLastErrMsg = "Path is empty";
		return -1;
	}

	PyRun_SimpleString("import sys");
	std::string sCmd = std::string("sys.path.append(\"") + sPath + "\")";
	int iRet = PyRun_SimpleString(sCmd.c_str());
	if (0 != iRet)
	{
		m_sLastErrMsg = "PyRun_SimpleString failed:" + sCmd;
		return -1;
	}
	return 0;
}

PyObject * CCallPythonApi::DoImportModule(const std::string &sModuleName)
{
	PyObject * pModule = PyImport_ImportModule(sModuleName.c_str());
	if (NULL == pModule)
	{
		m_sLastErrMsg = "Import module '" + sModuleName + "' failed";
	}

	return pModule;
}



}}