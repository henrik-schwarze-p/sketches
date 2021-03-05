#include "UI.h"
#include "Drawing.h"
#include "Text.h"
#include "FatalError.h"
#include "Namespaces.h"
#include "BoardLib.h"
#include "Chunks.h"
#include "Heap.h"
#include "Icon.h"
#include "Instance.h"

extern char firstOfManyDefinition;

Button buttons[MAX_NUMBER_OF_BUTTONS + MAX_NUMBER_OF_DEBUG_BUTTONS];
char   buttonCounter = 0;
char   debugButtonCounter = 0;

int _goToScreenLock = 0;

int goToScreenLock() {
    return _goToScreenLock;
}

Button buttonAtIndex(int index) {
    return buttons[index];
}

// Clicked on button array 3, index 4, returns 3004
int buttonForCoordinates(int px, int py) {
    for (int bi = 0; bi < buttonCounter + debugButtonCounter; bi++) {
        Button* b = &buttons[bi];
        for (int i = 0; i < b->nx; i++)
            for (int j = 0; j < b->ny; j++) {
                int x = b->x + i * (b->width + b->deltaX);
                int y = b->y + j * (b->height + b->deltaY);
                if (x <= px && x + b->width >= px && y <= py && y + b->height >= py)
                    return bi * 1000 + j * b->nx + i;
            }
    }
    return -1;
}

void resetButtons() {
    buttonCounter = 0;
    debugButtonCounter = 0;
}

Button* addButton(int     x,
                  int     y,
                  int     width,
                  int     height,
                  command command,
                  screen  screen,
                  int     nx,
                  int     ny,
                  int     deltaX,
                  int     deltaY,
                  int     param) {
    if (buttonCounter >= MAX_NUMBER_OF_BUTTONS)
        fatalError(19, buttonCounter);
    Button* b = &buttons[buttonCounter];
    b->x = x;
    b->y = y;
    b->width = width;
    b->height = height;
    b->command = command;
    b->screen = screen;
    b->nx = nx;
    b->ny = ny;
    b->deltaX = deltaX;
    b->deltaY = deltaY;
    b->param = param;
    buttonCounter++;
    return b;
}

Button* unsecureAddButton(int     x,
                          int     y,
                          int     width,
                          int     height,
                          command command,
                          screen  screen,
                          int     nx,
                          int     ny,
                          int     deltaX,
                          int     deltaY,
                          int     param) {
    Button* b = &buttons[buttonCounter + debugButtonCounter];
    b->x = x;
    b->y = y;
    b->width = width;
    b->height = height;
    b->command = command;
    b->screen = screen;
    b->nx = nx;
    b->ny = ny;
    b->deltaX = deltaX;
    b->deltaY = deltaY;
    b->param = param;
    debugButtonCounter++;
    return b;
}

void drawDebugButtonRect(Button b) {
    return;
    pushClipping(0, 0, 320 * 3, 240 * 3);
    for (int i = 0; i < b.nx; i++)
        for (int j = 0; j < b.ny; j++) {
            setColor(colorRed);
            int zx = b.x + i * (b.width + b.deltaX);
            int zy = b.y + j * (b.height + b.deltaY);
            drawRect(zx, zy, b.width, b.height);
        }
    popClipping();
}

void addLabel(int x, int y, const char* _label, int width, int height, int color, int style, int suffix) {
    if (style & STYLE_BIGFONT)
        setBigFont();
    else
        setStandardFont();
    int extra = 0;
    if (style & STYLE_CENTERED)
        extra = width / 2 - stringWidth(_label) / 2;
    if (style & STYLE_BOLD)
        setBoldStyle();
    setColor(color);
    if (stringWidth(_label) > width)
        drawStringWithIntSubscript(_label, x, y + height / 2 - fontHeight() - 2, width, suffix);
    else {
        drawString(_label, x + extra, y + height / 2 - fontHeight() / 2);
        if (suffix)
            drawInt(suffix, x + extra + stringWidth(_label), y + height / 2 - fontHeight() / 2 + 5);
    }
    setNormalStyle();
}

void addLabel(int x, int y, int instance, int handle, int width, int height, int color, int style, int suffix) {
    int address = chunkAddressForInstanceAndHandle(instance, handle);
    setPrintModeEEPROM(address);
    int strWidth = stringWidth();
    if (style & STYLE_BIGFONT)
        setBigFont();
    else
        setStandardFont();
    int extra = 0;
    if (style & STYLE_CENTERED)
        extra = width / 2 - strWidth / 2;
    if (style & STYLE_BOLD)
        setBoldStyle();
    setColor(color);
    if (strWidth > width)
        drawStringWithIntSubscript(x, y + height / 2 - fontHeight() - 2, width, suffix);
    else {
        drawString(x + extra, y + height / 2 - fontHeight() / 2);
        if (suffix)
            drawInt(suffix, x + extra + strWidth, y + height / 2 - fontHeight() / 2 + 5);
    }
    setPrintModePSTR();
    setNormalStyle();
}

