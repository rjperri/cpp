// Copernican Solar System.cpp : Defines the entry point for the console application.
//


#include "Kepler_System.h"
#include <iostream>

using namespace std;


int main(){
	KeplerSystem SolarSystem;
	int month, day, year, planet;
	float ra, dec;
	cout << "Enter the date mm, dd, yyyy" << endl;
	cin >> month;
	cin >> day;
	cin >> year;
	SolarSystem.findNumberofDays(month, day, year);
	SolarSystem.calculateSolarSystem();
	planet = 0;
	for (int i = 0; i < 8; i++){
		SolarSystem.returnRADecforPlanet(i, ra, dec);
		cout << "Planet " << i+1 << ", RA = " << ra << ", DEC = " << dec << endl;
	}
	return 0;
}

