#include "Arduino.h"
#include "Namespaces.h"
#include "Chunks.h"
#include "FatalError.h"
#include "Rules.h"
#include "Descriptors.h"
#include "Heap.h"
#include "Instance.h"
#include "Registration.h"
#include "Target.h"
#include "Text.h"

// The "EPROM data" is stored in this array.
// This is the configuration of the app in the store.

/* PERSISTANTE MEMORY MAPPING
 *** MAGIC BYTES
 OFFSET    DESCRIPTION
 0,1       Magic bytes too see if EPROM is formatted or not.

 *** INSTANCE TABLE
 OFFSET    DESCRIPTION
 2         Number of instances
 3,4       Type of instance 0        INSTANCE_ID offset
 5         Status of instance 0      INSTANCE_STATUS offset
 6,7       Type of instance 1
 8         Status...
 ...
 90,91     Type of instance 29
 92        Status...

 93,94     Random bytes to see if serialized emprom was unserialized.

 95,122    Reserved for future use.

 123-127   Time bytes
 130,4095  PERSISTANT MEMORY HEAP
 */
//
// BASIC INSTANCE MANAGEMENT
//
unsigned char activeInstance = NO_INSTANCE_ACTIVE;

unsigned char foregroundInstance = 0;

uint16_t lastState[MAX_NUMBER_OF_INSTANCES];

unsigned char getForegroundInstance() {
    return foregroundInstance;
}

void setForegroundInstance(unsigned char newInstance) {
    foregroundInstance = newInstance;
}

unsigned char pushedActiveInstances[MAX_NUMBER_OF_PUSHED_INSTANCES];
int           pushedActiveInstancesCounter = 0;

unsigned char numberOfInstances() {
    return readByteFromEEPROM(EPROM_NUMBER_OF_INSTANCES);
}

unsigned char lastInstance() {
    return readByteFromEEPROM(EPROM_NUMBER_OF_INSTANCES) - 1;
}

int getActiveInstance() {
    return activeInstance;
}

int instanceAddress(int instance) {
    return INSTANCE_TABLE_START + INSTANCE_OVERHEAD * instance;
}

int idForInstance(int instance) {
    return readIntFromEEPROM(instanceAddress(instance) + INSTANCE_ID);
}

int getInstanceWithStatus() {
    for (int i = 0; i < numberOfInstances(); i++)
        if (!statusForInstance(i, STATUS_OK))
            return i;
    return -1;
}

// If we have instance A,B,C,B,D,C,E,C
// the repetitions are 0,0,0,1,0,1,0,2
int getRepetitionForInstance(int instance) {
    int count = 0;
    for (int i = 0; i < instance; i++)
        if (typeForInstance(instance) == typeForInstance(i))
            count++;
    return count;
}

int isSetStatusForInstance(int status) {
    int address = instanceAddress(activeInstance) + INSTANCE_STATUS;
    return readByteFromEEPROM(address) & status;
}

void addStatusForInstance(int status) {
    int address = instanceAddress(activeInstance) + INSTANCE_STATUS;
    int oldStatus = readByteFromEEPROM(address);
    writeByteToEEPROM(address, oldStatus | status);
}

void setStatusForInstance(int instance, int status) {
    int address = instanceAddress(instance) + INSTANCE_STATUS;
    writeByteToEEPROM(address, status);
}

void addStatusForInstance(int instance, int status) {
    int address = instanceAddress(instance) + INSTANCE_STATUS;
    int oldStatus = readByteFromEEPROM(address);
    writeByteToEEPROM(address, oldStatus | status);
}

void removeStatusForInstance(int instance, int status) {
    int address = instanceAddress(instance) + INSTANCE_STATUS;
    int oldStatus = readByteFromEEPROM(address);
    writeByteToEEPROM(address, (oldStatus | status) - status);
}

int statusForInstance(int instance, int contains) {
    return readByteFromEEPROM(instanceAddress(instance) + INSTANCE_STATUS) & contains;
}

