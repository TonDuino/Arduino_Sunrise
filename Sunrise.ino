/*
   Author:  Ton Rutgers
   Date:    november 19, 2017
   Purpose: Calculate sunrise, transit and sunset for any location on Earth at any day.
   Licence: you can use and modify this code freely for your own purpose.

   Completely based on the work of <unknown author>
   Source document: http://aa.quae.nl/en/reken/zonpositie.html

   Input:   Longitude in degrees; West is positive, East is negative
            Latitude in degrees; North is positive, South negative
            Date: year, Month, monthDay

   Output:  A table with the events: sunrise, Transit (this is when the sun is at it's
            highest point in the sky), noon (position of the sun at noon) and sunrise
            It will also print the position of the sun at whole hours between sunrise and
            sunset.
            Above the polarcircle (or below if you are in the southern hemisphere) there are
            days at which the sun will allways be above the horizon (whole day light), or
            always below the horizon (whole day dark). The program will detect this, and
            in case the sun is above the horizon, it will show the position of sun in the sky.

   Note 1:  I have little Astronomy knowledge; I just put a lot of effort in translating the work in the
            abovementioned source document for Arduino.
   Note 2:  This program is not designed for memory or CPU efficiency. It simply follows the steps in the
            source document.

   The challenge was: accurate computing with the limited floating point capabilities of
   the Arduino (Nano in my case). The source document computes with Julian day numbers, which have 7 digits
   BEFORE the decimal point, while at least 4 digits AFTER the decimal point are also important. So for
   this method we need at least 11 accurate digits. Since Arduino has a floating point accuracy somewhere
   between 6 and 7 digits this won't work.
   Fortunately we don't need all 7 Julian day digits before the decimal point, because only the offset
   with the Julian day number of 1-1-2000 (= 2451545) is important. But still then, we need at least
   4 digits before the decimal point, leaving 2-3 after the decimal point. This is still not enough, so
   I have split the offset in an integer and a fractional part.
   Furthermore some formula's multiply to big numbers and then do a modulo 360 degrees. This applies to
   calculation of M and Theta. For these calculations I have added tables, calculated with Excel, for
   better floating point precision. These tables apply to the years 2000-2049, and will give better
   accuracy. It still works for years < 2000 and > 2049, but then there is less precision.


   You can set the program behavior with the booleans interactive, printAll and debug.

*/

// program behavior
boolean interactive = true;  // for manual input of date
boolean printAll = false;    // print in-between results
boolean debug = false;       // for serial output in debugging mode

// approximate location of my home
float Lo = -6.7;      //  longitude, 6.7 degrees East
float La = +53.0;     //  Latitude, 53.0 degrees North
// 0. calculate Longitude and Latitude in rad
float Lo_rad = Lo * PI / 180.0;
float La_rad = La * PI / 180.0;

// today
//int Yr = 2004;    // year
//byte Mo = 4;      // month
//byte Day = 1;     // monthday
int Yr = 2017;    // year
byte Mo = 9;      // month
byte Day = 13;     // monthday

// local time offset UTC = GMT +/- timezone
const int TimeZone = +1;      // at my location UTC = GMT + 1
const int daySavOffset = +1;  // at my location day savings = +1 from last sunday in March until last sunday in October
byte daySavings = 0;          // the actual amount of daysavings (0 if it's wintertime)

// Julian day number vars
const long J2000 = 2451545; // Julian date at 1-1-2000
long J;                     // Julian day number of 'today'

// global variables
boolean polarDay;                 // shows whether the sun is above thehorizon all day long
boolean polarNight;               // show whether the sun is below the horizon all day
const float pi = 102.9373;        // ecliptic longitude
const float h0 = -0.83;           // center of the sun is h0 degrees below horizon when the top touches the horizon
const float epsilon = 23.4393;    // obliquity of the equator of the planet compared to the orbit of the planet.
const float epsilon_rad = epsilon * PI / 180.0;
const byte daysPerMonth [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // days per month for a non-leap year
char inBuffer[10];                // input from keyboard

typedef struct {
  byte  eventType;      // event numbers for Noon, Transit, sunRise and sunSet
  float Jfract;         // Jfraction for each event
  float H;              // H for this event
  float A;              // Azimuth for this event
  float h;              // altitude (angle above horizon) for this event
  float delta_rad;      // delta for this event
  float lambdaSun;      // lambda for this event
} event_type;

// definitions of events. Note also used as index for array event[]
const byte Noon = 0;
const byte Transit = 1;
const byte sunRise = 2;
const byte sunSet = 3;
const byte wholeHour = 4;

const byte nrEvents = 5;
event_type event[nrEvents];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println(F("Calculate sunrise, transit and sunset for:"));
  Serial.print(F("Longitude (degrees): ")); Serial.println(Lo);
  Serial.print(F("Latitude  (degrees): ")); Serial.println(La);
}

