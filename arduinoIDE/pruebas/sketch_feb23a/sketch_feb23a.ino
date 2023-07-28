/*
Arduino Hall Effect Sensor Project
by Arvind Sanjeev
Please check out  http://diyhacking.com for the tutorial of this project.
DIY Hacking
*/


 volatile byte half_revolutions;
 unsigned int rpm;
 unsigned int rps;
 unsigned int rpmcount;
 unsigned long timemeasure = 5.00;
 unsigned long timeold;
 void setup()
 {
   Serial.begin(9600);
   attachInterrupt(19, magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)
   half_revolutions = 0;
   rpm = 0;
   timeold = 0;
 }
 void loop()//Measure RPM
 {
   if (half_revolutions >= 20) { 
 //    rpm = 30*1000/(millis() - timeold)*half_revolutions;
     timeold = millis();
     half_revolutions = 0;
     //Serial.println(rpm,DEC);
      rps = float(rpmcount)/float(timemeasure);       // rotations per second
      rpm = 60*rps;                         // rotations per minute
      Serial.println();
      Serial.print(rpmcount);      
      Serial.print("   ");
      Serial.print(rps);      
      Serial.print("   ");      
   }
 }
 void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
 {
   half_revolutions++;
 //  Serial.println("detect");

 }