// Of course, the app for the instance should have cardinality 1, if not, it
// would be called "firstInstanceForId"
int instanceForId(int id) {
    for (int i = 0; i < numberOfInstances(); i++)
        if (idForInstance(i) == id)
            return i;
    fatalError(10, id);
    return 0;
}

// REMOVE INSTANCE

void removeInstanceAt(int instance) {
    for (int i = 0; i < pushedActiveInstancesCounter; i++)
        if (pushedActiveInstances[i] == instance)
            fatalError(11, instance);

    callWhenDestroyed(instance);
    removeStatusForInstance(instance, STATUS_REGISTERM_ACTIVATED);
    removeStatusForInstance(instance, STATUS_REGISTERP_ACTIVATED);
    switchContextToInstance(instance);
    freeAllChunks();
    freeAllHeaps();
    decreaseInstanceOffset();
    popContext();
    // callWithContextSwitch(instance, freeAllChunks);
    // callWithContextSwitch(instance, freeAllHeaps);
    // callWithContextSwitch(instance, decreaseInstanceOffset);

    int n = numberOfInstances();
    for (int i = INSTANCE_TABLE_START + instance * INSTANCE_OVERHEAD;
         i < INSTANCE_TABLE_START + (MAX_NUMBER_OF_INSTANCES - 1) * INSTANCE_OVERHEAD; i++) {
        uint8_t r = readByteFromEEPROM(i + INSTANCE_OVERHEAD);
        writeByteToEEPROM(i, r);
    }
    writeByteToEEPROM(EPROM_NUMBER_OF_INSTANCES, n - 1);
    if (activeInstance > instance)
        activeInstance--;
    updateRulesBecauseOfDeletionOfInstance(instance);
    _mergeChunks();
}

// Pushes the active instance and makes a context switch to instance
void switchContextToInstance(int instance) {
    if (pushedActiveInstancesCounter == MAX_NUMBER_OF_PUSHED_INSTANCES)
        fatalError(12, instance);
    pushedActiveInstances[pushedActiveInstancesCounter] = activeInstance;
    pushedActiveInstancesCounter++;
    setActiveInstance(instance);
}

void popContext() {
    if (pushedActiveInstancesCounter == 0)
        fatalError(13, 0);
    pushedActiveInstancesCounter--;
    setActiveInstance(pushedActiveInstances[pushedActiveInstancesCounter]);
}

// The only place where the active instance should be changed!
void setActiveInstance(int instance) {
    if (instance >= numberOfInstances() && instance != NO_INSTANCE_ACTIVE)
        fatalError(14, instance);
    if (activeInstance != NO_INSTANCE_ACTIVE)
        serializeRegisteredVariables();
    activeInstance = instance;
    if (instance != NO_INSTANCE_ACTIVE)
        unserializeRegisteredVariables();
}

int numberOfVisibleInstances() {
    int r = 0;
    for (int i = 0; i < numberOfInstances(); i++)
        r += typeForInstance(i) != DESKTOP_ID;
    return r;
}

int visibleInstance(int index) {
    for (int i = 0; i < numberOfInstances(); i++)
        if (typeForInstance(i) != DESKTOP_ID)
            if (--index < 0)
                return i;
    return -1;
}

const char* instanceName(int instance) {
    if (instance == 255)
        return PSTR("Unused");
    return nameForInstance(instance);
}

// If no descriptor, the ExceptionApp is returned.
uint16_t getDescriptorForInstance(uint8_t instance) {
    if (instance >= numberOfInstances())
        fatalError(15, instance);
    int instanceTableId = readIntFromEEPROM(INSTANCE_TABLE_START + INSTANCE_OVERHEAD * instance + INSTANCE_ID);
    for (int i = 0; i < NUMBER_OF_DESCRIPTORS; i++)
        if (getDescriptor(i) == instanceTableId)
            return getDescriptor(i);
    fatalError(16, instance);
    return 0;
}

int typeForInstance(uint16_t instance) {
    if (instance >= numberOfInstances())
        fatalError(15, instance);
    return readIntFromEEPROM(INSTANCE_TABLE_START + INSTANCE_OVERHEAD * instance + INSTANCE_ID);
}

