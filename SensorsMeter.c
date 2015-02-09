/*
htop - SensorsMeter.c
(C) 2004-2011 Hisham H. Muhammad
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "SensorsMeter.h"

#include "CRT.h"
#include "Platform.h"
#include <stdlib.h>

/*{
#include "Meter.h"
}*/

int SensorsMeter_attributes[] = { SENSORS };

static void SensorsMeter_setValues(Meter* this, char* buffer, int len) {
   Platform_getSensors(&this->values[0], &this->values[1]);
   snprintf(buffer, len, "%d/%dC", (int) this->values[0], (int) this->values[1]);
   this->total = (long int)this->values[1];
}

static void SensorsMeter_display(Object* cast, RichString* out) {
   char buffer[50];
   Meter* this = (Meter*)cast;
   double current = (double) this->values[0];
   double max = (double) this->values[1];
   RichString_write(out, CRT_colors[METER_TEXT], ":");
   snprintf(buffer, sizeof(buffer), "%.2f", current);
   RichString_append(out, CRT_colors[METER_VALUE], buffer);
   snprintf(buffer, sizeof(buffer), "%.2f", max);
   RichString_append(out, CRT_colors[METER_TEXT], "max:");
   RichString_append(out, CRT_colors[METER_VALUE], buffer);
}

MeterClass SensorsMeter_class = {
   .super = {
      .extends = Class(Meter),
      .delete = Meter_delete,
      .display = SensorsMeter_display,
   },
   .setValues = SensorsMeter_setValues, 
   .defaultMode = BAR_METERMODE,
   .total = 100.0,
   .attributes = SensorsMeter_attributes,
   .name = "Sensors",
   .uiName = "Sensors",
   .caption = "Sen"
};
