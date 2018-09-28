#ifdef WIN32
//#define WINVER 0x0601
//#define _WIN32_WINNT 0x0601
#ifndef _UNICODE
#define _UNICODE
#endif

#endif

//#include <locale>
#include <iostream>

#ifdef WIN32
#include <qt_windows.h>

#include <Psapi.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>

#endif

#include <QCoreApplication>
#include <QStringList>
#include <QCommandLineParser>
#include <QTextStream>

#ifdef QT_DEBUG
#include <QDebug>
#endif





typedef LONG (NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
//typedef LONG (NTAPI *NtResumeProcess)(IN HANDLE ProcessHandle);

//#define NAMESIZE 255

/*
 * CreateToolhelp32Snapshot
 * Process32First
    GetProcessImageFileName
*/


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    const HMODULE hNtdll = GetModuleHandle(TEXT("ntdll"));

    TCHAR szProcessName[MAX_PATH];
    TCHAR *exeName;
    DWORD aProcesses[1024]; // change this to fit your use case
    DWORD cbNeeded;
//    DWORD result;
    DWORD *Process_cur = aProcesses + 1; // skip System Idle Process

// 1) set locale to fix wcout but not working
//    std::locale::global(std::locale(""));
//    std::locale::global(std::locale("Japan"));
//    std::locale::global(std::locale("LC_ALL=en_US.UTF-8"));
//	SetConsoleCP(65001);
//	SetConsoleOutputCP(65001);
// 2) _setmode range wtext > u16text?
//    _setmode(_fileno(stdout), _O_U16TEXT);

    _setmode(_fileno(stdout), _O_WTEXT);

    //TODO
    //help text and description
    //pid mode
    QCommandLineParser parser;
    parser.setApplicationDescription("suspends/resumes process(es)");
    parser.addHelpOption();
    parser.addOptions({
        {"n", "no operation"},
        {"r", "resume"},
        {"s", "case sensitive"},
        {"p", "process id (not implemented)"},
//        // A boolean option with multiple names (-f, --force)
//        {{"f", "force"},
//            QCoreApplication::translate("main", "Overwrite existing files.")},
//        // An option with a value
//        {{"t", "target-directory"},
//            QCoreApplication::translate("main", "Copy all source files into <directory>."),
//            QCoreApplication::translate("main", "directory")},
    });
//    bool parseResult = parser.parse(a.arguments());
    parser.process(a);
    const bool bResume = parser.isSet("r");
    const bool bNoOp = parser.isSet("n");
    const Qt::CaseSensitivity bCaseSensitivity = parser.isSet("s")
                ? Qt::CaseSensitive
                : Qt::CaseInsensitive;

    const NtSuspendProcess pfnOperation = bResume
                ? (NtSuspendProcess)GetProcAddress(hNtdll, "NtResumeProcess")
                : (NtSuspendProcess)GetProcAddress(hNtdll, "NtSuspendProcess");

//    bool bPID = parser.isSet(showProgressOption);

    QStringList nameList = parser.positionalArguments();
#ifdef QT_DEBUG
    qDebug() << nameList << "\n";
#endif


    // Get the list of process identifiers.
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return 1;

    // Calculate how many process identifiers were returned.
    const DWORD cProcesses = cbNeeded / sizeof(DWORD);
    if (cbNeeded > sizeof(aProcesses)) {
        std::wcout << L"compile program with bigger aProcesses[] size\n";
        return 1;
    }
	
    if (bNoOp)
        std::wcout << L"no operation mode\n";

    std::wcout << (bResume ? L"resumed" : L"suspended")
               << L" process(es):\n";

    for (DWORD * const Process_end = aProcesses + cProcesses; Process_cur< Process_end; ++Process_cur) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION |
                               PROCESS_SUSPEND_RESUME, false, *Process_cur);
        if (hProcess) {
            //BUG:  call GetProcessImageFileName() before QueryFullProcessImageName() or QueryFullProcessImageName() doesn't work properly
            //      That's why MSDN use strange exampe to get process name.
//            result = GetProcessImageFileName(hProcess, szProcessName, MAX_PATH);
//            if ( result && QueryFullProcessImageName(hProcess, 0, szProcessName, &result) )

            // use GetProcessImageFileName only (don't get full file path, use ImageFileName only)
            if ( GetProcessImageFileName(hProcess, szProcessName, MAX_PATH) ) {
                exeName = _tcsrchr(szProcessName, L'\\') + 1;
                if (exeName && nameList.contains(
                            QString::fromWCharArray(exeName), bCaseSensitivity )) {
                    if (bNoOp || !pfnOperation(hProcess))
                        std::wcout << exeName << L'\n';
                }
			}

            CloseHandle(hProcess);
        }
    }


    a.exit();
//    return a.exec();
}