void loop() {
  // 0. initialize
  Serial.println();
  if (interactive) manualInput();

  Serial.print(F("At Gregorian date  : ")); Serial.print(Yr); Serial.print(F("-"));
  Serial.print(Mo); Serial.print(F("-")); Serial.println(Day);
  Serial.print(F("Timezone: GMT"));
  if (TimeZone >= 0) Serial.print(F("+"));
  J = JulianDayNumber(Yr, Mo, Day); // calculate Julian day number of 'today' (before daySavings)
  Serial.println(TimeZone);
  Serial.print(F("Daysavings at this date (offset in hours): "));
  daySavings = calcDaySavings(Yr, Mo, Day, daySavOffset);  // determine the amount of daysavings given this date
  Serial.println(daySavings);
  Serial.print(F("Local time: GMT"));
  if (TimeZone + daySavings >= 0); Serial.print(F("+"));
  Serial.println(TimeZone + daySavings);
  Serial.println();


  // 1. calculate Julian day number defined by 'today'
  long Jdiff = J - J2000;             // Number of Julian days since J2000 (convenient for calculations)
  if (printAll) {
    Serial.print(F("J, Julian day number: ")); Serial.println(J);
    Serial.print(F("Jdiff, Julian day diff: ")); Serial.println(Jdiff);
  }

  // 2. calculate the Mean Anomaly M (degrees)

  // 3. equation of center: difference between mean and true anomaly, C (degrees)

  // 4. The Perihelion and the Obliquity of the Ecliptic
  // definitions of pi and epsilon are global
  // nothing to calculate

  // 5. ecliptical coordinates, lambda (degrees)

  // 6. equatorial coordinates: alpha = right ascension (degrees) and delta = declination (degrees)

  // 7. the observer: sidereal time theta (degrees), altitude above horizon, h (degrees),
  //    hour angle H (degrees) and azimuth A (degrees)
  //    translate A and h in terms of where you can see the sun in the sky from where you stand
  event[Noon].eventType = Noon;   // event type is Noon
  event[Noon].Jfract = 0.0;
  recalcPosition2(Jdiff, Noon);   // calculate sun position for this date and time fraction (Noon)

  // determine the season
  int season = event[Noon].lambdaSun / 90;
  if (La < 0) season = (season + 2) % 4;  // southern hemisphere is opposite to northern
  Serial.print(F("The season is: "));
  switch (season) {
    case 0:
      Serial.println(F("Spring"));
      break;
    case 1:
      Serial.println(F("Summer"));
      break;
    case 2:
      Serial.println(F("Autumn"));
      break;
    case 3:
      Serial.println(F("Winter"));
      break;
    default:
      Serial.print(F("unclear, lambda is out of range: "));
      Serial.println(event[Noon].lambdaSun);
  }
  Serial.println();

  // 8. solar transit
  const float J0 = 0.0009;
  const float J1 = 0.0053;
  const float J2 = -0.0068;
  const float J3 = 1.000000;  // since J3 is exactly 1.00000, we omit this parameter from the equations

  event[Transit].eventType = Transit;       // event type is Transit

  // first guess for Jtransit
  float nx = -J0 - Lo / 360.0;  // (J - J2000) = Jdiff excluded
  float n = round(nx);
  event[Transit].Jfract = n - nx; // this is the deviating fraction from Jdiff
  // The source doc says now calculate Jx = J + J3 * (n - nx). This should result in 2453096,9870, but
  // this is too much for the floating point arithmetic. So we have to trick a bit:
  // -  with Jx we calculate values for M and Lsun
  // -  these values are calculated for (J - J2000), which is Jdiff, a much smaller number
  // -  so we are calculating M and Lsun from Jdiffx = Jdiff + (n - nx)
  // -  second measure is to split Jx in the whole number Jdiff and a fractional part Jfraction
  // -  the fucntions calcM, calcTheta and calcH take both the whole number and the fraction for better precision
  float M_Jx = calcM(Jdiff, event[Transit].Jfract);
  float Lsun_Jx = calcLsun(M_Jx);
  event[Transit].Jfract += J1 * sin(M_Jx * PI / 180.0) + J2 * sin(Lsun_Jx * PI / 90.0); // first estimate of Jtransit
  if (printAll) {
    Serial.print(F("nx (Jdays): ")); Serial.println(nx, 6);
    Serial.print(F("n  (Jdays): ")); Serial.println(n, 6);
    Serial.print(F("M_Jx (degrees): ")); Serial.println(M_Jx, 6);
    Serial.print(F("Lsun_Jx (degrees): ")); Serial.println(Lsun_Jx, 6);
    Serial.print(F("First estimate of solar transit (Jdays): ")); Serial.print(Jdiff);
    if (event[Transit].Jfract >= 0.0) Serial.print(F(" +"));
    else Serial.print(F(" "));
    Serial.println(event[Transit].Jfract, 6);
  }

  // start iterating until H = 0.0000
  if (debug) Serial.println("Iteration transit");
  byte i = 0;   // prevent endless iteration
  recalcPosition2(Jdiff, Transit);
  // H may have gone beyond 0 resulting in 359.99... degrees. In this case subtract 360.0
  if (event[Transit].H > 180.0) event[Transit].H -= 360.0;
  while (abs(event[Transit].H) >= 0.00005 && i < 10) {
    if (debug) {
      Serial.print("Iteration# "); Serial.println(i);
    }
    event[Transit].Jfract -= event[Transit].H / 360.0;  // next estimate
    recalcPosition2(Jdiff, Transit);
    if (event[Transit].H > 180.0) event[Transit].H -= 360.0;
    i++;
  }
  if (printAll) {
    Serial.print(F("Number of iterations: ")); Serial.println(i);
    Serial.print(F("Final solar transit (Jdays): ")); Serial.print(Jdiff);
    if (event[Transit].Jfract >= 0.0) Serial.print(F(" + "));
    else Serial.print(F(" "));
    Serial.println(event[Transit].Jfract, 6);
  }

  // check whether it is polar night (no sun at all) or polar day (whole day sun)
  // this depends on the argument of acos(argument) for calculating Ht
  // if argument >= 1 it is dark all day
  // if argument <= -1 the sun is above the horizon all day
  float argument = (sin(h0 * PI / 180.0) - sin(La_rad) * sin(event[Noon].delta_rad)) / (cos(La_rad) * cos(event[Noon].delta_rad));
  if (argument > 1.0) {
    // polar night
    polarNight = true;
  }
  else {
    if (argument < -1.0) {
      polarDay = true;
    }
    else {
      polarNight = false;
      polarDay = false;
    }
  }

  // 9. equation of time
  // nothing to calculate

  // 10. sunrise and sunset

  if (polarNight) {
    Serial.println(F("It is dark all day; there is no sunrise and sunset"));
  }
  else {
    if (polarDay) {
      Serial.println(F("It is light all day; there is no sunrise and sunset"));
      // show position of the sun between 00:00 and 24:00 UTC
      Serial.println(F("UTC\tLocalT\tEvent\tLookTo\tatYour\tAngle\tAlt"));
      printIntermediate(Jdiff, -0.51, +0.5);
    }
    else {
      // first estimate of sunrise
      event[sunRise].eventType = sunRise; // event type is sunrise
      float Ht = calcHt(event[Transit].delta_rad);     // calculate Ht with delta_rad
      event[sunRise].Jfract = event[Transit].Jfract - Ht / 360.0;
      if (printAll) {
        Serial.print(F("First estimate of Jrise (Jdays): ")); Serial.print(Jdiff);
        if (event[sunRise].Jfract >= 0.0) Serial.print(F(" +"));
        else Serial.print(F(" "));
        Serial.println(event[sunRise].Jfract, 6);
      }

      // start iterating sunrise until correction is near zero
      if (debug) Serial.println("Iteration sunrise");
      i = 0;  //  prevents endless loop
      recalcPosition2(Jdiff, sunRise);
      if (event[sunRise].H >= 0.0) event[sunRise].H -= 360.0; // H should be negative for sunrise
      Ht = calcHt(event[sunRise].delta_rad);     // calculate Ht with delta_rad
      float Jcorrection = (event[sunRise].H + Ht) / 360.0;
      if (debug) {
        Serial.print("Correction "); Serial.println(Jcorrection, 6);
      }
      while (abs(Jcorrection) >= 0.00005 && i < 10) {
        if (debug) {
          Serial.print("Iteration# "); Serial.println(i);
        }
        event[sunRise].Jfract -= Jcorrection;
        recalcPosition2(Jdiff, sunRise);
        if (event[sunRise].H >= 0.0) event[sunRise].H -= 360.0; // H should be negative for sunrise
        Ht = calcHt(event[sunRise].delta_rad);     // calculate Ht with delta_rad
        Jcorrection = (event[sunRise].H + Ht) / 360.0;
        if (debug) {
          Serial.print("Correction "); Serial.println(Jcorrection, 6);
        }
        i++;
      }
      if (printAll) {
        Serial.print(F("Number of iterations: ")); Serial.println(i);
        Serial.print(F("Final Jrise (Jdays): ")); Serial.print(Jdiff);
        if (event[sunRise].Jfract >= 0.0) Serial.print(F(" +"));
        else Serial.print(F(" "));
        Serial.println(event[sunRise].Jfract, 6);
      }

      // first estimate of sunset
      event[sunSet].eventType = sunSet; // event type is sunset
      Ht = calcHt(event[Transit].delta_rad);     // calculate Ht with delta_rad
      event[sunSet].Jfract = event[Transit].Jfract + Ht / 360.0;
      if (printAll) {
        Serial.print(F("First estimate of Jset (Jdays): ")); Serial.print(Jdiff);
        if (event[sunSet].Jfract >= 0.0) Serial.print(F(" +"));
        else Serial.print(F(" "));
        Serial.println(event[sunSet].Jfract, 6);
      }

      // start iteration sunset until correction is near zero
      if (debug) Serial.println("Iteration sunset");
      i = 0;  //  prevents endless loop
//      float PositionSet[4]; // holds H, A, h and delta_rad
      recalcPosition2(Jdiff, sunSet);
      if (event[sunSet].H < 0.0) event[sunSet].H += 360.0; // for sunset Ht must be positive
      Ht = calcHt(event[sunSet].delta_rad);     // calculate Ht with delta_rad
      Jcorrection = (event[sunSet].H - Ht) / 360.0;
      if (debug) {
        Serial.print("Correction "); Serial.println(Jcorrection, 6);
      }
      while (abs(Jcorrection) >= 0.00005 && i < 10) {
        if (debug) {
          Serial.print("Iteration# "); Serial.println(i);
        }
        event[sunSet].Jfract -= Jcorrection;
        recalcPosition2(Jdiff, sunSet);
        if (event[sunSet].H < 0.0) event[sunSet].H += 360.0; // for sunset Ht must be positive
        Ht = calcHt(event[sunSet].delta_rad);     // calculate Ht with delta_rad
        Jcorrection = (event[sunSet].H - Ht) / 360.0;
        if (debug) {
          Serial.print("Correction "); Serial.println(Jcorrection, 6);
        }
        i++;
      }
      if (printAll) {
        Serial.print(F("Number of iterations: ")); Serial.println(i);
        Serial.print(F("Final Jset (Jdays): ")); Serial.print(Jdiff);
        if (event[sunSet].Jfract >= 0.0) Serial.print(F(" +"));
        else Serial.print(F(" "));
        Serial.println(event[sunSet].Jfract, 6);
      }

      debug = false;    // debug intended for iterations; switch off from here/
      // corrections for (great) altitude not implemented

      // 13. from Julian day numbers to UTC

      // show the results, and calculate intermediate whole hours. Output format
      // UTC    Local Event   LookTo  atYour  Angle Up
      // 05:45  06:45 Sunrise South   Right   82.7  -0.8

      // but first sort the events in chronological order
      sortEvents();
      
      Serial.println(F("UTC\tLocalT\tEvent\tLookTo\tatYour\tAngle\tAlt"));
      // Between the polar circles the order will be sunRise, Transit, Noon, sunSet or
      // sunRise, Noon, Transit, sunSet.
      // Beyond the polar circles, sunSet may occur before Noon or Noon before sunRise.
      printLine2(0);    // first line; mostly sunRise, but beyond the polar circle it can be Noon
      for (byte i = 1; i < nrEvents - 1; i++) {
        printIntermediate(Jdiff, event[i-1].Jfract, event[i].Jfract);
        printLine2(i);
      }
    }
  }

  if (!interactive) while (1);  // endless loop
}