//
// CALL FN
//

typedef int (*typeCardinality)();
typedef void (*typeRegisterVars)();
typedef int (*typeState)();
typedef void (*typeWhenCreated)();
typedef void (*typeMonitor)(int, int, int, int);
typedef void (*typeTimeSlice)(int, frequency);
typedef const char* (*typeActionsDef)(int);
typedef const char* (*typeConditionsDef)(int);
typedef void (*typeSetConditionParameter)(int, int);
typedef void (*typeSetActionParameter)(int, int);
typedef void (*typePrintConditionParameter)(int, int, uint16_t);
typedef void (*typePrintActionParameter)(int, int, uint16_t);
typedef void (*typeWhenPowered)();
typedef void (*typeStartScreen)();
typedef void (*typeWhenDestroyed)();
typedef const char* (*typeGetLabelForPort)(int, int);
typedef int (*typeEvalCondition)(int, uint16_t*);
typedef void (*typePerformAction)(int, uint16_t*);

#include "Glue.inc.h"

const char* callConditionNames(int instance, int j) {
    switchContextToInstance(instance);
    const char* result = callGeneratedConditionsDef(instance, j);
    popContext();
    return result;
}

const char* callGetLabelForPort(int instance, int port, int digital) {
    switchContextToInstance(instance);
    const char* result = callGeneratedGetLabelForPort(instance, port, digital);
    popContext();
    return result;
}

const char* callActionNames(int instance, int j) {
    switchContextToInstance(instance);
    const char* result = callGeneratedActionsDef(instance, j);
    popContext();
    return result;
}

int callEvalCondition(int instance, int conditionKind, uint16_t parameters[]) {
    switchContextToInstance(instance);
    uint16_t result = callGeneratedEvalCondition(instance, conditionKind, parameters);
    popContext();
    return result;
}

void callPerformAction(int instance, int actionKind, uint16_t* parameters) {
    switchContextToInstance(instance);
    callGeneratedPerformAction(instance, actionKind, parameters);
    popContext();
}

void callWhenCreated(int instance) {
    switchContextToInstance(instance);
    callGeneratedWhenCreated(instance);
    popContext();
}

void callWhenPowered(int instance) {
    switchContextToInstance(instance);
    callGeneratedWhenPowered(instance);
    popContext();
}

void callWhenDestroyed(int instance) {
    switchContextToInstance(instance);
    callGeneratedWhenDestroyed(instance);
    popContext();
}

void callRegisterDirectly(int instance, int mode) {
    // pico porque no switch
    startRegisterMode(mode);
    callGeneratedRegisterVars(instance);
    endRegisterMode();
}

void callTimeSlice(int instance, int fg, frequencyType frt, int first) {
    switchContextToInstance(instance);
    frequency fr;
    fr.type = frt;
    fr.justOpened = first;

    uint16_t state = callGeneratedState(instance);
    // pico

    if (state != lastState[instance] || state == STATE_CHANGED)
        callGeneratedTimeSlice(instance, fg, fr);
    lastState[instance] = state;
    popContext();
}

void callMonitor(int instance, int x, int y, int w, int h, int force) {
    switchContextToInstance(instance);
    uint16_t state = callGeneratedState(instance);
    if (state == STATE_CHANGED || state != lastState[instance] || force)
        callGeneratedMonitor(instance, x, y, w, h);
    popContext();
}

// STATIC FUNCTIONS

const char* nameForInstance(int instance) {
    return callGeneratedName(instance);
}

// int typeForInstance(int instance) {
//    return callGeneratedId(instance);
//}

int cardinalityForType(int type) {
    return callGeneratedCardinalityForType(type);
}

int cardinalityForInstance(int instance) {
    return callGeneratedCardinality(instance);
}

const char* nameForType(int type) {
    return callGeneratedNameForType(type);
}

int hasMonitorForInstance(int instance) {
    return callGeneratedHasMonitor(instance);
}

screenPtr screenForInstance(int instance) {
    return callGeneratedStartScreen(instance);
}
