#include <QCoreApplication>
#include <QStringList>
#include <QCommandLineParser>
#include <QDebug>
#include <QTextStream>

#include <locale>
#include <iostream>

#ifdef WIN32
#define _UNICODE
#include <qt_windows.h>
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
    bool bResume, bNoOp;
    Qt::CaseSensitivity bCaseSensitivity;
    TCHAR szProcessName[MAX_PATH];
    _TCHAR* exeName;
    DWORD aProcesses[1024], cbNeeded, cProcesses, result;
    HANDLE hProcess;
    std::locale::global(std::locale(""));
//	std::locale::global(std::locale("en_US.UTF-8"));
//	SetConsoleCP(65001);
//	SetConsoleOutputCP(65001);

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
    bCaseSensitivity = parser.isSet("s")? Qt::CaseSensitive : Qt::CaseInsensitive;
//    bool bPID = parser.isSet(showProgressOption);

    QStringList nameList = parser.positionalArguments();


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

    if (bNoOp)
        std::wcout <<"no operation mode"<< endl;
    if (bResume)
    {
        std::wcout <<"resumed process(es):"<< endl;
    }
    else
    {
        std::wcout <<"suspended process(es):"<< endl;
    }
    for (unsigned int i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
        {
            hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SUSPEND_RESUME, false, aProcesses[i]);
            if (hProcess)
            {
                //BUG:  call GetProcessImageFileName() before QueryFullProcessImageName() or QueryFullProcessImageName() doesn't work properly
                //      That's why MSDN use strange exampe to get process name.
                result = GetProcessImageFileName(hProcess, szProcessName, MAX_PATH);
//                result =1;
                if ( result && QueryFullProcessImageName(hProcess, 0, szProcessName, &result) )
                {
                    exeName = _tcsrchr(szProcessName, '\\');
//                    std::wcout << "szProcessName: "<< szProcessName <<endl;
//                        std::wcout << "exeName: " << ++exeName << endl;
                    if ( exeName++ && nameList.contains( QString::fromWCharArray(exeName), bCaseSensitivity ))
                    {
//                        qDebug() << nameList << endl;
//                        wcout <<"find" << endl;
                        if (!bNoOp)
                        {
                            if (bResume)
                            {
                                if (!pfnNtResumeProcess(hProcess))
                                    std::wcout << exeName << endl;
                            }
                            else
                            {
                                if(!pfnNtSuspendProcess(hProcess))
                                    std::wcout << exeName << endl;
                            }
                        }
                        else
                            std::wcout << exeName << endl;
                    }
				}
            }
            CloseHandle(hProcess);
        }
    }


    a.exit();
//    return a.exec();
}