//////////////////////// END OF MAIN LOOP ////////////////////////

void printLine2(byte index) {
  printTime(event[index].Jfract, 0); Serial.print("\t");
  printTime(event[index].Jfract, TimeZone + daySavings); Serial.print("\t");
  switch (event[index].eventType) {
    case sunRise:
      Serial.print(F("SunRise"));
      break;
    case Transit:
      Serial.print(F("Transit"));
      break;
    case Noon:
      Serial.print(F("Noon"));
      break;
    case sunSet:
      Serial.print(F("SunSet"));
    
  }
  Serial.print("\t");
  float A = fmod(event[index].A, 360.0);
  if (A < 0.0) A += 360.0;  // correct for negative Azimuth
  if (A >= 270.0 || A <= 90.0) {
    Serial.print(F("South")); Serial.print("\t");
    if (A <= 90.0) {
      Serial.print(F("Right")); Serial.print("\t");
      Serial.print(A, 1); Serial.print("\t");
    }
    else {
      Serial.print(F("Left")); Serial.print("\t");
      Serial.print(360.0 - A, 1); Serial.print("\t");
    }
  }
  else {
    Serial.print(F("North")); Serial.print("\t");
    if (A > 180.0) {
      Serial.print(F("Right")); Serial.print("\t");
      Serial.print(A - 180.0, 1); Serial.print("\t");
    }
    else {
      Serial.print(F("Left")); Serial.print("\t");
      Serial.print(180.0 - A, 1); Serial.print("\t");
    }
  }
  Serial.print(event[index].h, 1); Serial.print("\t");
  Serial.println();
}

