#include <iostream>
#include <windows.h>
#include "myLib/thread.h"
#include "myLib/inputUtils.h"

struct Events
{
	HANDLE hStartEvent = NULL;
	HANDLE hBlockEvent = NULL;
	HANDLE hContinueEvent = NULL;
	HANDLE hFinishEvent = NULL;
};

struct param
{
	int index;
};

int* shared_array;
int array_size;
CRITICAL_SECTION shared_cs;


DWORD WINAPI marker(LPVOID lpParam)
{
	param* p = static_cast<param*>(lpParam);

	Events e;
	srand(p->index);

	WaitForSingleObject(e.hStartEvent, INFINITE);

	srand(p->index);

	while (true)
	{
		int index = rand() % array_size;
		EnterCriticalSection(&shared_cs);
		if (shared_array[index] == 0)
		{
			LeaveCriticalSection(&shared_cs);
			Sleep(5);
			EnterCriticalSection(&shared_cs);
			shared_array[index] = p->index;
			LeaveCriticalSection(&shared_cs);
			Sleep(5);
			continue;
		}

		int count = 0;
		for (int i = 0; i < array_size; i++)
		{
			if (shared_array[i] == p->index)
			{
				count++;
			}
		}
		LeaveCriticalSection(&shared_cs);

		std::cout << "Index of thread: " << p->index << "\nNumber of marked elements: " << count << "\nIndex of element blocked: " << index << std::endl;
		SetEvent(e.hBlockEvent);
		HANDLE events[2] = { e.hContinueEvent, e.hFinishEvent };
		DWORD res = WaitForMultipleObjects(2, events, FALSE, INFINITE);

		if (res == WAIT_OBJECT_0 + 1)
		{
			EnterCriticalSection(&shared_cs);
			for (int i = 0; i < array_size; ++i)
			{
				if (shared_array[i] == p->index)
				{
					shared_array[i] = 0;
				}									
			}				
			LeaveCriticalSection(&shared_cs);
			break;
		}
		else
		{
			ResetEvent(e.hBlockEvent);
			ResetEvent(e.hContinueEvent);
		}
	}

	return 0;
}

int main ()
{
	std::cout << "Enter array size: ";
	inputValue(array_size);

	if (g_size <= 0)
		throw std::runtime_error("Invalid array size.");
}