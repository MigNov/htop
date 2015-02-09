/*
htop - linux/Platform.c
(C) 2014 Hisham H. Muhammad
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "Platform.h"
#include "IOPriority.h"
#include "IOPriorityPanel.h"
#include "LinuxProcess.h"
#include "Battery.h"

#include "Meter.h"
#include "CPUMeter.h"
#include "MemoryMeter.h"
#include "SwapMeter.h"
#include "TasksMeter.h"
#include "LoadAverageMeter.h"
#include "UptimeMeter.h"
#include "SensorsMeter.h"
#include "ClockMeter.h"
#include "HostnameMeter.h"

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*{
#include "Action.h"
#include "BatteryMeter.h"
}*/

static Htop_Reaction Platform_actionSetIOPriority(Panel* panel, ProcessList* pl, Header* header) {
   (void) panel, (void) pl;
   LinuxProcess* p = (LinuxProcess*) Panel_getSelected(panel);
   if (!p) return HTOP_OK;
   IOPriority ioprio = p->ioPriority;
   Panel* ioprioPanel = IOPriorityPanel_new(ioprio);
   const char* fuFunctions[] = {"Set    ", "Cancel ", NULL};
   void* set = Action_pickFromVector(panel, ioprioPanel, 21, fuFunctions, header);
   if (set) {
      IOPriority ioprio = IOPriorityPanel_getIOPriority(ioprioPanel);
      bool ok = Action_foreachProcess(panel, (Action_ForeachProcessFn) LinuxProcess_setIOPriority, (size_t) ioprio, NULL);
      if (!ok)
         beep();
   }
   Panel_delete((Object*)ioprioPanel);
   return HTOP_REFRESH | HTOP_REDRAW_BAR | HTOP_UPDATE_PANELHDR;
}

void Platform_setBindings(Htop_Action* keys) {
   keys['i'] = Platform_actionSetIOPriority;
}

MeterClass* Platform_meterTypes[] = {
   &CPUMeter_class,
   &ClockMeter_class,
   &LoadAverageMeter_class,
   &LoadMeter_class,
   &MemoryMeter_class,
   &SwapMeter_class,
   &TasksMeter_class,
   &UptimeMeter_class,
   &SensorsMeter_class,
   &BatteryMeter_class,
   &HostnameMeter_class,
   &AllCPUsMeter_class,
   &AllCPUs2Meter_class,
   &LeftCPUsMeter_class,
   &RightCPUsMeter_class,
   &LeftCPUs2Meter_class,
   &RightCPUs2Meter_class,
   &BlankMeter_class,
   NULL
};

int Platform_getUptime() {
   double uptime = 0;
   FILE* fd = fopen(PROCDIR "/uptime", "r");
   if (fd) {
      fscanf(fd, "%64lf", &uptime);
      fclose(fd);
   }
   return (int) floor(uptime);
}

int Platform_getSensors(double *current, double *max) {
   if ((current == NULL) || (max == NULL))
      return 1;

   *current = -1;
   *max = -1;
   if (getenv("HTOP_SENSORS_BINARY") == NULL)
      return 1;
   else {
      char cmd[1024] = { 0 };

      snprintf(cmd, sizeof(cmd), "HTOP_FORMAT=1 %s 2> /dev/null", getenv("HTOP_SENSORS_BINARY"));
      FILE *fp = popen(cmd, "r");
      if (fp != NULL) {
        char line[16];
        fgets(line, sizeof(line), fp);
        fclose(fp);

	int i;
        for (i = 0; i < strlen(line); i++)
		if (line[i] == '/') {
			*current = atof(line);
			*max = atof(line + i + 1);
			break;
		}
      }
   }

   return 0;
}

void Platform_getLoadAverage(double* one, double* five, double* fifteen) {
   int activeProcs, totalProcs, lastProc;
   *one = 0; *five = 0; *fifteen = 0;
   FILE *fd = fopen(PROCDIR "/loadavg", "r");
   if (fd) {
      int total = fscanf(fd, "%32lf %32lf %32lf %32d/%32d %32d", one, five, fifteen,
         &activeProcs, &totalProcs, &lastProc);
      (void) total;
      assert(total == 6);
      fclose(fd);
   }
}

int Platform_getMaxPid() {
   FILE* file = fopen(PROCDIR "/sys/kernel/pid_max", "r");
   if (!file) return -1;
   int maxPid = 4194303;
   fscanf(file, "%32d", &maxPid);
   fclose(file);
   return maxPid;
}

void Platform_getBatteryLevel(double* level, ACPresence* isOnAC) {

   double percent = Battery_getProcBatData();

   if (percent == 0) {
      percent = Battery_getSysBatData();
      if (percent == 0) {
         *level = -1;
         *isOnAC = AC_ERROR;
         return;
      }
   }

   *isOnAC = Battery_isOnAC();
   *level = percent;
}
