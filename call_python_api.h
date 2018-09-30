#ifndef __ANDY_UTIL_CALL_PYTHON_API_H_
#define __ANDY_UTIL_CALL_PYTHON_API_H_

#include "Python.h"
#include <string>
#include <map>
#include <vector>

namespace andy{
namespace util{

class CCallPythonApi
{
public:
	CCallPythonApi();
	~CCallPythonApi();

    // 初始化python解释器
    int Initialize();

    /**
     * 执行python函数
     * @param sPath：模块目录
     * @param sModuleName：模块名称
     * @param sFuncName：函数名称
     * @param vsParam：入参
     * @param sRetParam：出参
    */
    int CallPython
    (
        const std::string &sPath,
        const std::string &sModuleName,
        const std::string &sFuncName,
		const std::vector<std::string> &vsParam,
		std::string &sRetParam
    );

    /**
     * 获取最后一次错误信息
     * */ 
    std::string GetErrMsg() const
    {
        return m_sLastErrMsg;
    }

private:
	/**
	* 加载python模块
	* */
	PyObject * ImportModule
	(
		const std::string &sModuleName,
		const std::string &sPath
	);

	/**
	* 加载python函数
	* */
	PyObject * ImportFunc
	(
		PyObject *pModule,
		const std::string &sModuleName,
		const std::string &sFuncName
	);

	/**
	* 转换C++参数为python参数
	* */
	PyObject * ConvertArgs(const std::vector<std::string> &vsParam);

	int SetPathEnv(const std::string &sPath);

	PyObject * DoImportModule(const std::string &sModuleName);



private:
    std::string m_sLastErrMsg;

    // 已经加载模块
    std::map<std::string, PyObject *> m_mapModuleImported;
};

}}
#endif // __UTIL_CALL_PYTHON_API_H_