void printLine(float Jf, char text[10], float *pos) {
  printTime(Jf, 0); Serial.print("\t");
  printTime(Jf, TimeZone + daySavings); Serial.print("\t");
  Serial.print(text); Serial.print("\t");
  float A = fmod(pos[1], 360.0);
  if (A < 0.0) A += 360.0;  // correct for negative Azimuth
  if (A >= 270.0 || A <= 90.0) {
    Serial.print(F("South")); Serial.print("\t");
    if (A <= 90.0) {
      Serial.print(F("Right")); Serial.print("\t");
      Serial.print(A, 1); Serial.print("\t");
    }
    else {
      Serial.print(F("Left")); Serial.print("\t");
      Serial.print(360.0 - A, 1); Serial.print("\t");
    }
  }
  else {
    Serial.print(F("North")); Serial.print("\t");
    if (A > 180.0) {
      Serial.print(F("Right")); Serial.print("\t");
      Serial.print(A - 180.0, 1); Serial.print("\t");
    }
    else {
      Serial.print(F("Left")); Serial.print("\t");
      Serial.print(180.0 - A, 1); Serial.print("\t");
    }
  }
  Serial.print(pos[2], 1); Serial.print("\t");
  Serial.println();
}

void printIntermediate(long Jdiff, float Jfrom, float JupTo) {
//  float Jfraction = -0.5;   // start at 00:00 UTC
//  float Position[4];  // holds H, A, h and delta_rad
  // note the + and -0.0007 is to compensate for floating point inaccuracy (0.0007 = 1 minute)
  event[nrEvents - 1].eventType = wholeHour;
  event[nrEvents - 1].Jfract = -0.5;
  while (event[nrEvents - 1].Jfract < (JupTo - 0.0007)) {
    if (event[nrEvents - 1].Jfract > (Jfrom + 0.0007)) {
      recalcPosition2(Jdiff, nrEvents - 1);
      printLine2(nrEvents - 1);
    }
    event[nrEvents - 1].Jfract += 1.0 / 24.0; // add one hour
  }
}

