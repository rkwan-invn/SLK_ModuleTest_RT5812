#include <stdint.h>
#include <stdlib.h>
#include <array>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <io.h>
#include <iostream>
#include <sstream>
#include <numeric>


//#include "ModuleTestDLL_Defs.h"

#pragma once

#define MTDLL_OK				0
#define MTDLL_ERROR				-1
#define MTDLL_ERROR_FILEIO		-2
#define MTDLL_ERROR_CALI		-3
#define MTDLL_ERROR_REGS		-4
#define MTDLL_ERROR_POWER		-5


#ifdef MODULETESTDLL_EXPORTS
#define MTDLLAPI __declspec(dllexport) 
#else
#define MTDLLAPI __declspec(dllimport) 
#endif



#define ROOTDIR "..\\fpsys_python"
#define REGSDBFILE ROOTDIR "\\System\\FP001\\regmap.xml"
#define DMPINFOFILE ROOTDIR "\\Core\\FW\\dmp4_info.csv"

// DMP 1.1.2
#define DMPCODEFILE ROOTDIR "\\Core\\FW\\dmp4_sherlock_ram_1_1_2.bin"
#define DSPSYMFILE ROOTDIR "\\Core\\FW\\dmp4_sherlock_data_map_1_1_2.s"
#define DMPSTACK_ADDRESS 0x1C000
#define IMAGE_ADDRESS 0x19EC6

#define TXRXFILE ROOTDIR "\\System\\TxRxPatterns\\BF3x3FullRx.csv"
#define GOLDENTRIMFILE ROOTDIR "\\System\\FP001\\GoldenTrim_A1A1.csv"
#define FLATCOMPMAP ROOTDIR "\\System\\FP001\\CompMap-105.csv"

#define OTPMAPFILE ROOTDIR "\\System\\FP001\\DE-000778 FP001_OTPMap_v1.3.csv"
#define OTPGOLDENTRIMLISTFILE ROOTDIR "\\System\\FP001\\DE-000778 FP001_GoldenTrim_v1.3.csv"

#define MTSTRINGMAX 256
#define MTTRIMREGMAX 512
#define MTOTPMAPREGMAX 512
#define MTOTPGOLDENTRIMLISTMAX 256
#define MTOTPPAGESIZE 24576
#define MTSESSIONMAX 8



