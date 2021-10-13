#pragma once
#include <vector>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include "serialport.h"
#include "serialbitmap.h"

using namespace std;

bool splitTokens(vector<double>& dvector, std::string& str, char token);
float pctmap(float val, float minval, float maxval);
bool findcolor(float pct, int& r, int& g, int& b);
bool findcolormap(float pct, int& r, int& g, int& b);
bool bmpmap(BMP& bmp, int pos, float reading, float minval, float maxval);
