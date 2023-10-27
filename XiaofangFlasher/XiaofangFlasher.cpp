// XiaofangFlasher.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <afxdlgs.h>
#include <iostream>
#include <atlbase.h>
#include <Windows.h>
#include <devguid.h>
#include <setupapi.h>
#include <Dbt.h>
#include "resource.h"

#pragma comment(lib, "setupapi.lib")

bool is64BitOS() {//判断是否为64位系统
	SYSTEM_INFO cur_system_info;
	GetNativeSystemInfo(&cur_system_info);
	WORD system_str = cur_system_info.wProcessorArchitecture;
	if (system_str == PROCESSOR_ARCHITECTURE_IA64 || system_str == PROCESSOR_ARCHITECTURE_AMD64)
	{
		return true;
	}
	return false;
}

BOOL FreeResFile(DWORD dwResName, LPCSTR lpResType, LPCSTR lpFilePathName) {
	HMODULE hInstance = ::GetModuleHandle(NULL);//得到自身实例句柄  

	HRSRC hResID = ::FindResource(hInstance, MAKEINTRESOURCE(dwResName), lpResType);//查找资源  
	HGLOBAL hRes = ::LoadResource(hInstance, hResID);//加载资源  
	LPVOID pRes = ::LockResource(hRes);//锁定资源  

	if (pRes == NULL)//锁定失败  
	{
		return FALSE;
	}
	DWORD dwResSize = ::SizeofResource(hInstance, hResID);//得到待释放资源文件大小  
	HANDLE hResFile = CreateFile(lpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);//创建文件  

	if (INVALID_HANDLE_VALUE == hResFile)
	{
		//TRACE("创建文件失败！");  
		return FALSE;
	}

	DWORD dwWritten = 0;//写入文件的大小	 
	WriteFile(hResFile, pRes, dwResSize, &dwWritten, NULL);//写入文件  
	CloseHandle(hResFile);//关闭文件句柄  

	return (dwResSize == dwWritten);//若写入大小等于文件大小，返回成功，否则失败  
}

int main()
{
	CRegKey RegKey;
	bool deviceFound = false;
	std::string TargetSerial;
	std::string TargetHexFile;
	char command[1024];
	std::string Serials[256];
	std::cout << "使用方法：先不要将小方连接电脑，接下来将弹出选择界面，\n选择小方的HEX更新文件，等出现提示后再将小方连接电脑，程序将自动开始烧录\n按任意键以继续"<<std::endl;
	getchar();
	std::cout << "正在初始化..." << std::endl;

	if (RegKey.Open(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM") == ERROR_SUCCESS) {
		int nCount = 0;
		while (true)
		{
			char ValueName[_MAX_PATH];
			unsigned char ValueData[_MAX_PATH];
			DWORD nValueSize = _MAX_PATH;
			DWORD nDataSize = _MAX_PATH;
			DWORD nType;

			if (::RegEnumValue(HKEY(RegKey), nCount, ValueName, &nValueSize, NULL, &nType, ValueData, &nDataSize) == ERROR_NO_MORE_ITEMS){
				break;
			}

			Serials[nCount] = static_cast<std::string>((LPCSTR)ValueData);

			nCount++;
		}
	}
	else {
		std::cout << "错误：无法检测串口设备 请确认以管理员权限运行！\n按任意键退出" << std::endl;
		getchar();
		return 0;
	}

	if (is64BitOS()) {
		FreeResFile(IDR_AVR64, "AVR", "avrdude.exe");
		FreeResFile(IDR_AVR64CONF, "AVR", "avrdude.conf");
	}
	else {
		FreeResFile(IDR_AVR86, "AVR", "avrdude.exe");
		FreeResFile(IDR_AVR86CONF, "AVR", "avrdude.conf");
	}

	std::cout << "请选择小方的更新HEX文件" << std::endl;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "HEX file(*.hex)|*.hex||", NULL);
	while ((dlg.DoModal() != IDOK)) {
		std::cout << "请选择更新文件！" << std::endl;
	}
	TargetHexFile = dlg.GetPathName();
	std::cout << "已选择文件：" << TargetHexFile << std::endl;
	
	std::cout << "等待小方插入..." << std::endl;

	while (true) {
		Sleep(200);

		if (RegKey.Open(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM") == ERROR_SUCCESS) {
			int nCount = 0;
			while (true) {
				char ValueName[_MAX_PATH];
				unsigned char ValueData[_MAX_PATH];
				DWORD nValueSize = _MAX_PATH;
				DWORD nDataSize = _MAX_PATH;
				DWORD nType;

				if (::RegEnumValue(HKEY(RegKey), nCount, ValueName, &nValueSize, NULL, &nType, ValueData, &nDataSize) == ERROR_NO_MORE_ITEMS) {
					break;
				}

				if (Serials[nCount] != static_cast<std::string>((LPCSTR)ValueData)) {
					TargetSerial = static_cast<std::string>((LPCSTR)ValueData);
					HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, NULL, NULL, 0);
					if (hDevInfo) {
						SP_DEVINFO_DATA SpDevInfo = { sizeof(SP_DEVINFO_DATA) };
						for (DWORD iDevIndex = 0; SetupDiEnumDeviceInfo(hDevInfo, iDevIndex, &SpDevInfo); iDevIndex++) {
							char szName[512] = { 0 };
							if (SetupDiGetDeviceRegistryProperty(hDevInfo, &SpDevInfo, SPDRP_FRIENDLYNAME,
								NULL, (PBYTE)szName, sizeof(szName), NULL)) {
								std::string str = szName;
								int index1 = str.find("CH340"), index2 = str.find(TargetSerial);
								if (index1 != std::string::npos && index2 != std::string::npos) {
									deviceFound = true;
									break;
								}
							}
						}
						SetupDiDestroyDeviceInfoList(hDevInfo);
					}
				}

				if (deviceFound)break;

				nCount++;
			}
		}
		if (deviceFound)break;
	}

	std::cout << "检测到小方，开始烧录" << std::endl;

	wsprintf(command, ".\\avrdude.exe -C avrdude.conf -v -v  -p atmega328p -c arduino -P %s -b 115200 -D -U flash:w:\"%s\":i", TargetSerial.c_str(), TargetHexFile.c_str());
	//std::cout << command << std::endl;
	
	system(command);

	DeleteFile("avrdude.exe");
	DeleteFile("avrdude.conf");

	std::cout << "烧录完成！\n按任意键退出";
	getchar();
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
