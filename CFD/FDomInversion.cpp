#include "CFDInversion.h"



#include<ctime>

int main()
{
	time_t timeBegin  ;
	time(&timeBegin) ;

	CFDInversion test ;


	time_t timeEnd ;
	time(&timeEnd) ;

	cout<<timeEnd - timeBegin<<endl ;

	long tempTime = timeEnd - timeBegin ;
	int hour = 0 ;
	int minute = 0 ;
	int second = 0 ;

	second = tempTime%60 ;
	tempTime /= 60 ;

	minute = tempTime%60 ;
	tempTime /= 60 ;
	hour = tempTime ;

	fstream f_write("time.txt" , fstream::out) ;

	f_write<<tempTime<<" : "<<minute<<" : "<<second<<endl ;

	f_write.close() ;

	cout<<tempTime<<" : "<<minute<<" : "<<second<<endl ;
	return 0 ;
}