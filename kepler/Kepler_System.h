//Robert Perri
//Feb 5, 2010
//Uses the model developed by kepler to calculate the positions of the planets
//Three assumptions are made
// Kepler's 1) planets orbit the sun in ellipses with the sun a one focus 
// Kepler's 2) planets sweep out equal areas with time
// Kepler's 3) P^2 = A^3 is true here.
//The math libraies only work in radians so there will be lots and lots of converting
// between radians and degrees. Any time you see a (180/PI), that means we're in degrees. 
// And any time you see (PI/180), that means we're in Radians. Also I'm stubborn fool, I like 
// music hard rockin, and my angles between 0 and 2PI. I know, I know it doesn't matter if the
// angle isn't between 0 and 2PI, but you tell me where the hell -37.6789506432385764 rads is?
// Also I can't read rads, so when i'm testing the angles out, I convert them to degs, cause 
// once again, you tell me where 4.56789234678549402938755 rads is? GOT IT. 
// Yes, I know I misspelled 'angel' so suck it!
//Calculating the date is a lot more complicated than it sounds
//Let's see what else...oh yeah...PLUTO IS NOT A PLANET!!!!
//



#ifndef  KEPLERSYSTEM_H
#define  KEPLERSYSTEM_H
#include <vector>
#include <cmath>
//#include <iostream>

using namespace std;

struct RADec{
	float RA;
	float Dec;
};

struct Planet{
	//To Kepler this all you need to be a planet
	//Stuff you need
	float inc;
	float ascendLong;
	float perhLong;
	float meanDist;
	float dailyMotion;
	float eccen;
	float meanLong;
	//Stuff you calculate
	float meanAnom;
	float trueAnom;
	float radiusVector;
	float X;
	float Y;
	float Z;
	RADec coord;
};
const float EPOCH = -864.5; //-1224.5; //-864.5; 8/20/1997?
const float PI = 4*atan(1.0);
const float RAD = (PI/180);
const float DEG = (180/PI);
const float ECLIPTIC = 23.4393*RAD;
const int EARTH = 2;
int daysInYear [] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334}; 

class KeplerSystem{
private:
	vector <Planet> planets;
	float numberOfDays;
	void calculateMeanAnomalyforAllPlanets(){
		//Calculate the mean Anomaly. If the orbit was like a perfect circle what would the
		// andlge be on the given date. You take the daily motion and multipy it by the number of days
		// then add it to the number from when it was first observed Mean Longitude, and
		// then subtract the angel at whcih the perhilion is. Perhilion is where the sun is
		// closest to the planet
		//Kepler's 3rd law comes to play here
		//cout << numberOfDays << endl << "MeanAnomaly" << endl;
		//  
		for(int i = 0; i < 8; i++){
			planets.at(i).meanAnom = planets.at(i).dailyMotion * numberOfDays + planets.at(i).meanLong - planets.at(i).perhLong;
			//Keep the angels between 0 and 2PI for easier testing. I know I don't really need to do this
			//But it helps on trying to figure out just where the terryfunk the angel is
			if(planets.at(i).meanAnom < 0)
				while(planets.at(i).meanAnom < 0)
					planets.at(i).meanAnom += 2*PI;
			else if(planets.at(i).meanAnom > 2*PI)
				while(planets.at(i).meanAnom > 2*PI)
					planets.at(i).meanAnom -= 2*PI;
			//cout << planets.at(i).meanAnom * DEG << "   " << planets.at(i).dailyMotion *DEG << endl;
			
		}
	}
	float findEccentricAnomaly(float ma, float ec){
		//This is going to attempted o solve Kepler's Equation using the itterative method
		// M = E - e sin(E) what we want is E we are given M and e
		float EA = ma;
		for(int i = 0; i < 14; i++){
			EA = ma + ec * sin(EA); //kepler's equation EA is the eccentric anomaly
		}
		return EA;
	}
	void calculateTrueAnomalyforAllPlanets(){
		//Calculates the true anomaly which is the most piss poor worded term
		// the true anomaly is the angle between the planet, sun, and perheioln
		// Pretty complicated to solve so it must calculate the Eccentric Anomaly iterativly
		// Now I included a 2nd way ot solve it called the equation of the Center. I have 
		// no idea how it works, but it's pretty accurate. I perfer the first way better, cause 
		// that's how kepler would have done it, and keps the man
		// This is where Kepler's 2nd Law comes to play
		//cout << "True anomaly" << endl;
		float ma = 0;
		float ec = 0;
		float ea = 0;
		for(int i = 0; i < 8; i++){
			ea = findEccentricAnomaly(planets.at(i).meanAnom, planets.at(i).eccen);
			planets.at(i).trueAnom = atan(sqrt((1+planets.at(i).eccen)/(1-planets.at(i).eccen))*tan(ea/2))*2;
			/* //Equation of the Center
			ma = planets.at(i).meanAnom;
			ec = planets.at(i).eccen;
			planets.at(i).trueAnom = ma + (2*ec - (ec*ec*ec)/4) * sin(ma) + 5/4*(ec*ec)*sin(2*ma)+ 13/12*(ec*ec*ec)*sin(3*ma); 
			*/
			if (planets.at(i).trueAnom < 0)
				planets.at(i).trueAnom += 2*PI;
			else if(planets.at(i).trueAnom > 2*PI)
				planets.at(i).trueAnom -= 2*PI;

			//cout << i << " : " << planets.at(i).trueAnom *DEG << endl;
		}
	}
	void calculateRadiusVectorforALLPlanets(){
		//Now we will calculate the distance between sun and planet at the angle of the true
		// anomaly Not hard of an equation, just uses some simple ellipse equation
		// This is where Kepler's 1st law comes to play
		for(int i = 0; i < 8; i++){
			planets.at(i).radiusVector = planets.at(i).meanDist*(1-planets.at(i).eccen*planets.at(i).eccen)/(1+planets.at(i).eccen*cos(planets.at(i).trueAnom));
		}
	}
	
