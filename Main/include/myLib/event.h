#include <iostream>
#include <functional>
#include <windows.h>
#include <format>

namespace myLib
{
	class Event
	{
		HANDLE hEvent;
	public:
		Event() = default;

		Event(bool manualReset, bool bInitialState);

		~Event() { if (hEvent) CloseHandle(hEvent); }
	
		void set();

		void reset();

		void wait(DWORD wait);
		HANDLE native_handle() const noexcept { return hEvent; }
	};
}