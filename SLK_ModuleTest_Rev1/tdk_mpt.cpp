#include "stdafx.h"
#include <vector>
#include "stdlib.h"
#include "ModuleTestDLL.h"
#include <chrono>
#include <fstream>
#include "tdk_mpt.h"

/*
 * Here include the target platform hardware apis header files, like 
 * RealTek platform APIs
 */


 // SLK MT SW version 
#define SLKMTSWVERSIONMAJOR	1
#define SLKMTSWVERSIONMINOR	13

//#include "resource.h"
//#pragma comment(lib, "user32")

#include <atlbase.h>
#include <atlconv.h>
#include <Windows.h>
#include <ObjIdl.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib, "Gdiplus.lib")

#if 1
HDC         imageDC;        // the DC to hold our image
HBITMAP     imageBmp;       // the actual bitmap which contains the image (will be put in the DC)
HBITMAP     imageBmpOld;    // the DC's old bitmap (for cleanup)
HINSTANCE	inst;

const int   screenSize_X = 320;
const int   screenSize_Y = 200;

GdiplusStartupInput         gdiplusStartupInput;
ULONG_PTR                   gdiplusToken;
bool Target_Ready = false;
#define ID_CONTINUE_BUTTON					0x0011



///////////////////////////////
///////////////////////////////
// Function to load the image into our DC so we can draw it to the screen
void loadImage(const char* pathname)
{
	//	imageDC = CreateCompatibleDC(NULL);     // create an offscreen DC

	imageBmp = (HBITMAP)LoadImageA(         // load the bitmap from a file
		NULL,                           // not loading from a module, so this is NULL
		pathname,                       // the path we're loading from
		IMAGE_BITMAP,                   // we are loading a bitmap
		0, 0,                            // don't need to specify width/height
		LR_DEFAULTSIZE | LR_LOADFROMFILE// use the default bitmap size (whatever the file is), and load it from a file
	);

	imageBmpOld = (HBITMAP)SelectObject(imageDC, imageBmp);  // put the loaded image into our DC
}


///////////////////////////////
// Function to clean up
void cleanUpImage()
{
	SelectObject(imageDC, imageBmpOld);      // put the old bmp back in our DC
	DeleteObject(imageBmp);                 // delete the bmp we loaded
//	DeleteDC(imageDC);                      // delete the DC we created
}

///////////////////////////////
///////////////////////////////
// The function to draw our image to the display (the given DC is the screen DC)
void drawImage(HDC screen)
{
	BitBlt(
		screen,         // tell it we want to draw to the screen
		80, 65,            // as position 0,0 (upper-left corner)
		screenSize_X,   // width of the rect to draw
		screenSize_Y,   // height of the rect
		imageDC,        // the DC to get the rect from (our image DC)
		0, 0,            // take it from position 0,0 in the image DC
		SRCCOPY         // tell it to do a pixel-by-pixel copy
	);
}


wchar_t * char2wchar(const char* cchar)
{
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}
void displayImage(const char* pathName, HWND  hWnd)
{

	WCHAR		*imagePath = NULL;
	HDC			hdc = GetWindowDC(hWnd);
	
	if (NULL != pathName && hdc != 0) {
		Graphics	graphics(hdc);
		imagePath = char2wchar(pathName);
		if (imagePath) {
			Image		bmpImg(imagePath);
			UINT        width = bmpImg.GetWidth();
			UINT        height = bmpImg.GetHeight();
			graphics.DrawImage(&bmpImg, 25, 85, width * 2, height * 2);
		}
		ReleaseDC(hWnd, hdc);
		if (imagePath) {
			delete imagePath;
		}
	}
}

