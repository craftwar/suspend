#include <QCoreApplication>
#include <QStringList>
#include <QCommandLineParser>
#include <QDebug>
#include <QTextStream>

#include <iostream>

#ifdef WIN32
#include <qt_windows.h>
#define _UNICODE
#include <WinNT.h>
#include <Psapi.h>
#include <tchar.h>
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

    NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress( GetModuleHandle( TEXT("ntdll") ), "NtSuspendProcess");
    NtResumeProcess pfnNtResumeProcess = (NtResumeProcess)GetProcAddress( GetModuleHandle( TEXT("ntdll") ), "NtResumeProcess");
    bool bResume = false;
    TCHAR szProcessName[MAX_PATH];
    _TCHAR* exeName;
    DWORD aProcesses[1024], cbNeeded, cProcesses, result;
    HANDLE hProcess;

    //TODO
    QCommandLineParser parser;
    parser.setApplicationDescription("Test helper");
    parser.addHelpOption();
    parser.addOptions({
        // A boolean option with a single name (-p)
        {"p", "process id"},
//        // A boolean option with multiple names (-f, --force)
//        {{"f", "force"},
//            QCoreApplication::translate("main", "Overwrite existing files.")},
//        // An option with a value
//        {{"t", "target-directory"},
//            QCoreApplication::translate("main", "Copy all source files into <directory>."),
//            QCoreApplication::translate("main", "directory")},
    });

    QStringList nameList = a.arguments();
    nameList.removeFirst();
    if ( !nameList.empty() && !nameList.at(0).compare("-r") )
    {
        nameList.removeFirst();
        bResume = true;
    }


    // Get the list of process identifiers.
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        return 1;
    }

    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);
    if (cbNeeded > sizeof(aProcesses))
    {
        std::wcout <<" need more size to save process identifiers\n";
        return 1;
    }

    // Print the name and process identifier for each process.


////****

//hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION |PROCESS_SUSPEND_RESUME, false, 4792);
////hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |PROCESS_SUSPEND_RESUME |PROCESS_VM_READ, false, 4792);




//result = GetProcessImageFileName(hProcess, szProcessName, MAX_PATH);
//std::wcout << "result: " << result << endl;
//std::wcout << szProcessName << endl;


//if (hProcess &&
//        QueryFullProcessImageName(hProcess, 0, szProcessName, &result)
//         )
//{

//    std::wcout << "result: " << result << endl;
//    std::wcout << szProcessName << endl;
//    exeName = _tcsrchr(szProcessName, '\\');
//    //int index = strProcessName.lastIndexOf('\\');
//    std::wcout << "exeName: " << ++exeName << endl;
//    if ( nameList.contains( QString::fromWCharArray(exeName) ))
//    {
//        std::wcout <<"find" << endl;
//    }
//    else
//        std::wcout <<"not find" << endl;
//}
//else
//{
//    std::wcout << "result: " <<result <<endl;
//    std::wcout << "lasterror: " <<GetLastError() <<endl;
//}
////qDebug() << "debug test: "<< szProcessName <<endl;

//if (NULL != hProcess )
//{
//    HMODULE hMod;
//    DWORD cbNeeded;

//    if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod),
//         &cbNeeded) )
//    {
//        GetModuleBaseName( hProcess, hMod, szProcessName,
//                           sizeof(szProcessName)/sizeof(TCHAR) );
//    }
//    std::wcout <<"szProcessName from GetModuleBaseName: " <<szProcessName << endl;
//}
//CloseHandle(hProcess);

//****


    for (unsigned int i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
        {
            hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION |PROCESS_VM_READ | PROCESS_SUSPEND_RESUME, false, aProcesses[i]);
            if (hProcess)
            {
                //BUG:  call GetProcessImageFileName() before QueryFullProcessImageName() or QueryFullProcessImageName() doesn't work properly
                //      That's why MSDN use strange exampe to get process name.
                result = GetProcessImageFileName(hProcess, szProcessName, MAX_PATH);
                if ( result && QueryFullProcessImageName(hProcess, 0, szProcessName, &result) )
                {
                    exeName = _tcsrchr(szProcessName, '\\');
                    szProcessName[result] = TEXT('\0');
//                    std::wcout << "szProcessName: "<< szProcessName <<endl;
//                        std::wcout << "exeName: " << ++exeName << endl;
                    if ( exeName++ && nameList.contains( QString::fromWCharArray(exeName) ))
                    {
//                        qDebug() << nameList << endl;
//                        wcout <<"find" << endl;
                        if (bResume)
                        {
                            if (!pfnNtResumeProcess(hProcess))
                                std::wcout <<"resume process " << exeName << endl;
                        }
                        else
                        {
                            if(!pfnNtSuspendProcess(hProcess))
                                std::wcout <<"suspend process " << exeName << endl;

                        }
                    }
				}
            }
            CloseHandle(hProcess);
        }
    }


    a.exit();
//    return a.exec();
}
