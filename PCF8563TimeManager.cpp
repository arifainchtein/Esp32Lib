/*
 * PCF8563TimeManager.cpp
 *
 *  Created on: 21 Mar 2021
 *      Author: arifainchtein
 */

#include <PCF8563TimeManager.h>
#include <Wire.h>

static const byte monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
#define PCF8563address 0x51
#define I2C_SDA 21
#define I2C_SCL 22

PCF8563TimeManager::PCF8563TimeManager( HardwareSerial& serial):TimeManager(serial ){}

void PCF8563TimeManager::hourlyTasks(long time, int previousHour ){

 }
 void PCF8563TimeManager::dailyTasks(long time, int yesterdayDate, int yesterdayMonth, int yesterdayYear ){

 }
 void PCF8563TimeManager::monthlyTasks(long time){

 }
 void PCF8563TimeManager::yearlyTasks(long time){

 }

// char* PCF8563TimeManager::epochToString(unsigned long epoch) {
//   static char str[20];
//   time_t t = epoch;
//   struct tm *tm = gmtime(&t);
//   sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d",
//     tm->tm_year + 1900,
//     tm->tm_mon + 1,
//     tm->tm_mday,
//     tm->tm_hour,
//     tm->tm_min,
//     tm->tm_sec);
//   return str;
// }

bool PCF8563TimeManager::setTime(RTCInfoRecord e){
	Wire.beginTransmission(PCF8563address);
	  Wire.write(0x02);
	  Wire.write(decToBcd(e.second));
	  Wire.write(decToBcd(e.minute));
	  Wire.write(decToBcd(e.hour));
	  Wire.write(decToBcd(e.date));
	  Wire.write(decToBcd(e.dayOfWeek));
	  Wire.write(decToBcd(e.month));
	  Wire.write(decToBcd(e.year));
	  Wire.endTransmission();
}

bool PCF8563TimeManager::setTime(String command){
	//SetTime#24#10#19#4#17#32#00
	int date = GeneralFunctions::getValue(command, '#', 1).toInt();
	int month = GeneralFunctions::getValue(command, '#', 2).toInt();
	int year = GeneralFunctions::getValue(command, '#', 3).toInt();
	int dayOfWeek = GeneralFunctions::getValue(command, '#', 4).toInt();
	int hour = GeneralFunctions::getValue(command, '#', 5).toInt();
	int minute = GeneralFunctions::getValue(command, '#', 6).toInt();
	int second = GeneralFunctions::getValue(command, '#', 7).toInt();

	_HardSerial.print(date);
	_HardSerial.print("/");
		_HardSerial.print(month);
		_HardSerial.print("/");
		_HardSerial.print(year);
		_HardSerial.print(" ");
		_HardSerial.print(hour);
		_HardSerial.print(":");
		_HardSerial.print(minute);
		_HardSerial.print(":");
		_HardSerial.print(second);


		Wire.beginTransmission(PCF8563address);
		  Wire.write(0x02);
		  Wire.write(decToBcd(second));
		  Wire.write(decToBcd(minute));
		  Wire.write(decToBcd(hour));
		  Wire.write(decToBcd(date));
		  Wire.write(decToBcd(dayOfWeek));
		  Wire.write(decToBcd(month));
		  Wire.write(decToBcd(year));
		  Wire.endTransmission();




	//	setTime(hour,min,sec,date,month,year);
	//	RTC.set(now());
	//getTime();
	return true;

}