///////////////////////////////
///////////////////////////////
// A callback to handle Windows messages as they happen
LRESULT CALLBACK wndProc(HWND wnd, UINT msg, WPARAM w, LPARAM l)
{
	HWND					hReadyBtn;
	HDC					hdc = 0;
	int					wmID, wmEvent;
	// what kind of message is this?
	switch (msg){
	case WM_CREATE:
		// Create the buttons
		hReadyBtn = CreateWindow(L"BUTTON", L"Continue",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			105,
			180,
			80,
			30,
			wnd,
			(HMENU)ID_CONTINUE_BUTTON,
			(HINSTANCE)GetWindowLongPtr(wnd, GWLP_HINSTANCE),
			NULL);
#if 0
		hdc = GetWindowDC(wnd);
		if (hdc) {
			Graphics    graphics(hdc);
			Color       color = Color(255, 255, 255, 255);
			SolidBrush  brush(Color(255, 0, 0, 255));
			FontFamily  fontFamily(L"Microsoft YaHei");
			Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
			PointF      pointF(10.0f, 60.0f);

			graphics.DrawString(L"   请下压假手指,之后点击Continue按钮   ", -1, &font, pointF, &brush);

			ReleaseDC(wnd, hdc);
		}
#endif
		break;

	case WM_COMMAND:
		wmID = LOWORD(w);
		wmEvent = HIWORD(w);
		if (wmID == ID_CONTINUE_BUTTON) {
			Target_Ready = true;
		}
		break;

		// we are interested in WM_PAINT, as that is how we draw
		// MGAO: We don't care the WM_PARINT, the codes will cause the flashing screen issue.

	// we are also interested in the WM_DESTROY message, as that lets us know when to close the window
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		//return TRUE;

	default: 
		DefWindowProc(wnd, msg, w, l);

	}
	// for everything else, let the default window message handler do its thing
	return 0;

}


///////////////////////////////
///////////////////////////////
// A function to create the window and get it set up

LRESULT CALLBACK imgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int    wmID, wmEvent;
	HDC     hdc;
	HWND    hReadyButton;

	switch (message) {
	case WM_CREATE:
		// Create the buttons
		hReadyButton = CreateWindow(L"BUTTON", L"Continue",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			105,
			180,
			80,
			30,
			hWnd,
			(HMENU)ID_CONTINUE_BUTTON,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);

		break;

	case WM_COMMAND:
	{
		wmID = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		if (wmID == ID_CONTINUE_BUTTON) {
			Target_Ready = true;
		}
		//isWndClosed = true;
	}

	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		//isWndClosed = true;
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);

	}
	return 0;
}

void displayAlert(HWND  hWnd)
{
#if 1
	HDC hdc = GetWindowDC(hWnd);
	if (hdc) {
		Graphics    graphics(hdc);
		Color       color = Color(255, 255, 255, 255);
		SolidBrush  brush(Color(255, 0, 0, 255));
		FontFamily  fontFamily(L"Microsoft YaHei");
		Font        font(&fontFamily, 16, FontStyleRegular, UnitPixel);
		PointF      pointF(10.0f, 60.0f);
		graphics.DrawString(L"   请下压假手指,之后点击Continue按钮   ", -1, &font, pointF, &brush);

		ReleaseDC(hWnd, hdc);
	}
#endif
}

HWND createWindow(HINSTANCE inst)
{
	HWND                hWnd;
	WNDCLASSEX          wndClassEx = { sizeof(WNDCLASSEX),
								   CS_DBLCLKS,
									imgWndProc,
									0,
									0,
									GetModuleHandle(0),
									LoadIcon(0,IDI_APPLICATION),
									LoadCursor(0,IDC_ARROW),
									HBRUSH(COLOR_WINDOW + 1),
									0,
									L"TDKImageWindow",
									LoadIcon(0,IDI_APPLICATION)
	};

	ATOM  ret= RegisterClassEx(&wndClassEx);

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	hWnd = CreateWindowEx(0, L"TDKImageWindow", L"TDK TCFS2000",
		WS_OVERLAPPEDWINDOW | WS_EX_TOPMOST, 20, 20,
		320, 260, 0, 0, GetModuleHandle(0), 0);
	if (hWnd == NULL) {
		DWORD error = GetLastError();
		printf("Failed to create window:%x\r\n", error);
	}
	else{
		ShowWindow(hWnd, SW_SHOWDEFAULT);
	}
	return hWnd;
}
#endif


extern unsigned char CfgFileRootPath[];

//#define FULLYAUTO

#define TIMERSTART start = std::chrono::steady_clock::now();
#define TIMEREND  end = std::chrono::steady_clock::now(); timefile << " Test time(ms)	:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

void show(HWND hwnd);

inline int NULLFunc(void)
{
	// This API is not implemented

	return INVS_SUCCESS;
}


