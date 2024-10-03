#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <tchar.h>


BOOL GetProcessHandle(LPCWSTR processName, HANDLE* hProcess, DWORD* pID)
{
	DWORD pid = 0;
	HANDLE hP = NULL;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		printf("[ERROR] Invalid HANDLE to process snapshots [%d]\n", GetLastError());
		return FALSE;
	}

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);

	if (!Process32First(hSnapshot, &pe))
	{
		printf("[ERROR] Could not enumerate processes [%d]\n", GetLastError());
		CloseHandle(hSnapshot);
		return FALSE;
	}

	do {
		// ANSI 문자열을 유니코드 문자열로 변환
		WCHAR exeFileName[MAX_PATH]; //WCHAR는 유니코드 형식의 문자열 
		
		MultiByteToWideChar(CP_ACP, 0, pe.szExeFile, -1, exeFileName, MAX_PATH); 


		if (_wcsicmp(processName, exeFileName) == 0) //이 함수는 대소문자를 구분하지 않고 유니코드 문자열을 비교  찾고자하는 프로세스를 찾으면 0을 반환
		{
			pid = pe.th32ProcessID;
			printf("[!] Trying to open handle on %ls, on pid %d\n", processName, pid);


			hP = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);
			if (hP == NULL)  
			{
				printf("[X] Could not open handle on %d, continuing\n", pid);
			}
			else  //핸들이 성공적으로 열렸다는의미
			{
				printf("[+] Successfully got handle on %d\n", pid);
				*pID = pid;
				*hProcess = hP;
				CloseHandle(hSnapshot);
				return TRUE; //일치하는 프로세스를 찾으면 더이상 반복하지않고 함수를 종료
			}
		}
	} while (Process32Next(hSnapshot, &pe));

	CloseHandle(hSnapshot);
	return FALSE; //프로세스를 찾지 못하면 오류 메시지를 출력합니다.
}
int main()
{
	HANDLE hProcess; //windows시스템 자원을 참조하는데 사용되는 자료형 프로세스 핸들을 저장함
	DWORD pid; // 32비트 정수형 데이터타입 프로세스ID를 저장함
	LPWSTR targetProcess = L"explorer.exe";  // 타겟 프로세스 이름  유니코드 문자열을 가르킴

	if (GetProcessHandle(targetProcess, &hProcess, &pid)) { 
		printf("[+] Process handle opened successfully! PID: %d\n", pid);
		CloseHandle(hProcess);  // 핸들 닫기
	}
	else {
		printf("[X] Failed to find or open the process\n");
	}
	system("pause");
	return 0;
}