RTCInfoRecord PCF8563TimeManager::now(){

		Wire.beginTransmission(PCF8563address);
		Wire.write(0x02);
		Wire.endTransmission();
		Wire.requestFrom(PCF8563address, 7);
		RTCInfoRecord aRTCInfoRecord;
		aRTCInfoRecord.second     = bcdToDec(Wire.read() & B01111111); // remove VL error bit
		aRTCInfoRecord.minute     = bcdToDec(Wire.read() & B01111111); // remove unwanted bits from MSB
		aRTCInfoRecord.hour       = bcdToDec(Wire.read() & B00111111);
		aRTCInfoRecord.date =       bcdToDec(Wire.read() & B00111111);
		aRTCInfoRecord.dayOfWeek  = bcdToDec(Wire.read() & B00000111);
		aRTCInfoRecord.month      = bcdToDec(Wire.read() & B00011111);  // remove century bit, 1999 is over
		aRTCInfoRecord.year       = 2000+bcdToDec(Wire.read());
		aRTCInfoRecord.timezoneInfo=timezoneInfo;
		aRTCInfoRecord.timezoneOffset=getTimezoneOffset(aRTCInfoRecord.year ,aRTCInfoRecord.month ,aRTCInfoRecord.date) ;	
		aRTCInfoRecord.epoch=TimeUtils::getEpochTime( aRTCInfoRecord.year,  aRTCInfoRecord.month,  aRTCInfoRecord.date,  aRTCInfoRecord.hour,  aRTCInfoRecord.minute,   aRTCInfoRecord.second);
		return aRTCInfoRecord;
	}


bool PCF8563TimeManager::printTimeToSerial(RTCInfoRecord aRTCInfoRecord){



	_HardSerial.print(aRTCInfoRecord.date);
	_HardSerial.print("/");
	_HardSerial.print(aRTCInfoRecord.month);
	_HardSerial.print("/");
	_HardSerial.print(aRTCInfoRecord.year);
	_HardSerial.print(" ");
	_HardSerial.print(aRTCInfoRecord.hour);
	_HardSerial.print(":");
	_HardSerial.print(aRTCInfoRecord.minute);
	_HardSerial.print(":");
	_HardSerial.print(aRTCInfoRecord.second);


	return true;
}


//
// End of Functions that represents commands received via the serial port
//


void PCF8563TimeManager::start(){
	// strncpy(timezoneInfo, tz, sizeof(timezoneInfo) - 1);
    // timezoneInfo[sizeof(timezoneInfo) - 1] = '\0';
}


long PCF8563TimeManager::dateAsSeconds(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second){


	// note year argument is full four digit year (or digits since 2000), i.e.1975, (year 8 is 2008

	int i;


	if(year < 69){
		year+= 2000;
	}

	// seconds from 1970 till 1 jan 00:00:00 this year
	long seconds= (year-1970)*(60*60*24L*365);

	// add extra days for leap years
	for (i=1970; i<year; i++) {
		if (LEAP_YEAR(i)) {
			seconds+= 60*60*24L;
		}
	}
	// add days for this year
	for (i=0; i<month; i++) {
		if (i==1 && LEAP_YEAR(year)) {
			seconds+= 60*60*24L*29;
		} else {
			seconds+= 60*60*24L*monthDays[i];
		}
	}

	seconds+= (date-1)*3600*24L;
	seconds+= hour*3600L;
	seconds+= minute*60L;
	seconds -=  timeZoneHours*3600L;
	seconds+=second;
	return seconds;
}


