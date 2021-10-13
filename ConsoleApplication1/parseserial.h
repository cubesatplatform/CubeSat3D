#pragma once

#include <vector>
#include <string>
#include "serialport.h"

using namespace std;
//const char portName[] = "\\\\.\\COM10";   //This doesnt work  Edit the function directly
const char portName[] = "COM9";   //This doesnt work  Edit the function directly

struct SIRArray {
	std::string offset;
	std::string data;
	std::string min;
	std::string max;
	SIRArray() { offset = ""; data = ""; min = ""; max = ""; }
};

class CParseSerial {
#define DATA_LENGTH  255
#define MAX_DATA_LENGTH  255

	char incomingData[MAX_DATA_LENGTH];
	char receivedString[DATA_LENGTH];

	//Arduino SerialPort object
	Serial* arduino=NULL;

	std::string strbuf;

	int pos = 0;
	int pcount = 0;

	vector<double> dvector;

public:
	SIRArray IRArray[4];

	CParseSerial() {}
	~CParseSerial() {delete arduino;	}
	void setup();//Doesnt matter   edit direct
	void loop();
	SIRArray parse(std::string & str);
	std::string parse(const char* cfind, std::string &str);
	

};