int	INVS_ExecuteTestCase(unsigned char  *invsBase, unsigned char *sn, INVS_TEST_CASE testCase, PINVS_MPT_RESULT  pDetailedResult)
{
	int rc;
	uint32_t ifpdllVer;
	uint32_t MTDLLVer;

	int RunNum;
	int ALLRUNNUM;
	char ContRun;
	int PassFailThisRun[MTSESSIONMAX];
	int PassFailOverall[MTSESSIONMAX];	std::fill(PassFailOverall, PassFailOverall + MTSESSIONMAX, 0);
	double Yield_All[MTSESSIONMAX];	std::fill(Yield_All, Yield_All + MTSESSIONMAX, 0.0);
	char BinCodeAll[MTSESSIONMAX*MTSTRINGMAX];
	//std::string BinCodeAll[MTSESSIONMAX];
	char SNLogFile[MTSESSIONMAX][MTSTRINGMAX] = {};
	HWND imageWnd = NULL;

	// Data Directory
	std::string DataDir(reinterpret_cast<char*>(invsBase));
	std::string ImgDir = DataDir;
	std::string ImgFileName = ImgDir;
	std::string CfgDir = DataDir + "\\cfgfile";
	std::string CfgFile;
	std::string SrcCfgFile;
	std::string SrcCfgFileName;
	std::string CfgFilethiscase;
	std::string SNLogFileName;

	char imagefilename[MTSTRINGMAX];


	//Timer ----------------------------------------------------------------------------------------------------
	std::ofstream timefile;
	std::string timefilename = DataDir + "\\TestTime.txt";
	timefile.open(timefilename, std::ios_base::out | std::ios_base::app);

	auto overallstart = std::chrono::steady_clock::now();
	auto start = std::chrono::steady_clock::now();
	auto end = std::chrono::steady_clock::now();
	auto thisrunstart = std::chrono::steady_clock::now();
	auto thisrunend = std::chrono::steady_clock::now();
	//timefile << "Test time in seconds :" << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " sec" << std::endl;
	//std::cout << "Test time in seconds :" << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " sec" << std::endl;

	// --------------------------------------------------------------------------------------------------------
	if ((rc = MT_HelloWorld(0)) != MTDLL_OK) { printf("MT HelloWorld error\r\n");		return -1; }

	if ((rc = MT_ChkDLLVer(&ifpdllVer, &MTDLLVer)) != MTDLL_OK) { printf("MT Chk DLL version error\r\n");		return -1; }
	printf("ifpdll version %d.%d.%d\n", (ifpdllVer >> 16) & 0xFF, (ifpdllVer >> 8) & 0xFF, (ifpdllVer & 0xFF));
	printf("MTdll version %d.%d.%d\n", (MTDLLVer >> 16) & 0xFF, (MTDLLVer >> 8) & 0xFF, (MTDLLVer & 0xFF));



	// Subfolder to store Images
//	char MakeImageFolder[MTSTRINGMAX];
	char OpenImageFolder[MTSTRINGMAX];
	char CloseImageFolder[MTSTRINGMAX];
	char DC[MTSTRINGMAX];
	if ((rc = MT_GetDate(DC)) != MTDLL_OK) { printf("MT Get Date Code error\r\n");		return -1; }
	char TS[MTSTRINGMAX];
	if ((rc = MT_GetTime(TS)) != MTDLL_OK) { printf("MT Get Time Stamp error\r\n");		return -1; }
	timefile << std::endl << std::endl;
	timefile << "===== New Round of Test Run On " << DC << " " << TS << std::endl;


	TIMERSTART;
//	//sprintf_s(MakeImageFolder, MTSTRINGMAX, "mkdir %s\\%s", ImgDir.c_str(), DC);
	//system(MakeImageFolder);

	// === Load Nucleo COM Port No from file ===== 
	uint64_t MYCOMPORT[MTSESSIONMAX];
	int PortCnt;



	PortCnt = 1;
	MYCOMPORT[0] = 5;


	// ===== Parse CFG file =====
	char MakeCfgFolder[MTSTRINGMAX];
	char DelCfgFolder[MTSTRINGMAX];
	char CopyCfgFile[MTSTRINGMAX];

	sprintf_s(DelCfgFolder, MTSTRINGMAX, "rmdir /S /Q %s", CfgDir.c_str());
	system(DelCfgFolder);

	sprintf_s(MakeCfgFolder, MTSTRINGMAX, "mkdir %s", CfgDir.c_str());
	system(MakeCfgFolder);

	std::stringstream ss;
	// Copy corresponding Cfg file to folder
	switch (testCase) {
	case INVS_TEST_CASE_1:
		ss << "SLK_Flex-01_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	case INVS_TEST_CASE_2:
		ss << "SLK_Flex-02_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	case INVS_TEST_CASE_3:
		ss << "SLK_Flex-03_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	case INVS_TEST_CASE_4:
		ss << "SLK_*-04_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	case INVS_TEST_CASE_5:
		ss << "SLK_*-05_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	//case INVS_TEST_CASE_6:
	//	ss << "SLK_*-06_rev*.cfg";
	//	CfgFilethiscase = ss.str();
	//	CfgFile = CfgDir + "\\" + CfgFilethiscase;
	//	SrcCfgFile = (char *)CfgFileRootPath;
	//	sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
	//	break;
	//case INVS_TEST_CASE_7:
	//	ss << "SLK_*-07_rev*.cfg";
	//	CfgFilethiscase = ss.str();
	//	CfgFile = CfgDir + "\\" + CfgFilethiscase;
	//	SrcCfgFile = (char *)CfgFileRootPath;
	//	sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
	//	break;
	default:
		ss << "SLK_*-05_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	}
	SrcCfgFileName = SrcCfgFile + "\\" + CfgFilethiscase;
/*
	intptr_t file;
	_finddata_t filedata;
	file = _findfirst(SrcCfgFileName.c_str(), &filedata);
	if (file != -1)
	{
		do
		{
			std::cout << filedata.name << std::endl;
			break;
			// Or put the file name in a vector here
		} while (_findnext(file, &filedata) == 0);
	}
	else
	{
		std::cout << "No described Cfg file " << SrcCfgFileName << " found" << std::endl;
		std::cout << "Please check before continuing." << std::endl;
		return -1;
	}
	_findclose(file);
*/	
	system(CopyCfgFile);

	if ((rc = MT_CfgFileParse(CfgDir.c_str(), CfgFile.c_str())) != MTDLL_OK) { printf("MT CFG Parser error\r\n");		return -1; }

	// ===== Set Data Directory =====
	if ((rc = MT_Set_DataDir(invsBase)) != MTDLL_OK) { printf("MT Set Data Directory error\r\n");		return -1; }

	// ===== Deal With Diff Test Cases =====
	// Pass Through for TEST_CASE_0, TEST_CASE_5
	// Run corresponding Cfg file for other cases
	if (testCase == INVS_TEST_CASE_0 || testCase == INVS_TEST_CASE_6 || testCase == INVS_TEST_CASE_7 || testCase == INVS_TEST_CASE_8) {
		std::cout << "TEST CASE " << testCase << " Skipped. It Will Be Performed In other TEST CASES" << std::endl;
		return INVS_SUCCESS;
	}
	else if (	testCase == INVS_TEST_CASE_1 || testCase == INVS_TEST_CASE_2 || testCase == INVS_TEST_CASE_3 || testCase == INVS_TEST_CASE_4||
				testCase == INVS_TEST_CASE_5) {
		std::cout << "TEST CASE " << testCase << " Selected. " << std::endl;
	}
	else {
		std::cout << "ERROR: Wrong TEST CASE Selected" << std::endl;
		return INVS_FAILURE;
	}

	// ===== Write Cfg and SW ver to Log File Header =====
	uint32_t SLKSWVer = (SLKMTSWVERSIONMAJOR << 16) + SLKMTSWVERSIONMINOR;
	if ((rc = MT_WrtCFGtoLog(PortCnt, SLKSWVer)) != MTDLL_OK) { printf("MT Init LOG error\r\n");		return -1; }
	

	// Test Setup --------------------------------------------------------------------------------------------------------------
	uint64_t COMPORTin[MTSESSIONMAX];
	uint64_t COMPORTout[MTSESSIONMAX];
	uint64_t COMPORTsorted[MTSESSIONMAX];

	for (int i = 0; i < PortCnt; i++)
		COMPORTin[i] = MYCOMPORT[i];

	if ((rc = MT_Init(PortCnt)) != MTDLL_OK) { printf("MT Init error\r\n"); return -1; }
	if ((rc = MT_Chk_HW(COMPORTin, PortCnt, COMPORTout)) != MTDLL_OK) { printf("MT ChkHW error\r\n");		return -1; }
	
	// ===== Power off Nucleo just in case it was not shut down properly =====
	if ((rc = MT_PowerOFFDUT(PortCnt)) != MTDLL_OK) { printf("MT Power ON DUT error\r\n");		return -1; }
	timefile << "Test Setup	"; TIMEREND;

	ContRun = '1';
	RunNum = 1;

	while (ContRun != '0') {
#ifdef FULLYAUTO
#else
		TIMERSTART;
		thisrunstart = std::chrono::steady_clock::now();
		std::cout << std::endl;
		std::cout << "This is Run No " << RunNum << "  >>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
		std::cout << "Press Enter when DUT is connected ready >> ";
		std::cin.get();
		std::fill(PassFailThisRun, PassFailThisRun + 8, 0);
		timefile << "User Manually Start Test "; TIMEREND;
#endif
		// Main Test Loop ---------------------------------------------------------------------------------------	
		// ===== Initial Log =====
		TIMERSTART;
		if ((rc = MT_InitLOG(PortCnt)) != MTDLL_OK) { printf("MT Init LOG error\r\n");		return -1; }

		// ===== Write SN =====
		unsigned char SNs[MTSESSIONMAX][MTSTRINGMAX];
		for (int i = 0; i < PortCnt; i++) { 
			strcpy_s((char*)(SNs+i*MTSTRINGMAX), MTSTRINGMAX, (char *)sn);
		}
		if ((rc = MT_Input_SN(PortCnt, (unsigned char *)SNs)) != MTDLL_OK) { printf("MT Input SN error\r\n");	return -1; }

		std::string SNStr(reinterpret_cast<char*>(SNs[0]));
		ImgDir = ImgDir + "\\" + SNStr;
		sprintf_s(OpenImageFolder, MTSTRINGMAX, "start /W Explorer %s", ImgDir.c_str());
		sprintf_s(CloseImageFolder, MTSTRINGMAX, "taskkill /F /FI \"IMAGENAME eq explorer.exe\" /FI \"WINDOWTITLE eq %s\" ", SNStr.c_str());

		// ===== Write CFG to SN Log file ======
		if ((rc = MT_WrtCFGtoSNLog(PortCnt, (char *)SNLogFile, SLKSWVer)) != MTDLL_OK) { printf("MT Init SN LOG error\r\n");		return -1; }
		timefile << "Init LOG	"; TIMEREND;

		std::string SNLogFileName(reinterpret_cast<char*>(SNLogFile[0]));
		std::string substr = "_TargetImage.bmp";
		ImgFileName = SNLogFileName;
		ImgFileName.replace(ImgFileName.begin()+ImgFileName.length()-4, ImgFileName.end(), substr.begin(), substr.end());

		// ===== Power ON =====
		TIMERSTART;
		if ((rc = MT_PowerONDUT(PortCnt)) != MTDLL_OK) { printf("MT Power ON DUT error\r\n");		return -1; }
		timefile << "Power ON DUT	"; TIMEREND;

		// ===== SPI COMM =====
		TIMERSTART;
		if ((rc = MT_Chk_SPIComm(PortCnt)) != MTDLL_OK) { printf("MT SPI Comm Chk error\r\n");		return -1; }
		timefile << "SPI COMM Chk	"; TIMEREND;

		// ===== ASIC =====
		TIMERSTART;
		//int* DieID = new int[PortCnt];
		int DieID[MTSESSIONMAX];
		int WaferID[MTSESSIONMAX];
		char LotID[MTSESSIONMAX][8];
		if ((rc = MT_Chk_ASICID(PortCnt, DieID, WaferID, (char *)LotID)) != MTDLL_OK) { printf("MT ASIC ID Chk error\r\n");		return -1; }
		for (int i = 0; i < PortCnt; i++) { printf("LotID, WaferID & DieID Outputs from MT_Chk_ASICID, Port%d: %s %d %d\n", i, LotID[i], WaferID[i], DieID[i]); }
		timefile << "ASIC ID Chk	"; TIMEREND;

		// ===== Pass Bit =====
		TIMERSTART;
		int STPass[MTSESSIONMAX];
		int MTPass[MTSESSIONMAX];
		int DieIDPB[MTSESSIONMAX];

		if ((rc = MT_Chk_PassBit(PortCnt, STPass, MTPass, DieIDPB)) != MTDLL_OK) { printf("MT Pass Bit Chk error\r\n");		return -1; }
		for (int i = 0; i < PortCnt; i++) { printf("PassBit Output from MT_Chk_PassBit, Port%d(DieID:%d): ST-%d, MT-%d\n", i, DieIDPB[i], STPass[i], MTPass[i]); }
		timefile << "Pass Bit Chk	"; TIMEREND;
		
		// ===== OTP Content =====
		TIMERSTART;
		if ((rc = MT_Chk_OTPContent(PortCnt)) != MTDLL_OK) { printf("MT OTP Content Chk error\r\n");		return -1; }
		timefile << "OTP Content Chk	"; TIMEREND;

		// If ST=0 for Station 1, 2, 3 or MT=0 for Station 5, 6, 7 --> quit right away
		if (STPass[0] == 0 &&
			(testCase == INVS_TEST_CASE_1 || testCase == INVS_TEST_CASE_2 || testCase == INVS_TEST_CASE_3)) {
			PassFailOverall[0] = 0;
			if ((rc = MT_Chk_BinCode(PortCnt, BinCodeAll) != MTDLL_OK)) { printf("MT Test Result Check error\r\n");		return -1; }
			MT_DisplayResult(PortCnt, RunNum, PassFailOverall, Yield_All);
			TIMERSTART;
			if ((rc = MT_WriteLOG(PortCnt, RunNum, Yield_All)) != MTDLL_OK) { printf("MT Write LOG error\r\n");		return -1; }
			if ((rc = MT_WriteSNLOG(PortCnt, (char *)SNLogFile, 1, Yield_All)) != MTDLL_OK) { printf("MT Write LOG error\r\n");		return -1; }
			timefile << "Write Log file	"; TIMEREND;
			break;
		}
		else if (MTPass[0] == 0 &&
			(testCase == INVS_TEST_CASE_5 || testCase == INVS_TEST_CASE_6 || testCase == INVS_TEST_CASE_7)) {
			PassFailOverall[0] = 0;
			if ((rc = MT_Chk_BinCode(PortCnt, BinCodeAll) != MTDLL_OK)) { printf("MT Test Result Check error\r\n");		return -1; }
			MT_DisplayResult(PortCnt, RunNum, PassFailOverall, Yield_All);
			TIMERSTART;
			if ((rc = MT_WriteLOG(PortCnt, RunNum, Yield_All)) != MTDLL_OK) { printf("MT Write LOG error\r\n");		return -1; }
			if ((rc = MT_WriteSNLOG(PortCnt, (char *)SNLogFile, 1, Yield_All)) != MTDLL_OK) { printf("MT Write LOG error\r\n");		return -1; }
			timefile << "Write Log file	"; TIMEREND;
			break;
		}
		
		// ===== LOAD DMP =====
		TIMERSTART;
		if ((rc = MT_LoadDMP(PortCnt)) != MTDLL_OK) { printf("MT Load DMP error\r\n");		return -1; }
		timefile << "Load DMP	"; TIMEREND;

		//===== IDD check =====
		TIMERSTART;
		if ((rc = MT_Chk_IDD(PortCnt)) != MTDLL_OK) { printf("MT IDD Chk error\r\n");		return -1; }
		timefile << "IDD Check	"; TIMEREND;
#if 1
		//===== Encryption Chk =====
		TIMERSTART;
		if ((rc = MT_Chk_Encrypt(PortCnt)) != MTDLL_OK) { printf("MT Encryption Chk error\r\n");		return -1; }
		timefile << "Encryption Chk	"; TIMEREND;
#endif
		// Exit Test if EncChk fails
		int PassFail_EncChk[MTSESSIONMAX];

		if ((rc = MT_Chk_TestResult(PortCnt, PassFail_EncChk) != MTDLL_OK)) { printf("MT Test Result Check error\r\n");		return -1; }
		if (PassFail_EncChk[0] == 0) {
			PassFailOverall[0] = 0;
			if ((rc = MT_Chk_BinCode(PortCnt, BinCodeAll) != MTDLL_OK)) { printf("MT Test Result Check error\r\n");		return -1; }
			MT_DisplayResult(PortCnt, RunNum, PassFailOverall, Yield_All);
			TIMERSTART;
			if ((rc = MT_WriteLOG(PortCnt, RunNum, Yield_All)) != MTDLL_OK) { printf("MT Write LOG error\r\n");		return -1; }
			if ((rc = MT_WriteSNLOG(PortCnt, (char *)SNLogFile, 1, Yield_All)) != MTDLL_OK) { printf("MT Write LOG error\r\n");		return -1; }
			timefile << "Write Log file	"; TIMEREND;
			break;
		}

		//===== Calibration =====
		TIMERSTART;
		if ((rc = MT_Calibration(PortCnt)) != MTDLL_OK) { printf("MT Calibration error\r\n");		return -1; }
		timefile << "Calibration	"; TIMEREND;
		
		//===== Gain ONLY Image Chk =====
		TIMERSTART;
		if ((rc = MT_GainOnlyImageScan(PortCnt)) != MTDLL_OK) { printf("MT Gain Only Image Scan error\r\n");		return -1; }
		timefile << "Gain Only Image	"; TIMEREND;

		//===== Take Air Image =====
		TIMERSTART;
		if ((rc = MT_AirImageScan(PortCnt)) != MTDLL_OK) { printf("MT Gain Only Image Scan error\r\n");		return -1; }
		timefile << "Air Image	"; TIMEREND;

		// ===== Temp Chk =====
		TIMERSTART;
		if ((rc = MT_Chk_Temp(PortCnt)) != MTDLL_OK) { printf("MT Temp Chk error\r\n");		return -1; }
		timefile << "Temp Chk	"; TIMEREND;

#ifdef FULLYAUTO
		TIMERSTART;
		std::cout << "Target Loaded by GPIO controlled fixture" << std::endl;
		// Set GPIO to high to Load Target
		//if ((rc = GPIOWrite(PortCnt, 6, 2, 1)) != MTDLL_OK) { printf("MT GPIO Write error\r\n");		return -1; }
		timefile << "GPIO Ld Target	"; TIMEREND;
#else
		if (testCase == INVS_TEST_CASE_1 || testCase == INVS_TEST_CASE_2 || testCase == INVS_TEST_CASE_3 || testCase == INVS_TEST_CASE_5) {
			std::cout << "This station has No Target Loading Needed " << std::endl;
		}
		//else if (testCase == INVS_TEST_CASE_7 ) {
		//	std::cout << "SPI COMM Test Only, No Target Loading Needed " << std::endl;
		//}
		else {
			TIMERSTART;
			bool Target_Ready = false;
			std::string Target_Userin = "";
			std::cout << std::endl;
			std::cout << std::endl;
			std::cout << std::endl;
			std::cout << "Please put Target Down >>";

			// std::cin.get();
			Target_Ready = false;
		    imageWnd = createWindow(inst);
			SetFocus(imageWnd);
			displayAlert(imageWnd);
			//system(OpenImageFolder);
			//===== Image samples =====

			MSG msg;
			int timer = 0;
			sprintf_s(imagefilename, MTSTRINGMAX, "%s", ImgFileName.c_str());
			while (PeekMessage(&msg, imageWnd, 0, 0, PM_REMOVE) >= 0) {
				if (msg.message == WM_LBUTTONDOWN) {
					Target_Ready = true;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (Target_Ready)
					break;
			}
			if ((rc = MT_ImageScan(PortCnt, (char *)SNLogFile, "TargetImage", false, 1)) != MTDLL_OK) { 
				printf("MT Image Scan error\r\n");		
				return -1; 
			}

			displayImage(imagefilename, imageWnd);

#if 0
			while (!Target_Ready) {
				//===== Image samples =====
				if ((rc = MT_ImageScan(PortCnt, (char *)SNLogFile, "TargetImage", false, 1)) != MTDLL_OK) { printf("MT Image Scan error\r\n");		return -1; }

				std::cout << std::endl;
				std::cout << std::endl;
				std::cout << "Please check Image Sample  ";
				std::cout << "Is Target ready? Y-- Ready / N-- Take another Image Sample: ";

				show(GetConsoleWindow());
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

				std::cin >> Target_Userin;
				//cleanUpImage();

				Target_Ready = ((Target_Userin == "Y") ? true : false);
			}
#endif
			//system(CloseImageFolder);
			std::cout << std::endl;
			std::cout << "Please Keep Target Loaded";
			std::cout << std::endl;
			std::cout << std::endl;
			std::cout << std::endl;

			timefile << "User Ld Target	"; TIMEREND;
		}
#endif

		//===== Target Image =====
		TIMERSTART;
		if ((rc = MT_TargetImageScan(PortCnt)) != MTDLL_OK) { printf("MT Gain Only Image Scan error\r\n");		return -1; }
		timefile << "Target Image	"; 
		TIMEREND;

#ifdef FULLYAUTO
		TIMERSTART;
		std::cout << "Target Released by GPIO controlled fixture" << std::endl;
		// Set GPIO to high to Load Target
		//if ((rc = GPIOWrite(PortCnt, 6, 2, 0)) != MTDLL_OK) { printf("MT GPIO Write error\r\n");		return -1; }
		timefile << "GPIO Rlz Target	"; TIMEREND;
#else
		if (testCase == INVS_TEST_CASE_1 || testCase == INVS_TEST_CASE_2 || testCase == INVS_TEST_CASE_3) {
			std::cout << "Test Continues " << std::endl;
		}
		else {
			// Sleep(2000);
			std::cout << "Please Release Target" << std::endl;
		}
#endif
		//===== OTP Write =====
		TIMERSTART;
		if ((rc = MT_OTPWrite(PortCnt)) != MTDLL_OK) { printf("MT OTP Write error\r\n");		return -1; }
		timefile << "OTP Write	"; TIMEREND;

		//===== Power OFF=====
		TIMERSTART;
		if ((rc = MT_PowerOFFDUT(PortCnt)) != MTDLL_OK) { printf("MT Power OFF DUT error\r\n");		return -1; }
		timefile << "Power OFF DUT	"; TIMEREND;

		// ===== Return PassFail, BinCode for this Run =====
		TIMERSTART;
		if ((rc = MT_Chk_TestResult(PortCnt, PassFailThisRun) != MTDLL_OK)) { printf("MT Test Result Check error\r\n");		return -1; }
		if ((rc = MT_Chk_BinCode(PortCnt, BinCodeAll) != MTDLL_OK)) { printf("MT Test Result Check error\r\n");		return -1; }

		// ===== Display Result =====
		for (int i = 0; i < PortCnt; i++) {
			PassFailOverall[i] = PassFailOverall[i] + PassFailThisRun[i];
			Yield_All[i] = double(PassFailOverall[i]) / RunNum;
		}

		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << "Run " << RunNum << " Finished ------------->" << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;

		// Close the Image Window
		if (imageWnd != NULL) {
			//Sleep(5000);
			Sleep(1000);
			//CloseWindow(imageWnd);
			DestroyWindow(imageWnd);
		}
		MT_DisplayResult(PortCnt, RunNum, PassFailOverall, Yield_All);
		timefile << "Display Result	"; TIMEREND;

		// ===== PINVS_MPT_RESULT =====
		strcpy_s((char *)pDetailedResult->sn, MTSTRINGMAX, (char *)sn);
		pDetailedResult->caseNum = testCase;
		pDetailedResult->result = ((PassFailOverall[0] == 1) ? 0 : 1);
		pDetailedResult->errorCount = 0;
		//pDetailedResult->binCodes = BinCodeAll[0];
		strcpy_s(pDetailedResult->binCodes, 256, BinCodeAll);

		// ===== Write Log =====
		TIMERSTART;
		if ((rc = MT_WriteLOG(PortCnt, RunNum, Yield_All)) != MTDLL_OK) { printf("MT Write LOG error\r\n");		return -1; }		
		// ===== Write SN Log =====		
		if ((rc = MT_WriteSNLOG(PortCnt, (char *)SNLogFile, 1, Yield_All)) != MTDLL_OK) { printf("MT Write LOG error\r\n");		return -1; }
		timefile << "Write Log file	"; TIMEREND;

		RunNum++;
		thisrunend = std::chrono::steady_clock::now();
		timefile << "Curret Run Test time in seconds :	" << std::chrono::duration_cast<std::chrono::seconds>(thisrunend - thisrunstart).count() << " sec" << std::endl;


#ifdef FULLYAUTO
		TIMERSTART;
		ALLRUNNUM = 0;
		timefile << "AUTO End Test	"; TIMEREND;
#else
		TIMERSTART;
		std::cout << "====================================================================" << std::endl;
		std::cout << " Press Enter to Continue Testing Next Part (Use 0 for quit): " << std::endl;
		std::cout << "====================================================================" << std::endl;
		//ContRun = std::cin.get();
		ContRun = '0';
		timefile << "User End Test	"; TIMEREND;
#endif
	}

	std::cout << "All " << RunNum - 1 << " Runs finished. Stop Test." << std::endl;
	std::cout << std::endl;


	// ===== Finalize DLL =====
	TIMERSTART;
	if ((rc = MT_Finit(PortCnt)) != MTDLL_OK) { printf("MT Finit error\r\n");		return -1; }
	timefile << "Finalize DLL	"; TIMEREND;

	auto overallend = std::chrono::steady_clock::now();
	timefile << "===== Overall Test time in seconds :	" << std::chrono::duration_cast<std::chrono::seconds>(overallend - overallstart).count() << " sec" << std::endl;


	return ((PassFailOverall[0] == 1) ? INVS_SUCCESS : INVS_FAILURE);
}


// ====================== Helper Function ===================================================

void show(HWND hwnd) {

	WINDOWPLACEMENT place = { sizeof(WINDOWPLACEMENT) };
	GetWindowPlacement(hwnd, &place);
	switch (place.showCmd)
	{
	case SW_SHOWMAXIMIZED:
		ShowWindow(hwnd, SW_SHOWMAXIMIZED);
		break;
	case SW_SHOWMINIMIZED:
		ShowWindow(hwnd, SW_RESTORE);
		break;
	default:
		ShowWindow(hwnd, SW_NORMAL);
		break;
	}
	SetWindowPos(0, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
	SetForegroundWindow(hwnd);



	return;
}