long PCF8563TimeManager::getTimezoneOffset(const uint16_t year,uint8_t month,uint8_t date) {
        // Get standard time offset
        char* ptr = strchr(timezoneInfo, '-');
        if (!ptr) ptr = strchr(timezoneInfo, '+');
        if (!ptr) return 0;
        
        int std_offset = atoi(ptr + 1) * 3600;
        if (*ptr == '+') std_offset = -std_offset;

        // Parse DST rules
        char* dst_rules = strchr(timezoneInfo, ',');
        if (!dst_rules) return std_offset;
        
        int current_month = month;  // 1-12
        int current_day = date;     // 1-31

        // Parse DST start rule (M10.1.0)
        dst_rules++; // Skip comma
        int dst_start_month = atoi(dst_rules + 1);
        char* week_ptr = strchr(dst_rules, '.') + 1;
        int dst_start_week = atoi(week_ptr);
        char* day_ptr = strchr(week_ptr, '.') + 1;
        int dst_start_day = atoi(day_ptr);

        // Parse DST end rule (M4.1.0)
        char* end_rule = strchr(dst_rules, ',') + 1;
        int dst_end_month = atoi(end_rule + 1);
        week_ptr = strchr(end_rule, '.') + 1;
        int dst_end_week = atoi(week_ptr);
        day_ptr = strchr(week_ptr, '.') + 1;
        int dst_end_day = atoi(day_ptr);

        // Calculate actual start and end days
        int dst_start_date = calculateNthWeekday(dst_start_month, dst_start_week, dst_start_day, year);
        int dst_end_date = calculateNthWeekday(dst_end_month, dst_end_week, dst_end_day, year);

        // For Southern Hemisphere (like Melbourne)
        if (dst_start_month > dst_end_month) {
            if (current_month > dst_start_month || current_month < dst_end_month) {
                return std_offset + 3600;
            }
            else if (current_month == dst_start_month && current_day >= dst_start_date) {
                return std_offset + 3600;
            }
            else if (current_month == dst_end_month && current_day < dst_end_date) {
                return std_offset + 3600;
            }
        }
        // For Northern Hemisphere
        else {
            if (current_month > dst_start_month && current_month < dst_end_month) {
                return std_offset + 3600;
            }
            else if (current_month == dst_start_month && current_day >= dst_start_date) {
                return std_offset + 3600;
            }
            else if (current_month == dst_end_month && current_day < dst_end_date) {
                return std_offset + 3600;
            }
        }

        return std_offset;
    }

    int PCF8563TimeManager::calculateNthWeekday(int month, int nth, int weekday, int year) {
        struct tm timeinfo = {0};
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_mday = 1;
        
        mktime(&timeinfo);  // Normalize timeinfo
        
        int first_weekday = timeinfo.tm_wday;
        int day = 1 + (weekday - first_weekday + 7) % 7;
        day += (nth - 1) * 7;
        
        return day;
    }


long PCF8563TimeManager::getTimeForCodeGeneration(){

	RTCInfoRecord anRTCInfoRecord =now();
	uint8_t seconds = anRTCInfoRecord.second+SECONDOFFSET;
	uint8_t month = anRTCInfoRecord.month-1;
	uint16_t year = anRTCInfoRecord.year;

	return dateAsSeconds(year, month, anRTCInfoRecord.date, anRTCInfoRecord.hour, anRTCInfoRecord.minute, anRTCInfoRecord.second );
}


long PCF8563TimeManager::getTimeForCodeGeneration(RTCInfoRecord anRTCInfoRecord){

	//RTCInfoRecord anRTCInfoRecord =now();
	uint8_t seconds = anRTCInfoRecord.second+SECONDOFFSET;
	uint8_t month = anRTCInfoRecord.month-1;
	uint16_t year = anRTCInfoRecord.year;

	return dateAsSeconds(year, month, anRTCInfoRecord.date, anRTCInfoRecord.hour, anRTCInfoRecord.minute, anRTCInfoRecord.second );
}



long PCF8563TimeManager::getCurrentTimeInSeconds(RTCInfoRecord anRTCInfoRecord){

	int month = anRTCInfoRecord.month-1;
	long now=dateAsSeconds(anRTCInfoRecord.year, month, anRTCInfoRecord.date, anRTCInfoRecord.hour, anRTCInfoRecord.minute, anRTCInfoRecord.second);
	return now;
}