void recalcPosition2(long Jdiff, byte eventIndex) {
  float M = calcM( Jdiff, event[eventIndex].Jfract );
  float M_rad = M * PI / 180.0;   // in rad
  float C = calcC(M_rad);
  event[eventIndex].lambdaSun = calcLambdaSun(M, C);
  float lambdaSun_rad = event[eventIndex].lambdaSun * PI / 180.0;
  event[eventIndex].delta_rad = calcDelta_rad (lambdaSun_rad);
  float delta = event[eventIndex].delta_rad * 180.0 / PI;
  float alpha_rad = calcAlpha_rad (lambdaSun_rad);
  float alpha = alpha_rad * 180.0 / PI;
  float theta = calcTheta( Jdiff, event[eventIndex].Jfract);
  event[eventIndex].H = calcH(theta, alpha);
  float H_rad = event[eventIndex].H * PI / 180.0;
  float A_rad = calcA_rad(H_rad, event[eventIndex].delta_rad, La_rad);
  event[eventIndex].A = A_rad * 180.0 / PI;
  float h_rad = calc_h_rad(event[eventIndex].delta_rad, H_rad, La_rad);
  event[eventIndex].h = h_rad * 180.0 / PI;

  if (printAll && event[eventIndex].eventType != wholeHour) {
    Serial.print(F("\teventType: ")); Serial.println(event[eventIndex].eventType);
    Serial.print(F("\tJfract ")); Serial.println(event[eventIndex].Jfract, 6);
    Serial.print(F("\tM      ")); Serial.println(M, 6);
    Serial.print(F("\tC      ")); Serial.println(C, 6);
    Serial.print(F("\tlambda ")); Serial.println(event[eventIndex].lambdaSun, 6);
    Serial.print(F("\tdelta  ")); Serial.println(event[eventIndex].delta_rad * 180.0 / PI, 6);
    Serial.print(F("\talpha  ")); Serial.println(alpha, 6);
    Serial.print(F("\ttheta  ")); Serial.println(theta, 6);
    Serial.print(F("\tH      ")); Serial.println(event[eventIndex].H, 6);
    Serial.print(F("\tA      ")); Serial.println(event[eventIndex].A, 6);
    Serial.print(F("\th      ")); Serial.println(event[eventIndex].h, 6);
    Serial.println();
  }
}

