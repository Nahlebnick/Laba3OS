#include "myLib/thread.h"

myLib::Thread::Thread(LPTHREAD_START_ROUTINE proc, LPVOID param)
{
	hThread = CreateThread(NULL, 0, proc, param, 0, &IDThread);
	if (!hThread)
	{
		DWORD err = GetLastError();
		throw std::system_error(static_cast<int>(err), std::system_category(), "CreateThread failed");
	}
}

void myLib::Thread::join(DWORD wait)
{
	if (hThread)
	{
		DWORD res = WaitForSingleObject(hThread, wait);
		switch (res)
		{
		case WAIT_OBJECT_0: break;
		case WAIT_TIMEOUT: throw std::runtime_error("Waiting time expired!"); break;
		case WAIT_FAILED: throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "WaitForSingleObject failed"); break;
		default: throw std::runtime_error("Unexpected result from WaitForSingleObject!"); break;
		}
		close();
	}
}

void myLib::Thread::close()
{
	if (hThread)
	{
		bool res = CloseHandle(hThread);
		if (!res)
		{
			throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "CloseHandle failed");
		}
		hThread = nullptr;
	}
	IDThread = 0;
}