//
// TOOLBAR
//

ToolbarButton toolbarButtons[MAX_TOOLBAR_BUTTONS];
int           toolbarCount = 0;
int           toolbarNextAddResets = 1;

int _toolbarWidth(int p, int minis) {
    if (toolbarButtons[p].mini)
        return 44;

    int tw = 0;
    for (int i = 0; i < toolbarCount; i++)
        if (toolbarButtons[i].mini == 0)
            tw += stringWidth(toolbarButtons[i].label, toolbarButtons[i].fromLabel, toolbarButtons[i].toLabel);
    unsigned int awidth = (horizontalResolution - minis * 44);
    if (tw == 0)
        return awidth;
    return awidth * stringWidth(toolbarButtons[p].label, toolbarButtons[p].fromLabel, toolbarButtons[p].toLabel) / tw;
}

void toolbarClicked(int param) {
    int x = getTouchX();
    int accX = 0;
    for (int i = 0; i < toolbarCount; i++) {
        ToolbarButton button = toolbarButtons[i];
        accX += button.width;
        if (x < accX) {
            if (button.enabled) {
                if (button.action) {
                    button.action(button.param);
                } else if (button.screen)
                    goToScreen(button.screen);
            }
            return;
        }
    }
}

void drawToolbar() {
    setColor(colorToolbar);
    fillRect(0, 196, horizontalResolution, 44);
    setBigFont();
    int minis = 0;
    for (int i = 0; i < toolbarCount; i++)
        if (toolbarButtons[i].mini)
            minis++;
    if (minis == toolbarCount) {
        if (toolbarCount <= 2)
            for (int i = 0; i < toolbarCount; i++) {
                toolbarButtons[i].mini = 0;
                minis--;
            }
        else
            for (int i = 1; i < toolbarCount - 1; i++) {
                toolbarButtons[i].mini = 0;
                minis--;
            }
    }
    int x = 0;
    setColor(colorBG);
    for (int i = 0; i < toolbarCount - 1; i++) {
        x += _toolbarWidth(i, minis);
        drawLine(x, 196, x, 239);
    }
    x = 0;
    for (int i = 0; i < toolbarCount; i++) {
        ToolbarButton t = toolbarButtons[i];
        int           w = _toolbarWidth(i, minis);
        toolbarButtons[i].width = w;
        const char* label = t.label;
        setColor(colorDisabledToolbar);
        if (toolbarButtons[i].enabled)
            setColor(colorWhite);
        drawString(label, x + w / 2 - stringWidth(label, t.fromLabel, t.toLabel) / 2, 210, toolbarButtons[i].fromLabel,
                   toolbarButtons[i].toLabel);
        x += w;
    }
    addButton(0, 196, 320, 44, toolbarClicked, 0, 1, 1, 0, 0, 0);
}

void toolbarCommit() {
    // when there are no elements in the toolbar...
    if (toolbarNextAddResets)
        toolbarCount = 0;
    drawToolbar();
    toolbarNextAddResets = 1;
}

void toolbarAdd(toolbarButtonSize mini, int isEnabled, const char* label, command fn, int param, int first, int last) {
    if (!goToScreenLock())
        fatalError(20, 0);  // added toolbarButton outside screen code
    if (toolbarNextAddResets) {
        toolbarCount = 0;
        toolbarNextAddResets = 0;
    }
    toolbarButtons[toolbarCount].enabled = isEnabled;
    toolbarButtons[toolbarCount].label = label;
    toolbarButtons[toolbarCount].action = fn;
    toolbarButtons[toolbarCount].screen = 0;
    toolbarButtons[toolbarCount].mini = mini;
    toolbarButtons[toolbarCount].param = param;
    toolbarButtons[toolbarCount].fromLabel = first;
    toolbarButtons[toolbarCount].toLabel = last;
    toolbarCount++;
    if (toolbarCount >= MAX_TOOLBAR_BUTTONS)
        fatalError(21, toolbarCount);
}

void toolbarAdd(toolbarButtonSize mini, int isEnabled, const char* label, screen screen, int first, int last) {
    if (!goToScreenLock())
        fatalError(22, 0);
    if (toolbarNextAddResets) {
        toolbarCount = 0;
        toolbarNextAddResets = 0;
    }
    toolbarButtons[toolbarCount].enabled = isEnabled;
    toolbarButtons[toolbarCount].label = label;
    toolbarButtons[toolbarCount].action = 0;
    toolbarButtons[toolbarCount].screen = screen;
    toolbarButtons[toolbarCount].mini = mini;
    toolbarButtons[toolbarCount].fromLabel = first;
    toolbarButtons[toolbarCount].toLabel = last;
    toolbarCount++;
    if (toolbarCount >= MAX_TOOLBAR_BUTTONS)
        fatalError(23, toolbarCount);
}

