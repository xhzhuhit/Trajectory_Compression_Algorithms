#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

using namespace std;
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <assert.h>
#include <string>
#include <string.h>


typedef map<string,string> ConfigType;

void TrimSpace(char* str);
void AddConfigFromFile(ConfigType &cr,const char* filename);
void AddConfigFromCmdLine(ConfigType &cr,int argc,char** argv);
void ListConfig(ConfigType &cr);

float getConfigFloat(const char* key,ConfigType &cr,bool required=true,float _default=0);
int getConfigInt(const char* key,ConfigType &cr,bool required=true,int _default=0);
const char* getConfigStr(const char* key,ConfigType &cr,bool required=true,const char* _default=NULL);


#endif