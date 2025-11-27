#include <iostream>
#include <vector>
#include <windows.h>
#include "myLib/thread.h"
#include "myLib/inputUtils.h"

struct Events
{
	HANDLE hBlockEvent = NULL;
	HANDLE hContinueEvent = NULL;
	HANDLE hFinishEvent = NULL;
};

struct param
{
	int index;
	Events e;
};

int* shared_array;
int array_size;
CRITICAL_SECTION shared_cs;
HANDLE hStartEvent = NULL;


DWORD WINAPI marker(LPVOID lpParam)
{
	param* p = static_cast<param*>(lpParam);

	srand(p->index);

	WaitForSingleObject(hStartEvent, INFINITE);

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
		SetEvent(p->e.hBlockEvent);
		HANDLE events[2] = { p->e.hContinueEvent, p->e.hFinishEvent };
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
			ResetEvent(p->e.hBlockEvent);
			ResetEvent(p->e.hContinueEvent);
		}
	}

	return 0;
}

int main ()
{
	std::cout << "Enter array size: ";
	inputValue(array_size);

	if (array_size <= 0)
	{
		throw std::runtime_error("Invalid array size.");
	}

	shared_array = new int[array_size];
	for (int i = 0; i < array_size; i++)
	{
		shared_array[i] = 0;
	}

	int marker_num;
	inputValue(marker_num);

	if (marker_num <= 0 || marker_num > 1000)
	{
		throw std::runtime_error("Invalid number of marker! [1, 1000].");
	}

	std::vector<myLib::Thread> threads;
	threads.reserve(marker_num);

	InitializeCriticalSection(&shared_cs);

	std::vector<param> params(marker_num);


	for (size_t i = 0; i < marker_num; i++)
	{
		params[i].e.hBlockEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		params[i].e.hFinishEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		params[i].e.hContinueEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		params[i].index = i;

		if (!params[i].e.hBlockEvent || !params[i].e.hContinueEvent || !params[i].e.hFinishEvent)
		{
			throw std::runtime_error("Failed to create event!");	
		}

		threads.push_back(myLib::Thread(marker, &params[i]));
	}

	SetEvent(hStartEvent);

	int n = marker_num;
	while (n > 0)
	{
		std::vector<HANDLE> hToWait;
		for (size_t i = 0; i < marker_num; i++)
		{
			DWORD w = WaitForSingleObject(params[i].e.hFinishEvent, 0);
			if (w == WAIT_TIMEOUT)
			{
				hToWait.push_back(params[i].e.hFinishEvent);
			}
		}

		WaitForMultipleObjects(hToWait.size(), hToWait.data(), true, INFINITE);

		EnterCriticalSection(&shared_cs);
		std::cout << "Array: ";
		for (size_t i = 0; i < array_size; i++)
		{
			std::cout << shared_array[i] << ' ';
		}
		std::cout << std::endl;
		LeaveCriticalSection(&shared_cs);

		int MarkerToFinish
	}

}