#include <iostream>
#include <vector>
#include <windows.h>
#include "myLib/thread.h"
#include "myLib/inputUtils.h"
#include "myLib/event.h"


struct ThreadInfo
{
	int index;
	bool finished = false;
	myLib::Event BlockEvent, ContinueEvent, FinishEvent;
};

int* shared_array;
int array_size;
CRITICAL_SECTION shared_cs;
myLib::Event StartEvent;

const int SLEEPTIME = 5;

DWORD WINAPI marker(LPVOID lpParam)
{
	ThreadInfo* p = static_cast<ThreadInfo*>(lpParam);

	srand(p->index);

	StartEvent.wait(INFINITE);

	bool exitNow = false;
	while (true)
	{
		int index = rand() % array_size;
		EnterCriticalSection(&shared_cs);
		if (shared_array[index] == 0)
		{
			LeaveCriticalSection(&shared_cs);
			Sleep(SLEEPTIME);
			EnterCriticalSection(&shared_cs);
			shared_array[index] = p->index;
			LeaveCriticalSection(&shared_cs);
			Sleep(SLEEPTIME);
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
		p->BlockEvent.set();
		HANDLE events[2] = { p->ContinueEvent.native_handle(), p->FinishEvent.native_handle() };
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
			p->BlockEvent.reset();
			p->ContinueEvent.reset();
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

	std::vector<ThreadInfo> threadInfos;
	threadInfos.reserve(marker_num);

	std::vector<myLib::Thread> threads;

	InitializeCriticalSection(&shared_cs);

	for (size_t i = 0; i < marker_num; i++)
	{
		threadInfos[i].BlockEvent = myLib::Event(false, false);
		threadInfos[i].ContinueEvent = myLib::Event(false, false);
		threadInfos[i].FinishEvent = myLib::Event(false, false);

		threadInfos[i].index = i;

		myLib::Thread thread(marker, &threadInfos[i]);
		threads.push_back(thread);
	}

	StartEvent.set();

	int n = marker_num;
	while (n > 0)
	{
		std::vector<HANDLE> hToWait;
		for (size_t i = 0; i < marker_num; i++)
		{
			if (!threadInfos[i].finished)
			{
				hToWait.push_back(threadInfos[i].BlockEvent.native_handle());
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

		int MarkerToFinish;
		std::cout << "Enter marker to finish: ";
		while (true)
		{
			inputValue(MarkerToFinish);
			if (MarkerToFinish <= 0 || MarkerToFinish > n || threadInfos[MarkerToFinish].finished)
			{
				std::cout << "Invalid index, try again." << std::endl;
				continue;
			}
			else
			{
				break;
			}
		}

		threadInfos[MarkerToFinish].FinishEvent.set();
		threads[MarkerToFinish].join();
		threadInfos[MarkerToFinish].finished = true;
		n--;

		EnterCriticalSection(&shared_cs);
		std::cout << "Array: ";
		for (size_t i = 0; i < array_size; i++)
		{
			std::cout << shared_array[i] << ' ';
		}
		std::cout << std::endl;
		LeaveCriticalSection(&shared_cs);

		for (size_t i = 0; i < marker_num; i++)
		{
			if (!threadInfos[i].finished)
			{
				threadInfos[i].ContinueEvent.set();
			}
		}
	}
	DeleteCriticalSection(&shared_cs);

	delete[] shared_array;

	std::cout << "All markers finished. Exiting main." << std::endl;
	return 0;

}