void toolbarAdd(toolbarButtonSize mini, int isEnabled, const char* label, command fn, int param) {
    toolbarAdd(mini, isEnabled, label, fn, param, 0, stringLen(label) - 1);
}

void toolbarAdd(toolbarButtonSize mini, int isEnabled, const char* label, command fn) {
    toolbarAdd(mini, isEnabled, label, fn, ignoreParam);
}

void toolbarAdd(const char* label, command fn, int param) {
    toolbarAdd(standard, isenabled, label, fn, param);
}

void toolbarAdd(const char* label, command fn, int param, int f1, int f2) {
    toolbarAdd(standard, isenabled, label, fn, param, f1, f2);
}

void toolbarAdd(const char* label, command fn) {
    toolbarAdd(label, fn, ignoreParam);
}

void toolbarAdd(const char* label, screen screen) {
    toolbarAdd(standard, isenabled, label, screen);
}

void toolbarAdd(toolbarButtonSize mini, int isEnabled, const char* label, screen screen) {
    toolbarAdd(mini, isEnabled, label, screen, 0, stringLen(label) - 1);
}

void toolbarAddHome() {
    toolbarAdd(mini, 1, MINI_ICON_HOME, desktop::startScreen);
}

//
// MESSAGE
//

char messageY;
char messageH;
void (*fnMessage)(void);

void messageCallback(int param) {
    setColor(colorBG);
    fillRect(2 * margin, messageY - 2, horizontalResolution - margin * 4, messageH + 4);
    goToScreen(fnMessage, 0);
}

void message(const char* m, void (*a)(void)) {
    message(m, a, 1);
}

void message(const char* m, void (*a)(void), int clear) {
    goToScreen(a, clear * DRAWSCREEN_CLEAR_MIDDLE);
    resetButtons();
    setBigFont();
    fnMessage = a;
    setPrintX(2 * margin);
    setMaxPrintX(horizontalResolution - 2 * margin);
    int count = 0;
    int widths[10];
    measureText(m, widths, &count);
    messageH = count * fontHeight() + 8 + count * 4 + 20;
    messageY = verticalResolution / 2 - messageH / 2;
    setColor(colorBlack);
    drawRect(2 * margin, messageY - 2, horizontalResolution - margin * 4, messageH + 4);
    setColor(colorWhite);
    drawRect(2 * margin + 1, messageY - 1, horizontalResolution - margin * 4 - 2, messageH + 2);
    setColor(colorBlack);
    drawRect(2 * margin + 2, messageY, horizontalResolution - margin * 4 - 4, messageH);
    setColor(colorLightGray);
    fillRect(2 * margin + 3, messageY + 1, horizontalResolution - margin * 4 - 6, messageH - 2);
    setColor(colorBlack);
    setPrintY(messageY + 4);
    int y = messageY + 14;
    for (int i = 0; i < count; i++) {
        int p = i > 0 ? widths[i - 1] : 0;
        drawCenteredStringPart(m, p, widths[i], y);
        y += 18;
    }
    addButton(0, 0, horizontalResolution, verticalResolution, messageCallback, 0, 1, 1, 0, 0, 0);
}

//
// SCREEN DRAWING
//

screen _currentScreen = 0;

void cleanMiddle() {
    setColor(colorBG);
    fillRect(0, menuBarHeight + 1, horizontalResolution, verticalResolution - menuBarHeight - toolbarHeight - 1);
}

void dc(int p) {
    dumpChunks();
    dumpHeaps();
}

void goToScreen(screen screen, int mode) {
    if (goToScreenLock())
        fatalError(24, 0);
    firstOfManyDefinition = 1;
    _goToScreenLock = 1;
    _currentScreen = screen;
    resetButtons();
    setMargins(margin, margin);
    setPrintY(margin + menuBarHeight + 1);
    setColor(colorBG);
    if (mode & DRAWSCREEN_CLEAR_MENU)
        fillRect(0, 0, horizontalResolution, menuBarHeight);
    if (mode & DRAWSCREEN_CLEAR_MIDDLE)
        cleanMiddle();
    if (mode & DRAWSCREEN_CLEAR_BOTTOM)
        fillRect(0, verticalResolution - toolbarHeight, horizontalResolution, toolbarHeight);
    setColor(colorBlack);
    setStandardFont();
    screen();
    setMargins(margin, margin);
    toolbarCommit();
    _goToScreenLock = 0;

    for (int i = 0; i < buttonCounter + debugButtonCounter; i++)
        drawDebugButtonRect(buttons[i]);
}

void goToScreen(screen s) {
    goToScreen(s, DRAWSCREEN_CLEAR_MIDDLE);
}

int currentScreenIs(screen s) {
    return _currentScreen == s;
}

screen currentScreen() {
    return _currentScreen;
}
// 425 397
