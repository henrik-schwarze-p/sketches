#include "Text.h"
#include "Target.h"
#include "Drawing.h"
#include "Instance.h"
#include "Namespaces.h"
#include "BoardLib.h"

void fatalError(int code, int extraInfo) {
    int port = admin::alarmPort();
    if (port)
        pm::_digitalWrite(port, 1);

    setColor(colorBlack);
    fillRect(0, 0, horizontalResolution, verticalResolution);
    setPrintX(margin);
    setPrintY(margin);
    setColor(colorRed);
    println(PSTR("FATAL ERROR"));
    setColor(colorWhite);
    print(PSTR("  Code: "));
    println(code);
    print(PSTR("  Active instance: "));
    println(getActiveInstance());
    print(PSTR("  Foreground instance: "));
    println(getForegroundInstance());
    print(PSTR("  Extra info: "));
    println(extraInfo);

    consoleln(PSTR("FATAL ERROR"));
    setColor(colorWhite);
    console(PSTR("  Code: "));
    consoleln(code);
    console(PSTR("  Active instance: "));
    consoleln(getActiveInstance());
    console(PSTR("  Extra info: "));
    consoleln(extraInfo);

    terminate();
}
