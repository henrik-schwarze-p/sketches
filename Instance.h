#ifndef KernelInstanceManagement_h
#define KernelInstanceManagement_h
// The magic bytes to know if the EPROM is formatted or not. Hold also the
// version.
#define EPROM_FORMATTED 0
#define EPROM_NUMBER_OF_INSTANCES 2
#define INSTANCE_TABLE_START 3
#define EPROM_RANDOM 93

#define MAX_NUMBER_OF_PUSHED_INSTANCES 10

// 30 instance should be enought for everyone
#define MAX_NUMBER_OF_INSTANCES 30
#define INSTANCE_ID 0
#define INSTANCE_STATUS 2
// The "amount of memory used" by an instance in the instance table
#define INSTANCE_OVERHEAD 3

#define STATUS_OK 1
#define STATUS_REGISTERP_ACTIVATED 2
// This status is not being used at the time being, we
// are loudly failing when an allocation fails
#define STATUS_NO_MEMORY 4
#define STATUS_REGISTERM_ACTIVATED 8

#define NO_INSTANCE_ACTIVE 254

#include "Frequency.h"
#include "stdint.h"
#include "Descriptors.h"

int         callEvalCondition(int instance, int conditionKind, uint16_t parameters[]);
void        callPerformAction(int instance, int actionKind, uint16_t* parameters);
void        callMonitor(int instance, int x, int y, int w, int h, int n);
const char* callConditionNames(int instance, int j);
void        callTimeSlice(int instance, int fg, frequencyType frequency, int firt);
const char* callActionNames(int instance, int j);
void        callWhenPowered(int instance);
void        callWhenCreated(int instance);
void        callWhenDestroyed(int instance);
const char* callGetLabelForPort(int instance, int port, int digital);
void        callWithContextSwitch(int instance, void (*fn)(void));
void        callRegisterDirectly(int instance, int mode);

void addStatusForInstance(int status);
int  getActiveInstance();
int  isSetStatusForInstance(int status);
void addStatusForInstance(int instance, int status);

const char* nameForInstance(int instance);
const char* nameForType(int type);
int         cardinalityForType(int type);
int         cardinalityForInstance(int instance);
int         typeForInstance(uint16_t instance);
const char* nameForType(int type);
int         hasMonitorForInstance(int instance);

typedef void (*screenPtr)(void);
screenPtr screenForInstance(int instance);

unsigned char numberOfInstances();
unsigned char lastInstance();
int           getActiveInstance();
void          popContext();
void          switchContextToInstance(int instance);
int           instanceForId(int);
int           statusForInstance(int instance, int contains);
void          setActiveInstance(int instance);
int           getInstanceWithStatus();
void          removeInstanceAt(int instance);
int           getRepetitionForInstance(int instance);
int           idForInstance(int offset);

int numberOfVisibleInstances();
int visibleInstance(int index);

const char* instanceName(int instance);
void        removeStatusForInstance(int instance, int status);
uint16_t    getDescriptorForInstance(uint8_t instance);

unsigned char getForegroundInstance();
void          setForegroundInstance(unsigned char newInstance);


extern uint16_t lastState[];
#define STATE_CHANGED 65534
#define STATE_NOT_CONFIGURED 65533

#define SEL_CARDINALITY 1024
#define SEL_REGISTERVARS 1025
#define SEL_STATE 1026
#define SEL_WHENCREATED 1027
#define SEL_MONITOR 1028
#define SEL_TIMESLICE 1029
#define SEL_ACTIONSDEF 1030
#define SEL_CONDITIONSDEF 1031
#define SEL_SETCONDITIONPARAMETER 1032
#define SEL_SETACTIONPARAMETER 1033
#define SEL_PRINTCONDITIONPARAMETER 1034
#define SEL_PRINTACTIONPARAMETER 1035
#define SEL_WHENPOWERED 1036
#define SEL_STARTSCREEN 1037
#define SEL_WHENDESTROYED 1038
#define SEL_GETLABELFORPORT 1039
#define SEL_EVALCONDITION 1040
#define SEL_PERFORMACTION 1041
#define SEL_ID 1042
#define SEL_NAME 1043
#define SEL_HASMONITOR 1044

int         callGeneratedIdForType(int type);
int         callGeneratedId(int instance);
const char* callGeneratedNameForType(int type);
const char* callGeneratedName(int instance);
int         callGeneratedCardinalityForType(int type);
int         callGeneratedCardinality(int instance);
void        callGeneratedRegisterVarsForType(int type);
void        callGeneratedRegisterVars(int instance);
int         callGeneratedStateForType(int type);
int         callGeneratedState(int instance);
void        callGeneratedWhenCreatedForType(int type);
void        callGeneratedWhenCreated(int instance);
void        callGeneratedMonitorForType(int type, int x, int y, int w, int h);
void        callGeneratedMonitor(int instance, int x, int y, int w, int h);
void        callGeneratedTimeSliceForType(int type, int fg, frequency fr);
void        callGeneratedTimeSlice(int instance, int fg, frequency fr);
const char* callGeneratedActionsDefForType(int type, int kind);
const char* callGeneratedActionsDef(int instance, int kind);
const char* callGeneratedConditionsDefForType(int type, int kind2);
const char* callGeneratedConditionsDef(int instance, int kind2);
void        callGeneratedSetConditionParameterForType(int type, int kind, int param);
void        callGeneratedSetConditionParameter(int instance, int kind, int param);
void        callGeneratedSetActionParameterForType(int type, int kind, int param);
void        callGeneratedSetActionParameter(int instance, int kind, int param);
void        callGeneratedPrintConditionParameterForType(int type, int a, int b, uint16_t c);
void        callGeneratedPrintConditionParameter(int instance, int a, int b, uint16_t c);
void        callGeneratedPrintActionParameterForType(int type, int a, int b, uint16_t c);
void        callGeneratedPrintActionParameter(int instance, int a, int b, uint16_t c);
void        callGeneratedWhenPoweredForType(int type);
void        callGeneratedWhenPowered(int instance);
screen      callGeneratedStartScreenForType(int type);
screen      callGeneratedStartScreen(int instance);
void        callGeneratedWhenDestroyedForType(int type);
void        callGeneratedWhenDestroyed(int instance);
const char* callGeneratedGetLabelForPortForType(int type, int a, int b);
const char* callGeneratedGetLabelForPort(int instance, int a, int b);
int         callGeneratedEvalConditionForType(int type, int a, uint16_t* array);
int         callGeneratedEvalCondition(int instance, int a, uint16_t* array);
void        callGeneratedPerformActionForType(int type, int a, uint16_t* array);
void        callGeneratedPerformAction(int instance, int a, uint16_t* array);
int         callGeneratedHasMonitorForType(int type);
int         callGeneratedHasMonitor(int instance);

#endif
