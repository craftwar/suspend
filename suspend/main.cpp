#ifdef WIN32
//#define WINVER 0x0601
//#define _WIN32_WINNT 0x0601
#ifndef _UNICODE
#define _UNICODE
#endif

#endif

#include <locale>
#include <iostream>

#ifdef WIN32
#include <qt_windows.h>

#include <WinNT.h>
#include <Psapi.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>

#endif

#include <QCoreApplication>
#include <QStringList>
#include <QCommandLineParser>
#include <QTextStream>

#ifdef QT_DEBUG
#include <QDebug>
#endif





typedef LONG (NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef LONG (NTAPI *NtResumeProcess)(IN HANDLE ProcessHandle);

//#define NAMESIZE 255

/*
 * CreateToolhelp32Snapshot
 * Process32First
    GetProcessImageFileName
*/


// not using std, wcout behavior will be strange
using namespace std;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HMODULE hNtdll = GetModuleHandle(TEXT("ntdll"));
    NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)
            GetProcAddress( hNtdll, "NtSuspendProcess");
    NtResumeProcess pfnNtResumeProcess = (NtResumeProcess)
            GetProcAddress( hNtdll, "NtResumeProcess");

    bool bResume, bNoOp;
    Qt::CaseSensitivity bCaseSensitivity;
    TCHAR szProcessName[MAX_PATH];
    TCHAR *exeName;
    DWORD aProcesses[1024]; // change this to fit your use case
    DWORD cbNeeded, cProcesses;
//    DWORD result;
    DWORD *Process_cur = aProcesses + 1; // skip System Idle Process
    DWORD *Process_end; //not included
    HANDLE hProcess;

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
    bResume = parser.isSet("r");
    bNoOp = parser.isSet("n");
    bCaseSensitivity = parser.isSet("s")? Qt::CaseSensitive
                                        : Qt::CaseInsensitive;
//    bool bPID = parser.isSet(showProgressOption);

    QStringList nameList = parser.positionalArguments();
#ifdef QT_DEBUG
	qDebug() << nameList << endl;
#endif


    // Get the list of process identifiers.
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return 1;

    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);
    if (cbNeeded > sizeof(aProcesses)) {
        std::wcout << L"compile program with bigger aProcesses[] size\n";
        return 1;
    }


    Process_end = aProcesses + cProcesses;

    if (bNoOp)
        std::wcout << L"no operation mode"<< endl;

    if (bResume)
        std::wcout << L"resumed process(es):"<< endl;
    else
        std::wcout << L"suspended process(es):"<< endl;

    for ( ; Process_cur< Process_end; ++Process_cur) {
        hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION |
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
                    if (!bNoOp) {
                        if (bResume) {
                            if (!pfnNtResumeProcess(hProcess))
                                std::wcout << exeName << endl;
                        } else if(!pfnNtSuspendProcess(hProcess))
                                std::wcout << exeName << endl;
                    } else
                        std::wcout << exeName << endl;
                }
			}

            CloseHandle(hProcess);
        }
    }


    a.exit();
//    return a.exec();
}
