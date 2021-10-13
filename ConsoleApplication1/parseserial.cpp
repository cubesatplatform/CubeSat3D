#pragma once
#include <iostream>

#include "parseserial.h"

SIRArray CParseSerial::parse(std::string& str) {
	SIRArray ir;


	std::string::size_type n;
	n = str.find("offset:", 0);

	if (n != std::string::npos) {
		std::string iroffset = str.substr(n + 7, 1).c_str();
		n = str.find("data:", n);

		if (n != std::string::npos) {
			std::string irdata = str.substr(n + 5).c_str();

			n = irdata.find("~");
			if (n != std::string::npos) {
				str = irdata.substr(n);
				irdata = irdata.substr(0, n);
				ir.offset = iroffset;
				ir.data = irdata;

			
			}
			/*
			Serial.println("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
			Serial.print("Offset:");
			Serial.println(iroffset.c_str());
			Serial.print("Data:");
			Serial.println(irdata.c_str());
			Serial.println("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
			*/
		}
	}
	return ir;

}


//std::string xx = "data:asdasasd~mn:12~sd:aaaaa";
//std::string data = parse("mn:", xx);
std::string CParseSerial::parse(const char * cfind, std::string &str) {
	
	//"offset:"
	std::string data, find = cfind;
	std::string::size_type  b, n;
	b= str.find(find, 0);

	if (b != std::string::npos) {

		n = str.find("~", b);

		if (n != std::string::npos) {
			data = str.substr(b + find.size(),n-b-find.size());
			str = str.substr(n + 1);
			}
	}
	return data;

}


void CParseSerial::setup() {
	arduino = new Serial(portName);  
}//Doesnt matter   edit direct


void CParseSerial::loop(){
	Sleep(110);
	
	if (arduino->IsConnected()) {
		int hasRead = arduino->ReadData(receivedString, DATA_LENGTH);
		if (hasRead) {
			if (strbuf.length() > 5000) strbuf = "";
			strbuf = strbuf + receivedString;

			
			SIRArray sir = parse(strbuf);

			if (sir.offset == "0") {
				SIRArray empty;
				IRArray[0] = sir;
				IRArray[1] = empty;
				IRArray[2] = empty;
				IRArray[3] = empty;
			}
			if (sir.offset == "1") IRArray[1] = sir;
			if (sir.offset == "2") IRArray[2] = sir;
			if (sir.offset == "3") IRArray[3] = sir;

			std::cout << receivedString << "Got stuff\n";
			//std::string p[] = splitTokens(strbuf, ", ");
			/*
			if (splitTokens(dvector, strbuf, ',') && dvector.size()) {

				BMP bmp2(140, 110);
				float minval, maxval, tmpval;
				minval = dvector[0];
				maxval = minval;
				for (pos = 1; pos < dvector.size(); pos++) {
					tmpval = dvector[pos];
					if (tmpval < minval)minval = tmpval;
					if (tmpval > maxval)maxval = tmpval;
				}

				//bmp2.fill_region(60, 40, 100, 200, 0, 0, 255, 255);    //B,G,R, A
				//bmp2.fill_region(4, 4, 4, 4, 255, 0, 255, 255);

				if (maxval > 30.0) maxval = 30.0;

				for (pos = 0; pos < dvector.size(); pos++) bmpmap(bmp2, pos, dvector[pos], minval, maxval);
				std::string name;
				std::string pstr = std::to_string(pcount);
				name = "img" + pstr;
				name += ".bmp";
				bmp2.write(name.c_str());
				pcount++;			
			}
			*/
		}
	}
}