	void calculateXYZforAllPlanets(){
		//uses simple trig functions to find an x,y,z coordinate of a planet with the sun
		//at 0,0,0 Now the funny thing is the calculationsare really big so I'm gonna
		// use varibles to break the equations down some
		//cout << endl << "X,Y,Z " << endl; 
		float r, lan, ta, lp, inc;
		for(int i = 0; i < 8; i++){
			r = planets.at(i).radiusVector;
			lan = planets.at(i).ascendLong;
			ta = planets.at(i).trueAnom;
			lp = planets.at(i).perhLong;
			inc = planets.at(i).inc;
			if(i != EARTH){ 
				planets.at(i).X = r*(cos(lan)*cos(ta+lp-lan) - sin(lan)*sin(ta+lp-lan)*cos(inc));
				planets.at(i).Y = r*(sin(lan)*cos(ta+lp-lan) + cos(lan)*sin(ta+lp-lan)*cos(inc));
				planets.at(i).Z = r*(sin(ta+lp-lan)*sin(inc));
			}
			else if(i == EARTH){ //Earth's calculations are a bit diferent
				planets.at(i).X = r*(cos(ta+lp));
				planets.at(i).Y = r*(sin(ta+lp));
				planets.at(i).Z = 0;
			}
			//cout << i << " : " << planets.at(i).X << "   " << planets.at(i).Y << "   " << planets.at(i).Z << endl;
		}
	}
	void translateXYZforEarthCenter(){
		//cout << endl << "Translate X,Y,Z " << endl; 
		//Change X,Y coordiantes so that the Earth is the center of the solar system
		for(int i = 0; i < 8; i++){
			if(i != EARTH){
				planets.at(i).X -= planets.at(EARTH).X;
				planets.at(i).Y -= planets.at(EARTH).Y;
				planets.at(i).Z -= planets.at(EARTH).Z;
				//cout << i << " : " << planets.at(i).X << "   " << planets.at(i).Y << "   " << planets.at(i).Z << endl;
			}
		}
	}
	void rotateYZaroundEclipticforAllPlanets(){
		//We need to take notice about the ecliptic eventualy, so why not now?
		// Using simple 3d roating trigonmics we make the x axis the axis we want
		// to rotate around
		for(int i = 0; i < 8; i++){
			if(i != EARTH){
				planets.at(i).Y = planets.at(i).Y*cos(ECLIPTIC) - planets.at(i).Z*sin(ECLIPTIC);
				planets.at(i).Z = planets.at(i).Y*sin(ECLIPTIC) + planets.at(i).Z*cos(ECLIPTIC);
			}
		}
	}
	void calculateRADecforAllPlanets(){
		//Calculates The RA and DEC with relation to the Ecliptic, angels need to be converted to deg
		//RA also needs to be devided by 15 to be in hrs
		//cout << endl << "Ra'sand stuff" <<endl;
		for(int i = 0; i < 8; i++)
			if(i != EARTH){
				float Y = planets.at(i).Y;
				float X = planets.at(i).X;
				float Z = planets.at(i).Z;
				//Calculate RA put it in degs then divide by 15 to get hrs
				float ra = atan(Y/X)*DEG;
				if(X < 0){
					planets.at(i).coord.RA = (ra+180)/15;
					//cout << " X < 0 " << i+1 << endl;
				}
				else if(Y < 0 && X > 0){
					planets.at(i).coord.RA = (ra+360)/15;
					//cout << " Y < 0 and X > 0 " << i+1 << endl;
				}
				else {
					planets.at(i).coord.RA = (ra)/15;
					//cout << "else " << i+1 << endl;
				}
				//Calculate Dec put in Degs
				planets.at(i).coord.Dec = atan(Z/sqrt(X*X + Y*Y))*DEG;
			}
	}
public:
	void findNumberofDays(int month, int day, int year){
		//Calculates the number of days since aug 25, 1996
		//The easiest way is count the number of days since Jan 1 2000 for both dates and 
		//subtract them
		//8/25/96 = EPOCH = -1224.5
		//The amount of days in a year till a given month is held in an array cause it is way
		// easier to calculate it that way tahn to figure out some stupid formula
		//Make sure this is not a leap yeap, and watch out for those pesky super leap years!
		bool leapYear = false;
		if(year % 4 == 0 && year % 1000 != 0)
			leapYear = true;
		//calculates the number of days in the years since 2000
		float daysSince2000 = (year - 2000) * 365 - 1.5 + (int)((year-2000)/4);
		//adds the days in years, months, and days all together
		float daysOfPos = daysInYear[month-1] + day + daysSince2000;
		//if it is a pesky leap year and is over feb, add 1
		if (leapYear && month > 2)
			daysOfPos++;
		//Finally we can calculate the number of days from 8/25/96
		numberOfDays = daysOfPos - EPOCH;
		/*cout << numberOfDays << endl;
		int a;
		cin >> a;*/
	}
	void calculateSolarSystem(){
		//calls all the various functions in the right order to calclate the solarsystem
		calculateMeanAnomalyforAllPlanets();	//Kepler's 3rd
		calculateTrueAnomalyforAllPlanets();	//Kepler's 2nd
		calculateRadiusVectorforALLPlanets();	//Kepler's 1st
		calculateXYZforAllPlanets();			//The rest of the functions put the planets
		translateXYZforEarthCenter();			// through the perspective of Earth
		rotateYZaroundEclipticforAllPlanets();	//	
		calculateRADecforAllPlanets();			//
	}
	void returnRADecforPlanet(int P, float &ra, float &dec){
		ra = planets.at(P).coord.RA;
		dec = planets.at(P).coord.Dec;
	}
	KeplerSystem(){
		//constructor, initilizes ervreything
		Planet Mercury = {
			7.00507*RAD,		//Inclicantion
			48.3339*RAD,		//Longitude of Ascending node
			77.45399*RAD,		//Longitude of Perhielion
			.3870978,			//Mean Distanc
			4.092353*RAD,		//Daily motion in RADS/DAY
			.2056324,			//eccenticity
			314.42369*RAD,		//mean Longitude
			0,0,0,0,0,0
		};
		planets.push_back(Mercury);
		Planet Venus = {
			3.39472*RAD,		//Inclination
			76.6889*RAD,		//Longitude of Asceding Node
			131.761*RAD,		//Longitude of Perhielion
			.7233238,			//mean Distanse
			1.602158*RAD,		//daily motion in Rads/day
			.0067933,			//eccentricty
			236.94045*RAD,		//mean Longitude
			0,0,0,0,0,0
		};
		planets.push_back(Venus);
		Planet Earth = {
			.00041*RAD,		//inclination
			349.2*RAD,		//Longitude of Ascendiong Node
			102.8517*RAD,	//Longitude of Peherhiol
			1.00002,		//mean distancs
			.9855769*RAD,	//daily motion in rads/day
			.0166967,		//eccentricity
			328.40353*RAD,	//mean Longitude
			0,0,0,0,0,0
		};
		planets.push_back(Earth);
		Planet Mars = {
			1.84992*RAD,		//inclination
			49.5664*RAD,		//Longitude of Ascendiong Node
			336.0882*RAD,	//Longitude of Peherhiol
			1.5236365,		//mean distancs
			.5240613*RAD,	//daily motion in rads/day
			.0934231,		//eccentricity
			262.42784*RAD,	//mean Longitude
			0,0,0,0,0,0
		};
		planets.push_back(Mars);
		Planet Jupiter = {
			1.30463*RAD,		//inclination
			100.4713*RAD,		//Longitude of Ascendiong Node
			15.9678*RAD,	//Longitude of Peherhiol
			5.202597,		//mean distancs
			.08309618*RAD,	//daily motion in rads/day
			.0484646,		//eccentricity
			322.55983*RAD,	//mean Longitude
			0,0,0,0,0,0
		};
		planets.push_back(Jupiter);
		Planet Saturn = {
			2.48524*RAD,		//inclination
			113.6358*RAD,		//Longitude of Ascendiong Node
			88.863*RAD,	//Longitude of Peherhiol
			9.57189999,		//mean distancs
			.03328651*RAD,	//daily motion in rads/day
			.0531651,		//eccentricity
			20.95759*RAD,	//mean Longitude
			0,0,0,0,0,0
		};
		planets.push_back(Saturn);
		Planet Uranus = {
			.77343*RAD,		//inclination
			74.0954*RAD,		//Longitude of Ascendiong Node
			175.6807*RAD,	//Longitude of Peherhiol
			19.30181,		//mean distancs
			.01162295*RAD,	//daily motion in rads/day
			.0428959,		//eccentricity
			303.18967*RAD,	//mean Longitude
			0,0,0,0,0,0
		};
		planets.push_back(Uranus);
		Planet Neptune = {
			1.7681*RAD,		//inclination
			131.7925*RAD,		//Longitude of Ascendiong Node
			7.206*RAD,	//Longitude of Peherhiol
			30.26664,		//mean distancs
			.005919282*RAD,	//daily motion in rads/day
			.0102981,		//eccentricity
			299.8641*RAD,	//mean Longitude
			0,0,0,0,0,0
		};
		planets.push_back(Neptune);
		numberOfDays = 0;
	}

			
};


#endif
