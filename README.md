# BLV-mgn-Cube-Bed-LED-control

BLV mgn Cube Bed Light Control

Connect DUET2 Wifi using I2C

Command on I2C (like M150)

I2C address : 8

Values:

 Red   : 0-255
 
 Green : 0-255
 
 Blue  : 0-255
 
 Brightness : 0-31
 
 Number of LEDs : 1-256
 
 Function : 0-end of constant color set, or all same color
 
            1-start or middle of constant color set
            
            2-LED demo mode (same as Demoreel100 in FastLED library
            
            

 Examples (commands from DuetWeb screen or PanelDue)
 
  M260 A8 B255:255:255:31:46:0 default power on all white(255:255:255) led with maximum brightness(31), 46 leds
  
  M260 A8 B255:0:0:15:46:0 Red(255:0:0) leds with half brightness(15), 46 leds
  
  M260 A8 B255:0:0:8:16:1 Start Red(255:0:0) leds with quater brightness(8), 16 leds
  
  M260 A8 B0:255:0:8:16:1 Next Green(0:255:0) leds with quater brightness(8), 16 leds (first 16 leds are Red)
  
  M260 A8 B0:0:255:8:14:0 Last Blue(0:0:255) leds with quater brightness(8), 14 leds (first Red, next Green, last Blue)
  
  M260 A8 B0:0:0:15:46:2 Demo mode with half brightness(15), RGB and LED numbers ignore
  
