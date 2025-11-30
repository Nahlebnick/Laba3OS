#include "myLib/event.h"

myLib::Event::Event(bool bManualReset, bool bInitialState)
{
	hEvent = CreateEvent(NULL, bManualReset, bInitialState, NULL);
	if (!hEvent)
	{
		DWORD err = GetLastError();
		throw std::system_error(static_cast<int>(err), std::system_category(), "CreateEvent failed");
	}
}

void myLib::Event::set()
{
	SetEvent(hEvent);
}

void myLib::Event::reset()
{
	ResetEvent(hEvent);
}

void myLib::Event::wait(DWORD wait)
{
	if (hEvent)
	{
		DWORD res = WaitForSingleObject(hEvent, wait);
		switch (res)
		{
			case WAIT_OBJECT_0: break;
			case WAIT_TIMEOUT: throw std::runtime_error("Waiting time expired!"); break;
			case WAIT_FAILED: throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "WaitForSingleObject failed"); break;
			default: throw std::runtime_error("Unexpected result from WaitForSingleObject!"); break;
		}
	}
}

