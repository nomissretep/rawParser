/*
 * Main.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: speters
 */
#include <time.h>
#include "rawParser.h"
#include <iostream>
using namespace std;
int main(int argc,char** argv)
{
	//if(argc!=2)
		//cout<<"Bitte richtige Parameter mitgeben"<<endl;
	//else
	{
		double time1,time2;
		time1=clock();
		rawParser<float> rp(argv[1]);
		rp.Read();
		time2=clock();
		cout<<"brauchte :"<< (time2 -time1)<<endl;
		//rp.Save(argv[2]);
	}
}