extern "C"
{
	//@brief	Print "This is From MT DLL, Hello World~~ " to console
	//@detail	To check if MT DLL is present
	//@param	sid - session id
	//@retval	MTDLL_OK - success, always
	int MTDLLAPI MT_HelloWorld(int sid);

	//@brief	Check DLL version
	//@detail	Return version No. for ifplib.DLL and MT DLL
	//@param	ifplibver(OUTPUT) - ptr to uint32_t stores ifplib.DLL version
	//			MTDLLver(OUTPUT) - ptr to uint32_t stores MTDLL version
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_ChkDLLVer(uint32_t* ifplibver, uint32_t* MTDLLver);

	//@brief	Check Nucleo COM port (FOR Nucleo ONLY)
	//@detail	Check the number of Nucleo testers that are connected, return their COM port number
	//@param	MYCOMPORT(OUTPUT) - ptr to uint64_t vector that store COM port value of Nucleo connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR_FILEIO - cannot open file that stores COM port information
	//int MTDLLAPI MT_Load_COM_Port(std::vector<uint64_t>& MYCOMPORT);
	int MTDLLAPI MT_Load_COM_Port(uint64_t MYCOMPORT[]);

	//@brief	Parse Cfg file
	//@detail	Parse test config file
	//@param	CFG_File - string contains path to Cfg file
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR_FILEIO - cannot open Cfg file
	//int MTDLLAPI MT_CfgFileParse(std::string CFG_File);
	int MTDLLAPI MT_CfgFileParse(const char CfgDir[], const char CFG_File[]);

	//@brief	Write Cfg info to log file
	//@detail	Write test config / limits to log file header 
	//@param	PortCnt - number of ports / testers connected
	//			SLKSWVer - SLK MT SW version, MSB16bit = Major version, LSB16bit = Minor Version
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_WrtCFGtoLog(int PortCnt, uint32_t SLKSWVer);

	//@brief	Write Cfg info to log files for each SN 
	//@detail	Write test config / limits to header of separate log files for each SN 
	//@param	PortCnt - number of ports / testers connected
	//			SNLogFile - char array stores separate log file name
	//			SLKSWVer - SLK MT SW version, MSB16bit = Major version, LSB16bit = Minor Version
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_WrtCFGtoSNLog(int PortCnt, char *SNLogFile, uint32_t SLKSWVer);

	//@brief	Initialize log
	//@detail	Initialize test result arrays, set them to 0
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR_FILEIO - cannot open log file
	int MTDLLAPI MT_InitLOG(int PortCnt);

	//@brief	Assign SN to device
	//@detail	Assign SN to each port / device connected
	//@param	PortCnt - number of ports / testers connected
	//			SNin -	usigned char array that stores SNs for testers connected
	//					For Nucleo testers:
	//						the order of SN is matching the order of COM port in the param COMPORTin[]
	//						of function MT_Chk_HW(), i.e. SN[i] will be assigned to DUT connected to COMPORTin[i] 
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Input_SN(int PortCnt, unsigned char *SNin);

	//@brief	Write test results to log file
	//@detail	
	//@param	PortCnt - number of ports / testers connected
	//			RunNum - current Run number
	//			Yield_All[] - array contains overall yield for each port. This will be written to log file
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_WriteLOG(int PortCnt, int RunNum, double Yield_All[]);

	//@brief	Write test results to log file for each SN
	//@detail	
	//@param	PortCnt - number of ports / testers connected
	//			SNLogFile - char array stores separate log file name
	//			RunNum - current Run number
	//			Yield_All[] - array contains overall yield for each port. This will be written to log file
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_WriteSNLOG(int PortCnt, char *SNLogFile, int RunNum, double Yield_All[]);


	//@brief	Initialize DLL
	//@detail	Initialize DLL and assign session id to each device connected.  
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Init(int PortCnt);

	//@brief	Finalized DLL
	//@detail	Finalized DLL, release session id for testers connected
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Finit(int PortCnt);


	//@brief	Power on DUT
	//@detail	Control tester device to power up DUT
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR_POWER - fail
	int MTDLLAPI MT_PowerONDUT(int PortCnt);

	//@brief	Power off DUT
	//@detail	Control tester device to shut down DUT 
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR_POWER - fail
	int MTDLLAPI MT_PowerOFFDUT(int PortCnt);

	//@brief	Power cycle DUT
	//@detail	Power off DUT, then power it back on
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR_POWER - fail
	int MTDLLAPI MT_PowerCycle(int PortCnt);

	//@brief	Load DMP to DUT
	//@detail	Load register map, DMP file, DMP symbol file to DUT, then initialize DMP
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_LoadDMP(int PortCnt);

	//@brief	Set GPIO Pin (FOR Nucleo ONLY)
	//@detail	Set GPIO "PXY = value" where X is port No. and Y is pin No.
	//@param	PortCnt - number of ports / testers connected
	//			port -	the port number of GPIO pin
	//			pin -	the pin number of GPIO pin
	//			value -	1 or 0, the value to be set to the GPIO pin
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	//int MTDLLAPI GPIOWrite(int PortCnt, uint8_t port, uint8_t pin, uint8_t value);

	//@brief	Read GPIO pin's value (FOR Nucleo ONLY)
	//@detail	Read GPIO pin "PXY"'s value, where X is port No. and Y is pin No.
	//@param	PortCnt - number of ports / testers connected
	//			port -	the port number of GPIO pin
	//			pin -	the pin number of GPIO pin
	//			value(OUTPUT) - the array stores GPIO readback value (1 or 0) for each device connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	//int MTDLLAPI GPIORead(int PortCnt, uint8_t port, uint8_t pin, uint8_t valueout[]);

	//@brief	Check tester device hardware
	//@detail	Check if tester device is up, open tester device, set DUT sensor type 
	//@param	PortCnt - number of ports / testers connected
	//			COMPORTin[] - array stores COM port numbers of connected testers (FOR Nucleo ONLY)
	//			COMPORT[](OUTPUT) - array stores COM port numbers of testers that are successfully opened (FOR Nucleo ONLY)
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Chk_HW(uint64_t COMPORTin[], const int PortCnt, uint64_t COMPORT[]);

	//@brief	Sort site (FOR Nucleo ONLY)
	//@detail	Use GPIO pins to detect site No. of each Nucleo connected
	//			Needed to have site sort module plugged onto J6 of Nucleo
	//@param	PortCnt - number of ports / testers connected
	//			COMPORTin[] - COM ports prior to site order sort
	//			COMPortout[](OUTPUT) - COM ports after site order sort
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_SortSite(int PortCnt, uint64_t COMPORTin[], uint64_t COMPORTout[]);

	//@brief	SPI_COMM Test
	//@detail	Perform SPI_COMM test, check if DUTs are up
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Chk_SPIComm(int PortCnt);

	//@brief	ASIC ID Test
	//@detail	Perform ASIC ID test, Read DUTs' DieID, WaferID, DieRev, lotID, AssmID
	//@param	PortCnt - number of ports / testers connected
	//			DieIDout[](OUTPUT) - int array stores DieIDs of DUTs that are up
	//			WaferIDout[](OUTPUT) - int array stores Wafer ID of DUTs that are up
	//			LotIDout[](OUTPUT) - char array stores Lot ID of DUTs that are up
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Chk_ASICID(int PortCnt, int DieIDout[], int WaferIDout[], char *LotIDout);


	//@brief	Check temperature
	//@detail	Read DUT's temperature
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Chk_Temp(int PortCnt);

	//@brief	IDD Test (FOR Nucleo ONLY)
	//@detail	Perform power rail current test
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Chk_IDD(int PortCnt);

	//@brief	Image Scan
	//			!!!!! Note !!!!! 
	//			Need to perform calibration by calling int MTDLLAPI MT_Calibration(int PortCnt) prior to calling this function
	//			!!!!!!!!!!!!!!!!
	//@detail	Perform image scan and save image samples to bmp files
	//@param	PortCnt - number of ports / testers connected
	//			SNLogFile - char array that stores the name of the Image sample bmp files
	//			fnappd - char / string that will be appended to the Image sample file name
	//			GUIShow - switch for showing the image sample GUI or not.
	//			NumFrame - frame number of images that will be scanned
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_ImageScan(int PortCnt, char *SNLogFile, char* fnappd, bool GUIShow, int NumFrame);


	//@brief	Gain Only Image Scan
	//			!!!!! Note !!!!! 
	//			Need to perform calibration by calling int MTDLLAPI MT_Calibration(int PortCnt) prior to calling this function
	//			!!!!!!!!!!!!!!!!
	//@detail	Perform gain only (offset = 0) image scan and save image array to internal log
	//			Calculate image score, noise map, perform cluster check, save results to internal log
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_GainOnlyImageScan(int PortCnt);

	//@brief	Air Image Scan
	//			!!!!! Note !!!!! 
	//			Need to perform calibration by calling int MTDLLAPI MT_Calibration(int PortCnt) prior to calling this function
	//			!!!!!!!!!!!!!!!!
	//@detail	Perform air (no target / no fake finger) image scan and save image array to internal log
	//			Calculate image score, noise map, zone noise, noise NUM1, noise NUM2, and save results to internal log
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_AirImageScan(int PortCnt);

	//@brief	Target Image Scan
	//			!!!!! Note !!!!! 
	//			Need to perform calibration by calling int MTDLLAPI MT_Calibration(int PortCnt) prior to calling this function
	//			Need to perform Air Image Scan by calling int MTDLLAPI MT_AirImageScan(int PortCnt) prior to calling this function
	//			!!!!!!!!!!!!!!!!
	//@detail	Perform target / fake finger image scan and save image array to internal log
	//			Calculate image score, signal map, zone signal, perform signal cluster check
	//			Use noise information from Air Image Scan, caluculate SNR, zone SNR 
	//			Save results to internal log
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_TargetImageScan(int PortCnt);

	//@brief	Target Image Data to Buffer
	//			!!!!! Note !!!!! 
	//			Need to perform calibration by calling int MTDLLAPI MT_Calibration(int PortCnt) prior to calling this function
	//			Need to perform Air Image Scan by calling int MTDLLAPI MT_AirImageScan(int PortCnt) prior to calling this function
	//			!!!!!!!!!!!!!!!!
	//@detail	Perform target / fake finger image scan and output image data to buffer
	//@param	PortCnt - number of ports / testers connected
	//			ImageBuf[] - array with size of 8370*PortCnt image data will be save to
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_TargetImageToBuf(int PortCnt, uint8_t ImageBuf[]);

	//@brief	Configure Force Calibration setting
	//@detail	When set to 1, a DUT will be calibrated even if its OTP has been programmed.
	//			Call this function prior to calling the int MTDLLAPI MT_Calibration(int PortCnt) to configure the setting.
	//@param	ForceCali - Force Calibration setting value. 
	//				1 = Force Calibrate OTPed DUT. 
	//				0 = No calibration will be performed for OTPed DUT
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Set_Force_Cali(int ForceCali);

	//@brief	Perform Calibration
	//@detail	Calibrate DUTs
	//			For non-OTPed DUT, regular calibartion percedure will be performed.
	//			For OTPed DUT, trim values will be extracted from OTP to compose calibration result file
	//			OTPed DUT will be calibrated if Force Calibration is set to 1 by int MTDLLAPI MT_Set_Force_Cali(int ForceCali), 
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Calibration(int PortCnt);

	//@brief	Program OTP
	//@detail	Program DUT's OTP
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_OTPWrite(int PortCnt);

	//@brief	Check current run's test result
	//@detail	Return Module Test result of current run
	//@param	PortCnt - number of ports / testers connected
	//			PassFailThisRun(OUTPUT) - array returns DUTs' Module Test result for current run
	//				1 = PASS
	//				0 = FAIL
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Chk_TestResult(int PortCnt, int PassFailThisRun[]);

	//@brief	Display test result
	//@detail	Display Current run number, Port number, DieID, SN, current run's result Pass/Fail, 
	//			Overall number of passed runs, overall Yield, BINCode for current run	
	//			!!!!! Note !!!!!
	//			Run number, Overall number of passed runs and overall Yield need to be kept track and input by user
	//			!!!!!!!!!!!!!!!!
	//@param	PortCnt - number of ports / testers connected
	//			RunNum - current run number
	//			PassFailOverall - array stores overall number of runs with passed result for each port / tester
	//			Yield_All - array stores overall yield for each port / tester
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_DisplayResult(int PortCnt, int RunNum, int PassFailOverall[], double Yield_All[]);

	//@brief	Get Date
	//@detail	Return a string contain current date in format of YYYYMMDD
	//@param	DataCode(OUTPUT) - ptr to a string contains current date code
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	//int MTDLLAPI MT_GetDate(std::string* DateCode);
	int MTDLLAPI MT_GetDate(char DateCode[]);
	//@brief	Get Time
	//@detail	Return a string contain current time stamp in format of HH:MM:SS
	//@param	TimeStamp(OUTPUT) - ptr to a string contains current time stamp
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	//int MTDLLAPI MT_GetTime(std::string* TimeStamp);
	int MTDLLAPI MT_GetTime(char TimeStamp[]);

	//@brief	Set Flex Mode
	//@detail	Set test to perform Flex Mode / Bare Sensor Test
	//			This will over write "Flex Mode" setting in Cfg file
	//@param	FlexMode_in - Flex Mode setting. 1 = Flex_Mode; 0 = Module Test Mode
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Set_FlexMode(int FlexMode_in);

	//@brief	Set data directory
	//@detail	
	//@param	DataDir_in - unsigned char array that contains path to data directory
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Set_DataDir(unsigned char *DataDir_in);

	//@brief	Return bin code
	//@detail	Return bin code of each device for current run
	//@param	PortCnt - number of ports / testers connected
	//			BinCodeout(OUTPUT) - ptr to string that contain bin code of each device
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Chk_BinCode(int PortCnt, char *BinCodeout);

	//@brief	Check DUT Pass Bit
	//			!!!!! NOTE !!!!!
	//			Need to check DUT ASIC ID by calling int MTDLLAPI MT_Chk_ASICID(int PortCnt, int DieIDout[]) prior to calling this function
	//			Need to assign DUT SN by calling int MTDLLAPI MT_Input_SN(int PortCnt, unsigned int SNin[]) prior to calling this function
	//			!!!!!!!!!!!!!!!!
	//@detail	Check DUT's ST Pass Bit, MT Pass Bit from OTP
	//			Also will return DUT's DieID and SN store in internal log
	//@param	PortCnt - number of ports / testers connected
	//			STPassout(OUTPUT) - array contains ST pass bit value for each DUT
	//			MTPassout(OUTPUT) - array contains MT pass bit value for each DUT
	//			DieIDout(OUTPUT) - array contains DieID for each DUT
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Chk_PassBit(int PortCnt, int STPassout[], int MTPassout[], int DieIDout[]);

	//@brief	Check OTP Content
	//@detail	Check OTP content against corrent values list
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Chk_OTPContent(int PortCnt);

	//@brief	Check Encryption Block
	//@detail	Check Chip's Encryption Block
	//@param	PortCnt - number of ports / testers connected
	//@retval	MTDLL_OK - success
	//			MTDLL_ERROR - fail
	int MTDLLAPI MT_Chk_Encrypt(int PortCnt);
}