long PCF8563TimeManager::getCurrentTimeInSeconds(){
	RTCInfoRecord anRTCInfoRecord =now();

	int month = anRTCInfoRecord.month-1;
	long now=dateAsSeconds(anRTCInfoRecord.year, month, anRTCInfoRecord.date, anRTCInfoRecord.hour, anRTCInfoRecord.minute, anRTCInfoRecord.second);
	return now;
}



	byte PCF8563TimeManager::bcdToDec(byte value)
	{
	  return ((value / 16) * 10 + value % 16);
	}

	byte PCF8563TimeManager::decToBcd(byte value){
	  return (value / 10 * 16 + value % 10);
	}

	bool PCF8563TimeManager::checkPCF8563alarm()
	// checks if the alarm has been activated
	{
	  byte test;
	  // get the contents from control register #2 and place in byte test;
	  Wire.beginTransmission(PCF8563address);
	  Wire.write(0x01);
	  Wire.endTransmission();
	  Wire.requestFrom(PCF8563address, 1);
	  test = Wire.read();
	  test = test & B00001000; // isolate the alarm flag bit
	 return (test == B00001000) ;// alarm on?
//	  {
//	    // alarm! Do something to tell the user
//	    Serial.println("** alarm **");
//	    delay(2000);
//
//	    // turn off the alarm
//	   //PCF8563alarmOff();
//	  }
	}

	void PCF8563TimeManager::setPCF8563alarm(byte am, byte ah)
	// this sets the alarm data to the PCF8563
	{
	  //byte am, ah, ad, adow;
	  am = decToBcd(am); //alarmMinute
	  am = am | 100000000; // set minute enable bit to on
	  ah = decToBcd(ah);
	  ah = ah | 100000000; // set hour enable bit to on
//	  ad = decToBcd(alarmDay);
//	  ad = ad | 100000000; // set day of week alarm enable bit on
//	  adow = decToBcd(alarmDayOfWeek);
//	  adow = ad | 100000000; // set day of week alarm enable bit on

	  // write alarm data to PCF8563
	  Wire.beginTransmission(PCF8563address);
	  Wire.write(0x09);
	  Wire.write(am);
	  Wire.write(ah);

	  // optional day of month and day of week (0~6 Sunday - Saturday)
	  /*
	  Wire.write(ad);
	  Wire.write(adow);
	  */
	  Wire.endTransmission();

	  // optional - turns on INT_ pin when alarm activated
	  // will turn off once you run void PCF8563alarmOff()
	  Wire.beginTransmission(PCF8563address);
	  Wire.write(0x01);
	  Wire.write(B00000010);
	  Wire.endTransmission();
	}

	void PCF8563TimeManager::PCF8563alarmOff()
	// turns off alarm enable bits and wipes alarm registers.
	{
	  byte test;
	  // first retrieve the value of control register 2
	  Wire.beginTransmission(PCF8563address);
	  Wire.write(0x01);
	  Wire.endTransmission();
	  Wire.requestFrom(PCF8563address, 1);
	  test = Wire.read();

	  // set bit 3 "alarm flag" to 0
	  test = test - B00001000;

	  // now write new control register 2
	  Wire.beginTransmission(PCF8563address);
	  Wire.write(0x01);
	  Wire.write(test);
	  Wire.endTransmission();
	}


	//
	// oscilators functions
	//
	void PCF8563TimeManager::PCF8563oscOFF()
	// turns off oscillator
	{
	  Wire.beginTransmission(PCF8563address);
	  Wire.write(0x0D);
	  Wire.write(0);
	  Wire.endTransmission();
	}

	void PCF8563TimeManager::PCF8563osc1Hz()
	// sets oscillator to 1 Hz
	{
	  Wire.beginTransmission(PCF8563address);
	  Wire.write(0x0D);
	  Wire.write(B10000011);
	 
	  Wire.endTransmission();
	}

	void PCF8563TimeManager::PCF8563osc32Hz()
	// sets oscillator to 32 kHz
	{
	  Wire.beginTransmission(PCF8563address);
	  Wire.write(0x0D);
	  Wire.write(B10000010);
	  Wire.endTransmission();
	}

	void PCF8563TimeManager::PCF8563osc1024kHz()
	// sets oscillator to 1.024 kHz
	{
	  Wire.beginTransmission(PCF8563address);
	  Wire.write(0x0D);
	  Wire.write(B10000001);
	  Wire.endTransmission();
	}

	void PCF8563TimeManager::PCF8563osc32768kHz()
	// sets oscillator to 32.768 kHz
	{
	  Wire.beginTransmission(PCF8563address);
	  Wire.write(0x0D);
	  Wire.write(B10000000);
	  Wire.endTransmission();
	}
PCF8563TimeManager::~PCF8563TimeManager() {
	// TODO Auto-generated destructor stub
}