float calcHt(float delta_rad_sun) {
  // output in degrees
  return (180.0 / PI) * acos( (sin(h0 * PI / 180.0) - sin(La_rad) * sin(delta_rad_sun)) /
                              (cos(La_rad) * cos(delta_rad_sun)) );
}

float calcM (long Jdiff, float Jfraction) {
  // Output in degrees
  // gives better precision for year = 2000 - 2050
  // next table is M0 value for julian day number = 0, 366, 732, etc, up to year 2049
  const float M0 [] =     { 357.5291000,
                            358.2588025,
                            358.9885050,
                            359.7182074,
                            0.4479099,
                            1.1776124,
                            1.9073149,
                            2.6370174,
                            3.3667198,
                            4.0964223,
                            4.8261248,
                            5.5558273,
                            6.2855298,
                            7.0152322,
                            7.7449347,
                            8.4746372,
                            9.2043397,
                            9.9340422,
                            10.6637446,
                            11.3934471,
                            12.1231496,
                            12.8528521,
                            13.5825546,
                            14.3122570,
                            15.0419595,
                            15.7716620,
                            16.5013645,
                            17.2310670,
                            17.9607694,
                            18.6904719,
                            19.4201744,
                            20.1498769,
                            20.8795794,
                            21.6092818,
                            22.3389843,
                            23.0686868,
                            23.7983893,
                            24.5280918,
                            25.2577942,
                            25.9874967,
                            26.7171992,
                            27.4469017,
                            28.1766042,
                            28.9063066,
                            29.6360091,
                            30.3657116,
                            31.0954141,
                            31.8251166,
                            32.5548190,
                            33.2845215
                          };
  const float M1 = 0.98560028;
  int index = Jdiff / 366;  // calc number of multiples of 366
  if (index < 0 || index > 49) {
    // outside the range of M0 table -> use the original formula -> less precision!
    return fmod(M0[0] + ((float)Jdiff + Jfraction) * M1, 360.0);
  }
  int remainder = Jdiff - index * 366;
  return fmod(M0[index] + ((float)remainder + Jfraction) * M1, 360.0);
}

float calcC (float M_rad ) {
  // input in rad
  // output in degrees
  const float C1 = 1.9148;
  const float C2 = 0.0200;
  const float C3 = 0.0003;
  return C1 * sin(M_rad) + C2 * sin(2 * M_rad) + C3 * sin(3 * M_rad);
}

float calcLambdaSun ( float M, float C) {
  // input in degrees
  // output in degrees
  return fmod(M + pi + C + 180.0, 360.0);
}

float calcLsun ( float M) {
  // input in degrees
  // output in degrees
  return fmod(M + pi + 180.0, 360.0);
}

float calcAlpha_rad( float lambdaSun_rad ) {
  // input in rad
  // output in rad
  return atan2(sin(lambdaSun_rad) * cos(epsilon_rad), cos(lambdaSun_rad));
}

float calcDelta_rad ( float lambdaSun_rad ) {
  // input in rad
  // output in rad
  return asin(sin(lambdaSun_rad) * sin(epsilon_rad));
}

float calcTheta (long Jdiff, float Jfraction) {
  // output in degrees
  const float theta0 [] = { 280.1470000,
                            280.8852010,
                            281.6234020,
                            282.3616030,
                            283.0998040,
                            283.8380050,
                            284.5762060,
                            285.3144070,
                            286.0526080,
                            286.7908090,
                            287.5290100,
                            288.2672110,
                            289.0054120,
                            289.7436130,
                            290.4818140,
                            291.2200150,
                            291.9582160,
                            292.6964170,
                            293.4346180,
                            294.1728190,
                            294.9110200,
                            295.6492210,
                            296.3874220,
                            297.1256230,
                            297.8638240,
                            298.6020250,
                            299.3402260,
                            300.0784270,
                            300.8166280,
                            301.5548290,
                            302.2930300,
                            303.0312310,
                            303.7694320,
                            304.5076330,
                            305.2458340,
                            305.9840350,
                            306.7222360,
                            307.4604370,
                            308.1986380,
                            308.9368390,
                            309.6750400,
                            310.4132410,
                            311.1514420,
                            311.8896430,
                            312.6278440,
                            313.3660450,
                            314.1042460,
                            314.8424470,
                            315.5806480,
                            316.3188490
                          };
  const float theta1 = 360.9856235;       // use this for fractional part of Julian day number
  const float theta1mod360 = 0.9856235;   // theta1 mod 360 -> use this for whole Julian day numbers
  int index = Jdiff / 366;  // calc number of multiples of 366
  if (index < 0 || index > 49) {
    // outside the range of theta0 table -> use the original formula -> less precision!
    return fmod(theta0[0] + (float)Jdiff * theta1mod360 + Jfraction * theta1 - Lo, 360.0);
  }
  int remainder = Jdiff - index * 366;
  return fmod(theta0[index] + (float)remainder * theta1mod360 +  Jfraction * theta1 - Lo, 360.0);
}

