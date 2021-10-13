#include "bmp.h"

bool splitTokens(vector<double>& dvector, std::string& str, char token) {
	unsigned int count;
	char c;
	std::string tmpstr;

	dvector.clear();
	bool bflag = false;
	bool eflag = false;


	for (count = 0; count < str.length(); count++) {
		c = str[count];
		if (!bflag) {
			if (c == 'B') bflag = true;
			continue;
		}
		if (c == 'E') {
			eflag = true;
			if (count < (str.length() - 1)) str = str.substr(count + 1, str.length() - count - 1);
			else str = "";
			break;
		}

		if (c == token) {
			if (tmpstr.length()) {
				dvector.push_back(atof(tmpstr.c_str()));
				tmpstr = "";
			}
		}
		else {
			tmpstr = tmpstr + c;
		}

	}
	return (bflag && eflag);

}


float pctmap(float val, float minval, float maxval) {
	float range = maxval - minval;
	if (range == 0.0) return 0.0;
	float offset = val - minval;
	float pct = offset / range;
	return pct;

}

bool findcolor(float pct, int& r, int& g, int& b) {
	float fr = 255.0 * pct;
	r = int(fr);
	g = 255;
	b = 255 - r;
	return true;
}

bool findcolormap(float pct, int& r, int& g, int& b) {
	float fper = 1022.0 * pct;
	int offset = int(fper);
	if (offset < 0) offset = 0;
	if (offset > 1022) offset = 1022;
	int hexValue = lookupTable[offset];

	float fr = ((hexValue >> 16) & 0xFF);// / 255.0;  // Extract the RR byte
	float fg = ((hexValue >> 8) & 0xFF);// / 255.0;   // Extract the GG byte
	float fb = ((hexValue) & 0xFF);// / 255.0;        // Extract the BB byte
	r = int(fr);
	g = int(fg);
	b = int(fb);
	return true;
}



bool bmpmap(BMP& bmp, int pos, float reading, float minval, float maxval) {
	int x, y, r, g, b;

	float pct = pctmap(reading, minval, maxval);

	//findcolor(pct, r, g, b);

	findcolormap(pct, r, g, b);
	const int pixelsize = 4;
	x = (pos % 32) * pixelsize;
	y = (pos / 32) * pixelsize;

	bmp.fill_region(x, y, pixelsize, pixelsize, g, b, r, 255);
	// std::cout << x << "," << y << "\n";
	return true;

}
