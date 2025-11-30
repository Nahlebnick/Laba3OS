#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <functional>
#include <windows.h>
#include <format>

namespace myLib {
class Thread
{
	HANDLE hThread;
	DWORD IDThread;
public:
	Thread() = default;

	Thread(LPTHREAD_START_ROUTINE proc, LPVOID param = nullptr);

	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

	Thread(Thread&& other) = default;
	~Thread() noexcept { close(); }
	
	void join(DWORD wait=INFINITE);

	DWORD get_id() const noexcept { return IDThread; }
	HANDLE native_handle() const noexcept { return hThread; }

private:
	void close();
};

}
#endif // PROCESS_H