float calcH (float theta, float alpha) {
  // output in degrees
  return fmod(theta - alpha, 360.0);
}

float calcA_rad ( float H_rad, float delta_rad, float La_rad) {
//  Serial.print("calcA H: "); Serial.println(H_rad * 180.0 / PI, 6);
//  Serial.print("calcA delta: "); Serial.println(delta_rad*180.0/PI, 6);
//  Serial.print("calcA La: "); Serial.println(La_rad*180.0/PI, 6);
  
  return atan2(sin(H_rad), cos(H_rad) * sin(La_rad) - tan(delta_rad) * cos(La_rad));
}

float calc_h_rad (float delta_rad, float H_rad, float La_rad) {
  // output in rad
  return asin(sin(La_rad) * sin(delta_rad) + cos(La_rad) * cos(delta_rad) * cos(H_rad));
}

long JulianDayNumber(int year, byte month, byte day) {
  const byte daysPerMonth [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // days per month for a non-leap year
  boolean leapYear = false;

  if (year % 4 == 0) leapYear = true;   // determine if today is in a leapyear

  int Ydiff = year - 2000;              // offset from Julian reference year
  int Ydiff4 = Ydiff / 4;             // number of set-of-4-years (including 1 leapyear)
  int Yremain = Ydiff - Ydiff4 * 4;   // remaining number of years
  long Jt = J2000 + Ydiff4 * (366 + 365 + 365 + 365);  // Julian day number January, 1 of Ydiff4 sets of 4 years

  // add remaining years
  int Jadd = 366;                     // first year to add is a leapyear
  while (Yremain > 0) {               // add number of days for Yremain
    Jt += Jadd;
    Jadd = 365;
    Yremain--;
  }

  // Jt is now at January, 1 of the year of today
  // add the months
  if (month < 1) month = 1;           // make sure month is in range 1 ... 12
  month = 1 + ((month - 1) % 12);

  for (byte i = 0; i < (month - 1); i++) {
    Jt += daysPerMonth[i];             // add the monthdays
    if (leapYear && i == 1) {
      Jt += 1;   // plus 1 for Feb in leapyear
    }
  }

  // and add the Month day, minus 1 (Julian day number!)
  Jt = Jt + day - 1;

  // now J contains the Julian day number
  return (Jt);
}

void printTime(float Jf, int offset) {
  // Calculate local time (or UTC if offset = 0) from Jfraction.
  // For local time offset = TimeZone + daySavings.
  // Jfraction if the offset in Julian time from 12:00 UTC.
  // Jfraction is -0.5 ... + 0.5 close to the prime meridian (Longitude = 0 degrees);
  // However when farther away from this meridian, the range of Jfraction will shift accordingly.
  // At longitude ~ 180 degrees, Jfraction may be within -1.0 ... 0.0 or 0.0 ... +1.0
  // Also, beyond the pole circles, sunrise can be before 00:00 (the previous day) of sunset
  // after 24:00 (the next day). So Jfraction will also be < -0.5 or > 0.5.
  // Therefore we have to calculate not only the hours and minutes, but also +/- number of days.
  // This can be no more than -1 or +1 day.
  // Because the arithmetic is easier for positive numbers, the time is shifted +1.5 days (+ 1 day
  // relative to 12:00 UTC), so the result will be relative to 00:00 UTC, which is the normal clock time.
  // And then 1 day is subtracted to align again.
  const int dayMins = 24 * 60;  // minutes per day
  // add another 0.5 minute to make the integer arithmetic round properly to the nearest whole minute 
  float minutes = (float)dayMins * (1.5 + Jf) + offset * 60 + 0.5;
  int day = int(minutes / (float)dayMins) - 1;   // the offset in days (normally 0)
  int hour = int(minutes / 60.0) % 24;            // hours
  int minute = ((int)minutes - hour * 60) % 60;   // minute
    
  Serial.print(day);
  Serial.print(F(":"));
  if (hour < 10) Serial.print(F("0"));
  Serial.print(hour);
  Serial.print(F(":"));
  if (minute < 10) Serial.print(F("0"));
  Serial.print(minute);
}

byte calcDaySavings(int Yr, byte Mo, byte Day, int daySavOffset) {
  // determine whether daySavings timeshift applies to this date
  // note: daysavings starts at  the last sunday of march until the last sunday of october
  // exception 1: during 1981 until 1995 it was the last sunday in september
  // exception 2: during 1977 until 1980 it varies, but close to exception 1; this is not implemented,
  // but treated as exception 2
  // reference year is 2000, at which the last sunday in march occurred at:
  const byte mar2000 = 26;
  byte end2000;     // daynumber of last sunday in month 'endMonth'
  byte endMonth;

  // determine endMonth for period of daysavings
  if (Yr > 1995) {
    endMonth = 10;
    end2000 = 29;   // october for > Yr 1995
  }
  else {
    if (Yr > 1976) {
      endMonth = 9;
      end2000 = 24;   // september; for exception 1
    }
    else return (0); // no daysavings in this year
  }
  int offset2000 = Yr - 2000;

  // determine amount of leap years since march 2000
  int leapYears = 0;
  if (offset2000 >= 0) leapYears = offset2000 / 4;  // for positive do not include 2000
  else leapYears = ((offset2000 + 1) / 4) - 1;      // for negative include 2000

  // determine last sunday in march
  int subtract = (31 - mar2000) + leapYears + offset2000;
  subtract = mod(subtract, 7);
  int marday = 31 - subtract;
  long JDS = JulianDayNumber(Yr,  3, marday);
  // determine last sunday in september or october, depending on the year
  long JDE;
  int endday;
  subtract = (daysPerMonth[endMonth - 1] - end2000) + leapYears + offset2000;
  subtract = mod(subtract, 7);
  endday = daysPerMonth[endMonth - 1] - subtract;
  JDE = JulianDayNumber(Yr, endMonth, endday);

  // now we know the period of daysavings in this year
  if (J >= JDS && J < JDE) return (daySavOffset); // yes, daysavings apply
  return (0);
}

int mod(int subtract, int modNr) {
  // this will do a proper modulo modNr, also for negative numbers
  if (subtract < 0) {
    subtract = modNr - (abs(subtract) % modNr);
    if (subtract == modNr) subtract = 0;
  }
  else {
    subtract = subtract % modNr;
  }
  return (subtract);
}

void sortEvents() {
  // only sort the first 4 events, not the last row, because that row is used for calculating in-between
  // results for whole hours
  event_type swap;      // define struct for swapping
  
  for (byte pass = nrEvents - 2; pass > 0; pass--) {
    for (byte i = 0; i < pass; i++) {
      if (event[i].Jfract > event[i+1].Jfract) {
        // swap event[i] and event[i+1]
        swap = event[i];
        event[i] = event[i+1];
        event[i+1] = swap;
      }
    }
  }
}


void manualInput() {
  boolean inputValid = false;
  while (!inputValid) {
    inputValid = true;  // unless proven otherwise
    Serial.println(F("Enter Year, 4 digits, 0000 ... 9999"));
    readInBuffer(4);
    Yr = (inBuffer[0] - '0') * 1000 + (inBuffer[1] - '0') * 100 + (inBuffer[2] - '0') * 10 + (inBuffer[3] - '0');
    // validate
    if (Yr < 0 || Yr > 9999) {
      inputValid = false;
      Serial.print(F("Invalid year: ")); Serial.println(Yr);
    }
    else {
      Serial.println(F("Enter Month, 2 digits, 01 ... 12"));
      readInBuffer(2);
      Mo = (inBuffer[0] - '0') * 10 + (inBuffer[1] - '0');
      if (Mo < 0 || Mo > 12) {
        inputValid = false;
        Serial.print(F("Invalid month: ")); Serial.println(Mo);
      }
      else {
        Serial.println(F("Enter Day, 2 digits"));
        readInBuffer(2);
        Day = (inBuffer[0] - '0') * 10 + (inBuffer[1] - '0');
        if (Day < 1 || Day > daysPerMonth[Mo - 1]) {
          inputValid = false;
          Serial.print(F("Invalid day for this month: ")); Serial.println(Mo);
        }
      }
    }
  }
}

void readInBuffer(byte nr) {
  while (!Serial.available());
  Serial.readBytes(inBuffer, nr);
  while (Serial.available()) Serial.read(); // read the string
}

// ENF OF FILE

