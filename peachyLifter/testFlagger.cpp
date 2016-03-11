#include "Flagger.h"
//#define _TEST
#ifdef _TEST
#include <iostream>

int main()
{
	using namespace std;
	uint8_t tmp_flag;

	cout << "Hello World" << endl;
	Flagger MyFlagger;
	MyFlagger.registerFlag(20);

	tmp_flag=MyFlagger.getFlag(0);
	cout << "Flag State: "<< (int)tmp_flag << endl;

	tmp_flag=MyFlagger.getFlag(7);
	cout << "2nd Flag State: "<< (int)tmp_flag << endl;


}

#endif
