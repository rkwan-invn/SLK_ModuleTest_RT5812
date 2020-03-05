#pragma once

extern "C" {

#define  INVS_SUCCESS							0
#define  INVS_FAILURE							-1

#define  INVS_MAX_RESERVED_INTERFACES			16
#define  INVS_MAX_DIR_PATH						64
//#define  INVS_MAX_BIN_ERROR_COUNT				16
#define  INVS_MAX_BIN_ERROR_COUNT				256
	typedef enum {
		INVS_POWER_ON = 0,
		INVS_POWER_OFF = 1,
		INVS_POWER_SLEEP = 2,
		INVS_POWER_DEEP_SLEEP = 3,
	} INVS_POWER_STATE;

	typedef enum {
		INVS_TEST_CASE_0,
		INVS_TEST_CASE_1,
		INVS_TEST_CASE_2,
		INVS_TEST_CASE_3,
		INVS_TEST_CASE_4,
		INVS_TEST_CASE_5,
		INVS_TEST_CASE_6,
		INVS_TEST_CASE_7,
		INVS_TEST_CASE_8,
	} INVS_TEST_CASE;

	typedef struct _tdk_invs_mp_test_result_ {
	  unsigned char				sn[MTSTRINGMAX];				// Sensor module serial number
	  INVS_TEST_CASE			caseNum;		// Current case number host executes	
	  unsigned int				result;			// MP testing result for this case: Pass - 0, Fail - 1;
	  unsigned int				errorCount;		// How many sub-cases failed or how many bin codes returned;
	  char						binCodes[INVS_MAX_BIN_ERROR_COUNT];   // Error bin codes array;
	} INVS_MPT_RESULT, *PINVS_MPT_RESULT;

	/* APIS for high level application usage */


	/* int INVS_ExecuteTestCase(unsigned char  *invsBase, unsigned int sn, INVS_TEST_CASE   testCase, PINVS_MPT_RESULT  *pDetailedResult)
	 * 
	 * Description: 
	 * this is the test case execution function, according to the case you input, 
	 * will execute different testing scenarios. 
	 * 
	 * Parameters: 
	 * invsBase - the base directory for the mass production testing logs and files
	 * sn       - the serial number of the sensor module
	 * testCase -  the test case number 
	 * pDetailedResult - the detailed testing result with bin codes
	 *
	 * Return: 
	 *  if succeed, no error happened , will return 0 , else will return the error code. 
	 * 
	 */
	int	INVS_ExecuteTestCase(unsigned char  *invsBase, unsigned char *sn, INVS_TEST_CASE   testCase, PINVS_MPT_RESULT  pDetailedResult);

}
