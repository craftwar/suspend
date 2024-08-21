#include <cstdlib>
#include <iostream>
#include <vector>

#include <windows.h>
#include <Psapi.h>
#include <powrprof.h>

// for _setmode
#include <fcntl.h>
#include <io.h>
// for _getch
#include <conio.h>

#define WSTRCMP_CONST(str, const_str) \
	wmemcmp(str, const_str, sizeof(const_str) / sizeof(*const_str))
typedef LONG(NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);

LONG NTAPI NoOperation(HANDLE ProcessHandle)
{
	return 0;
}

int wmain(int argc, wchar_t *__restrict argv[])
{
	const HMODULE hNtdll = GetModuleHandleA("ntdll");
	bool monitorOff = false;
	unsigned long monitorDelay = 0;
	bool suspend = false;

	wchar_t szProcessName[MAX_PATH];
	DWORD aProcesses[1024]; // change this to fit your use case
	DWORD cbNeeded;
	DWORD *Process_cur = aProcesses + 1; // skip System Idle Process

	enum class Mode : unsigned char { suspend, resume, transition } mode = Mode::suspend;
	NtSuspendProcess pfnOperation =
		reinterpret_cast<NtSuspendProcess>(GetProcAddress(hNtdll, "NtSuspendProcess"));
	//NtSuspendProcess pfnSuspend = reinterpret_cast<NtSuspendProcess>(
	//	GetProcAddress(hNtdll, "NtSuspendProcess"));
	NtSuspendProcess pfnResume =
		reinterpret_cast<NtSuspendProcess>(GetProcAddress(hNtdll, "NtResumeProcess"));
	std::vector<wchar_t *__restrict> nameList;
	nameList.reserve(argc - 1); // allocate max possible size to prevent reallocation(s)
	//std::vector<wchar_t *> nameList(argc); // cause some element filled with nullptr when there is any option argument
	std::vector<DWORD> suspendedList;

	_setmode(_fileno(stdout), _O_WTEXT);

	// Get the list of process identifiers.
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return 1;

	// Calculate how many process identifiers were returned.
	const DWORD cProcesses = cbNeeded / sizeof(DWORD);
	if (cbNeeded > sizeof(aProcesses)) {
		std::wcout << L"compile program with bigger aProcesses[] size\n";
		return 1;
	}

	/*
  {"n", "no operation"},
  { "r", "resume" },
  { "s", "case sensitive" },	// removed
  { "t", "suspend then resume after pressing any key" },
  { "p", "process id" },
  */

	// later arg override early ones
	{
		wchar_t **const end = argv + argc;
		for (wchar_t *__restrict *__restrict arg = argv + 1; arg < end; ++arg) {
			if (!WSTRCMP_CONST(*arg, L"-n")) {
				pfnOperation = NoOperation;
				pfnResume = NoOperation;
				std::wcout << L"no operation mode\n";
			} else if (!WSTRCMP_CONST(*arg, L"-t"))
				mode = Mode::transition;
			else if (!WSTRCMP_CONST(*arg, L"-r"))
				mode = Mode::resume;
			else if (!WSTRCMP_CONST(*arg, L"-m")) {
				monitorOff = true;
				wchar_t **const ptrDelay = arg + 1;
				if (ptrDelay < end)
					monitorDelay = std::wcstoul(*ptrDelay, nullptr, 10);
				if (monitorDelay)
					++arg;
			} else if (!WSTRCMP_CONST(*arg, L"-s"))
				suspend = true;
			else if (!WSTRCMP_CONST(*arg, L"-p")) {
				++arg;
				for (; arg < end; ++arg)
					suspendedList.push_back(std::wcstoul(*arg, nullptr, 10));
			} else
				nameList.push_back(*arg);
		}
	}

	if (mode == Mode::resume) {
		pfnOperation = pfnResume;
		std::wcout << L"resumed";
	} else {
		std::wcout << L"suspended";
	}
	std::wcout << L" process(es):\n";

	// handle pid mode target
	for (DWORD it : suspendedList) {
		HANDLE hProcess = OpenProcess(
			PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SUSPEND_RESUME, false, it);
		if (hProcess && !pfnOperation(hProcess)) {
			std::wcout << L"pid " << it << '\n';
			CloseHandle(hProcess);
		}
	}

	for (const DWORD *const Process_end = aProcesses + cProcesses; Process_cur < Process_end;
	     ++Process_cur) {
		HANDLE hProcess =
			OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SUSPEND_RESUME,
				    false, *Process_cur);
		if (hProcess && GetProcessImageFileNameW(hProcess, szProcessName, MAX_PATH)) {
			wchar_t *__restrict const exeName = wcsrchr(szProcessName, L'\\') + 1;
			for (wchar_t *__restrict name : nameList) {
				if (!wcscmp(exeName, name)) {
					if (!pfnOperation(hProcess)) {
						std::wcout << exeName << L'\n';
						if (mode == Mode::transition)
							suspendedList.push_back(*Process_cur);
					}
					break;
				}
			}
			CloseHandle(hProcess);
		}
	}

	// or use GetDesktopWindow() as target
	if (monitorOff) {
		if (monitorDelay)
			Sleep(monitorDelay);
		SendMessage(GetConsoleWindow(), WM_SYSCOMMAND, SC_MONITORPOWER, 2);
	}
	if (suspend)
		SetSuspendState(FALSE, FALSE, FALSE);

	// for performance, I save pid only
	// display pid is not much meaningful to suspened process name
	if (mode == Mode::transition) {
		std::wcout << L"Press any key to resume suspended process(es):\n";
		_getch();

		for (DWORD it : suspendedList) {
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION |
							      PROCESS_SUSPEND_RESUME,
						      false, it);
			if (hProcess) {
				pfnResume(hProcess);
				CloseHandle(hProcess);
			}
		}
	}

	return 0;
}
