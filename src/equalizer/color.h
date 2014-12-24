#ifndef COLOR_H
#define COLOR_H

struct TColor {
  int red;
  int green;
  int blue;
 
  TColor(const TColor& orig): red(orig.red),green(orig.green),blue(orig.blue) { }
  
  TColor(int32_t hexcode) :
    red ( (hexcode & 0xff0000) > 16),
    green( (hexcode & 0x00ff00) > 8),
    blue((hexcode & 0x0000ff) )
  {}
 
 TColor& shade(TColor& orig, int percent) {
   int t = percent<0 ? 0 : 255;
   double p = (abs(percent)%100)/100.0;
   
   TColor shaded = TColor(orig); 
   shaded.red     = round( (t - orig.red)*p)+orig.red;
   shaded.green   = round( (t - orig.green)*p)+orig.green;
   shaded.blue    = round( (t - orig.blue)*p)+orig.blue;
   
   return shaded;
 } 
};

#endif
