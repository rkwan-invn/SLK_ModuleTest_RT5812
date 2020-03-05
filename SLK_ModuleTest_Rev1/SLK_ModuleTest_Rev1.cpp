// ModuleTestDLLTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include "stdlib.h"
#include "ModuleTestDLL.h"
#include <chrono>
#include <fstream>

#include <filesystem> // or #include <filesystem>

namespace fs = std::experimental::filesystem;

#include "tdk_mpt.h"


//#define FULLYAUTO

#define CFGFILEPATH "CfgPathInfo.txt"
#define TSTSTATIONFILE "CurTestNo.txt"

unsigned char CfgFileRootPath[MTSTRINGMAX];

int CfgFilePath_Check(unsigned char *CfgFilePath) {

	std::ifstream myReadFile;

	myReadFile.open(CFGFILEPATH);

	if (myReadFile.is_open()) {
			myReadFile >> CfgFilePath;
	}
	else {
		return false;
	}
	myReadFile.close();
	return true;
}

int _tmain(int argc, _TCHAR* argv[]) {

	int rc;
	std::string PassFailThisRun;
	//unsigned char INVN_DataDir[INVS_MAX_DIR_PATH] = "C:\\MT_Result\\InvnBase";
	unsigned char INVN_DataDir[INVS_MAX_DIR_PATH];
	unsigned char SN[MTSTRINGMAX];
	
	strcpy_s((char*)SN, MTSTRINGMAX, "8SJYAAE0023HJ1KSYMDSSSS");

	INVS_TEST_CASE TestCase;
	int TEST_Staion_No;

	// =========== Check Cfg File Path ==============

	CfgFilePath_Check(CfgFileRootPath);

	if (fs::exists(CfgFileRootPath) == FALSE)
	{
		printf("Cfg file path %s does not exist!\r\n", CfgFileRootPath);
		printf("Please check before continuing.\r\n");
		return false;
	}

	// =========== User to specify TEST Station Number ===================
//	TEST_Staion_No = 7;

	std::ifstream TstNoFile;

	TstNoFile.open(TSTSTATIONFILE);

	if (TstNoFile.is_open()) {
		TstNoFile >> TEST_Staion_No;
	}
	else {
		printf("Cannot find the current station number from CurTestNo.txt!\r\n");
		printf("Please check before continuing.\r\n");
		return false;
	}
	TstNoFile.close();
	
	// ===================================================================

	switch (TEST_Staion_No) {
		case 1:
			TestCase = INVS_TEST_CASE_1;
			strcpy_s((char *)INVN_DataDir, INVS_MAX_DIR_PATH, "C:\\MT_Result\\InvnBase\\TestStation1");
			break;
		case 2:
			TestCase = INVS_TEST_CASE_2;
			strcpy_s((char *)INVN_DataDir, INVS_MAX_DIR_PATH, "C:\\MT_Result\\InvnBase\\TestStation2");
			break;
		case 3:
			TestCase = INVS_TEST_CASE_3;
			strcpy_s((char *)INVN_DataDir, INVS_MAX_DIR_PATH, "C:\\MT_Result\\InvnBase\\TestStation3");
			break;
		case 4:
			TestCase = INVS_TEST_CASE_4;
			strcpy_s((char *)INVN_DataDir, INVS_MAX_DIR_PATH, "C:\\MT_Result\\InvnBase\\TestStation4");
			break;
		case 5:
			TestCase = INVS_TEST_CASE_5;
			strcpy_s((char *)INVN_DataDir, INVS_MAX_DIR_PATH, "C:\\MT_Result\\InvnBase\\TestStation5");
			break;
		case 6:
			TestCase = INVS_TEST_CASE_6;
			strcpy_s((char *)INVN_DataDir, INVS_MAX_DIR_PATH, "C:\\MT_Result\\InvnBase\\TestStation6");
			break;
		case 7:
			TestCase = INVS_TEST_CASE_7;
			strcpy_s((char *)INVN_DataDir, INVS_MAX_DIR_PATH, "C:\\MT_Result\\InvnBase\\TestStation7");
			break;
		default:
			printf("Incorrect Test Station No. in CurTestNo.txt!!\r\n");
			printf("Please check before continuing.\r\n");
			return -1;
	}

	INVS_MPT_RESULT TestRes = {};
	INVS_MPT_RESULT *pTestRes;
	pTestRes = &TestRes;

	// Make Data Directory
	std::string INVNDirstr((char*)INVN_DataDir);
	char MakeINVNDir[MTSTRINGMAX];
	sprintf_s(MakeINVNDir, MTSTRINGMAX, "mkdir %s", INVNDirstr.c_str());
	system(MakeINVNDir);	


	// ============ Main INVS Test ======================================
	rc = INVS_ExecuteTestCase(INVN_DataDir, SN, TestCase, pTestRes);
	// ==================================================================

	PassFailThisRun = ((rc == INVS_SUCCESS) ? "Pass" : "Fail");
	std::cout << "Pass Fail for this run is --> " << PassFailThisRun << std::endl;
	std::cout << "pDetailedResult -> sn is " << pTestRes->sn << std::endl;
	std::cout << "pDetailedResult -> casenum is " << pTestRes->caseNum << std::endl;
	std::cout << "pDetailedResult -> result is " << pTestRes->result << std::endl;
	std::cout << "pDetailedResult -> error count is " << pTestRes->errorCount << std::endl;
	std::cout << "pDetailedResult -> BinCode is " << pTestRes->binCodes << std::endl;

	return 0;
}

