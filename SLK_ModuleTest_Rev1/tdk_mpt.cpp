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
#define SLKMTSWVERSIONMINOR	5



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


	// Data Directory
	std::string DataDir(reinterpret_cast<char*>(invsBase));
#if 0
	std::string ImgDir = DataDir + "\\OutImages";
#endif
	std::string CfgDir = DataDir + "\\cfgfile";
	std::string CfgFile;
	std::string SrcCfgFile;
	std::string SrcCfgFileName;
	std::string CfgFilethiscase;
	


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
	char MakeImageFolder[MTSTRINGMAX];
	char OpenImageFolder[MTSTRINGMAX];
	char CloseImageFolder[MTSTRINGMAX];
	char DC[MTSTRINGMAX];
	if ((rc = MT_GetDate(DC)) != MTDLL_OK) { printf("MT Get Date Code error\r\n");		return -1; }
	char TS[MTSTRINGMAX];
	if ((rc = MT_GetTime(TS)) != MTDLL_OK) { printf("MT Get Time Stamp error\r\n");		return -1; }
	timefile << std::endl << std::endl;
	timefile << "===== New Round of Test Run On " << DC << " " << TS << std::endl;


	TIMERSTART;
#if 0
	sprintf_s(MakeImageFolder, MTSTRINGMAX, "mkdir %s\\%s", ImgDir.c_str(), DC);
	sprintf_s(OpenImageFolder, MTSTRINGMAX, "start /W Explorer %s%s", ImgDir.c_str(), DC);
	sprintf_s(CloseImageFolder, MTSTRINGMAX, "taskkill /F /FI \"IMAGENAME eq explorer.exe\" /FI \"WINDOWTITLE eq %s\" ", DC);
	system(MakeImageFolder);
#endif

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
		ss << "TE-*-01_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	case INVS_TEST_CASE_2:
		ss << "TE-*-02_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	case INVS_TEST_CASE_3:
		ss << "TE-*-03_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	case INVS_TEST_CASE_4:
		ss << "TE-*-04_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	case INVS_TEST_CASE_5:
		ss << "TE-*-05_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	case INVS_TEST_CASE_6:
		ss << "TE-*-06_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	case INVS_TEST_CASE_7:
		ss << "TE-*-07_rev*.cfg";
		CfgFilethiscase = ss.str();
		CfgFile = CfgDir + "\\" + CfgFilethiscase;
		SrcCfgFile = (char *)CfgFileRootPath;
		sprintf_s(CopyCfgFile, MTSTRINGMAX, "copy %s\\%s %s", SrcCfgFile.c_str(), CfgFilethiscase.c_str(), CfgFile.c_str());
		break;
	default:
		ss << "TE-*-05_rev*.cfg";
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
	if (testCase == INVS_TEST_CASE_0 || testCase == INVS_TEST_CASE_8) {
		std::cout << "TEST CASE " << testCase << " Skipped. It Will Be Performed In other TEST CASES" << std::endl;
		return INVS_SUCCESS;
	}
	else if (	testCase == INVS_TEST_CASE_1 || testCase == INVS_TEST_CASE_2 || testCase == INVS_TEST_CASE_3 || testCase == INVS_TEST_CASE_4||
				testCase == INVS_TEST_CASE_5 || testCase == INVS_TEST_CASE_6 || testCase == INVS_TEST_CASE_7) {
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

		// ===== Write CFG to SN Log file ======
		if ((rc = MT_WrtCFGtoSNLog(PortCnt, (char *)SNLogFile, SLKSWVer)) != MTDLL_OK) { printf("MT Init SN LOG error\r\n");		return -1; }
		timefile << "Init LOG	"; TIMEREND;

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

		//===== Encryption Chk =====
		TIMERSTART;
		if ((rc = MT_Chk_Encrypt(PortCnt)) != MTDLL_OK) { printf("MT Encryption Chk error\r\n");		return -1; }
		timefile << "Encryption Chk	"; TIMEREND;

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
		if (testCase == INVS_TEST_CASE_1 || testCase == INVS_TEST_CASE_3 || testCase == INVS_TEST_CASE_5 || testCase == INVS_TEST_CASE_6) {
			std::cout << "This station has No Target Loading Needed " << std::endl;
		}
		else if (testCase == INVS_TEST_CASE_7 ) {
			std::cout << "SPI COMM Test Only, No Target Loading Needed " << std::endl;
		}
		else {
			TIMERSTART;
#if 0
			bool Target_Ready = false;
			std::string Target_Userin = "";
			std::cout << std::endl;
			std::cout << std::endl;
			std::cout << std::endl;
			std::cout << "Please put Target Down, press Enter when target is loaded >>";
			std::cin.get();
#endif
  		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << "Please put Target Down, press OK button when target is loaded correctly! >>";
		std::cout << "\r\n";
		std::cout << std::endl;
		
		if ((rc = MT_ImageScan(PortCnt, (char *)SNLogFile, "TargetImage", true, 1)) != MTDLL_OK) { printf("MT Image Scan error\r\n");		return -1; }

#if 0

			system(OpenImageFolder);
			while (!Target_Ready) {
				//===== Image samples =====
				if ((rc = MT_ImageScan(PortCnt, 1)) != MTDLL_OK) { printf("MT Image Scan error\r\n");		return -1; }

				std::cout << std::endl;
				std::cout << std::endl;
				std::cout << "Please check Image Sample  ";
				std::cout << "Is Target ready? Y-- Ready / N-- Take another Image Sample: ";

				show(GetConsoleWindow());
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

				std::cin >> Target_Userin;
				Target_Ready = ((Target_Userin == "Y") ? true : false);
			}

			system(CloseImageFolder);
#endif
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
		timefile << "Target Image	"; TIMEREND;

#ifdef FULLYAUTO
		TIMERSTART;
		std::cout << "Target Released by GPIO controlled fixture" << std::endl;
		// Set GPIO to high to Load Target
		//if ((rc = GPIOWrite(PortCnt, 6, 2, 0)) != MTDLL_OK) { printf("MT GPIO Write error\r\n");		return -1; }
		timefile << "GPIO Rlz Target	"; TIMEREND;
#else
		if (testCase == INVS_TEST_CASE_1 || testCase == INVS_TEST_CASE_3 || testCase == INVS_TEST_CASE_7) {
			std::cout << "Test Continues " << std::endl;
		}
		else {
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