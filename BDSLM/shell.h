#pragma once
#include "pch.h"
using std::string;

class Shell
{
public:
	Shell(void);
	~Shell(void);

	bool RunProcess(const string& process);
	bool StopProcess(void);
	bool GetOutput(int timeout, string& outstr);//��ȡ����ַ���
	bool SetInput(const string& cmd);//ִ������
private:
	HANDLE m_hChildInputWrite;	//�����ض����ӽ�������ľ��
	HANDLE m_hChildInputRead;
	HANDLE m_hChildOutputWrite;	//�����ض����ӽ�������ľ��  
	HANDLE m_hChildOutputRead;
	PROCESS_INFORMATION m_cmdPI;//cmd������Ϣ
};
Shell::Shell(void)
{
	m_hChildInputWrite = NULL;
	m_hChildInputRead = NULL;
	m_hChildOutputWrite = NULL;
	m_hChildOutputRead = NULL;
	ZeroMemory(&m_cmdPI, sizeof(m_cmdPI));
}

Shell::~Shell(void)
{
	StopProcess();
}
bool Shell::RunProcess(const string& process)
{
	SECURITY_ATTRIBUTES   sa;
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	sa.nLength = sizeof(sa);

	//�����ӽ�����������ܵ� 
	if (FALSE == ::CreatePipe(&m_hChildOutputRead, &m_hChildOutputWrite, &sa, 0))
	{
		return false;
	}

	//�����ӽ������������ܵ�   
	if (FALSE == CreatePipe(&m_hChildInputRead, &m_hChildInputWrite, &sa, 0))
	{
		::CloseHandle(m_hChildOutputWrite);
		::CloseHandle(m_hChildOutputRead);
		return false;
	}

	ZeroMemory(&m_cmdPI, sizeof(m_cmdPI));
	STARTUPINFOA  si;
	GetStartupInfoA(&si);

	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = m_hChildInputRead;     //�ض����ӽ�������   
	si.hStdOutput = m_hChildOutputWrite;   //�ض����ӽ�������    
	si.hStdError = m_hChildOutputWrite;

	if (FALSE == ::CreateProcessA(NULL, (char*)process.c_str(), NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &m_cmdPI))
	{
		::CloseHandle(m_hChildInputWrite);
		::CloseHandle(m_hChildInputRead);
		::CloseHandle(m_hChildOutputWrite);
		::CloseHandle(m_hChildOutputRead);
		m_hChildInputWrite = NULL;
		m_hChildInputRead = NULL;
		m_hChildOutputWrite = NULL;
		m_hChildOutputRead = NULL;
		ZeroMemory(&m_cmdPI, sizeof(m_cmdPI));
		return false;
	}

	return true;
}
bool Shell::StopProcess(void)
{
	::CloseHandle(m_hChildInputWrite);
	::CloseHandle(m_hChildInputRead);
	::CloseHandle(m_hChildOutputWrite);
	::CloseHandle(m_hChildOutputRead);
	m_hChildInputWrite = NULL;
	m_hChildInputRead = NULL;
	m_hChildOutputWrite = NULL;
	m_hChildOutputRead = NULL;
	::TerminateProcess(m_cmdPI.hProcess, -1);
	::CloseHandle(m_cmdPI.hProcess);
	::CloseHandle(m_cmdPI.hThread);
	ZeroMemory(&m_cmdPI, sizeof(m_cmdPI));
	return true;
}
bool Shell::GetOutput(int timeout, string& outstr)
{
	if (NULL == m_hChildOutputRead)
	{
		return false;
	}

	outstr = "";
	char buffer[4096] = { 0 };
	DWORD readBytes = 0;
	while (timeout > 0)
	{
		//�Թܵ����ݽ��ж���������ɾ���ܵ�������ݣ����û�����ݣ�����������
		if (FALSE == PeekNamedPipe(m_hChildOutputRead, buffer, sizeof(buffer) - 1, &readBytes, 0, NULL))
		{
			return false;
		}

		//����Ƿ�������ݣ����û�����ݣ������ȴ�
		if (0 == readBytes)
		{
			timeout -= 1;
			continue;
		}

		readBytes = 0;
		if (::ReadFile(m_hChildOutputRead, buffer, sizeof(buffer) - 1, &readBytes, NULL))
		{
			outstr.insert(outstr.end(), buffer, buffer + readBytes);
			timeout -= 1;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool Shell::SetInput(const string& cmd)
{
	if (NULL == m_hChildInputWrite)
	{
		return "";
	}

	string tmp = cmd + "\r\n";
	DWORD writeBytes = 0;
	if (FALSE == ::WriteFile(m_hChildInputWrite, tmp.c_str(), tmp.size(), &writeBytes, NULL))
	{
		return false;
	}
	return true;
}