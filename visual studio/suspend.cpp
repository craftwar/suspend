#include <iostream>
#include <vector>

#include <windows.h>
#include <Psapi.h>

// for _setmode
#include <fcntl.h>
#include <io.h>
// for _getch
#include <conio.h>

#define WSTRCMP_CONST(str, const_str) wmemcmp(str, const_str, sizeof(const_str) / sizeof(*str))
typedef LONG(NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);

LONG NTAPI NoOperation(HANDLE ProcessHandle)
{
	return 0;
}

int wmain(int argc, wchar_t *argv[])
{
	const HMODULE hNtdll = GetModuleHandleA("ntdll");

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
	std::vector<wchar_t *> nameList(argc);
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
  { "p", "process id (not implemented)" },
  */

	// later arg override early ones
	{
		wchar_t **const end = argv + argc;
		for (wchar_t **arg = argv + 1; arg < end; ++arg) {
			if (WSTRCMP_CONST(*arg, L"-n") == 0) {
				pfnOperation = NoOperation;
				pfnResume = NoOperation;
				std::wcout << L"no operation mode\n";
			} else if (WSTRCMP_CONST(*arg, L"-t") == 0)
				mode = Mode::transition;
			else if (WSTRCMP_CONST(*arg, L"-r") == 0)
				mode = Mode::resume;
			else
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

	for (DWORD *const Process_end = aProcesses + cProcesses; Process_cur < Process_end;
	     ++Process_cur) {
		HANDLE hProcess =
			OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SUSPEND_RESUME,
				    false, *Process_cur);
		if (hProcess) {
			if (GetProcessImageFileNameW(hProcess, szProcessName, MAX_PATH)) {
				wchar_t *const exeName = wcsrchr(szProcessName, L'\\') + 1;
				for (wchar_t *name : nameList) {
					if (wcscmp(exeName, name) == 0 && !pfnOperation(hProcess)) {

						std::wcout << exeName << L'\n';
						if (mode == Mode::transition)
							suspendedList.push_back(*Process_cur);

						break;
					}
				}
			}
			CloseHandle(hProcess);
		}
	}

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
