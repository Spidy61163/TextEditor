#include <iostream>
#include "LinkList.h"
#include "TextEditor.h"
using namespace std;

int vim()
{
    TextEditor editor;
    int command;
    string prev_hit;
    string file = "";

    while (true) {
        editor.display();  // Display the current text editor content
        if (editor.quit())
        {
            return 0;
        }
        editor.warningGiven();
        command = getChar(prev_hit);  // Read single character input

        if (editor.isCommandMode() && command == 'n')
        {
            editor.next();
            continue;
        }

        if (editor.isCommandMode() && command == 'N')
        {
            editor.prev();
            continue;
        }

        if (command == 27) {  // Escape key to exit insert mode
            editor.exitInsertMode();
            editor.exitCommandMode();
            editor.set_cmdlist(false);
            continue;
        }

        if (editor.isInsertMode()) {
            // Handle arrow keys in insert mode
            switch (command) {
            case 65: // Up arrow
                editor.moveUp();
                break;
            case 66: // Down arrow
                editor.moveDown();
                break;
            case 67: // Right arrow
                editor.moveRight();
                break;
            case 1068: // Left arrow
                editor.moveLeft();
                break;
            case 10: // Enter pressed
                editor.newLine();
                break;
            case -1: // Backspace pressed
                editor.backspace();
                break;
            default:
                // If not an arrow key, insert the character
                if (command >= 32 && command <= 126) {  // Check for printable characters
                    editor.insertChar(static_cast<char>(command));
                }
                break;
            }
        }
        else {
            // Normal mode command handling
            switch (command) {
            case 'i':  // Enter insert mode
                editor.enterInsertMode();
                break;
            case 67:  // Right arrow
                editor.moveRight();
                break;
            case 1068:  // Left arrow
                editor.moveLeft();
                break;
            case 65:  // Up arrow
                editor.moveUp();
                break;
            case 66:  // Down arrow
                editor.moveDown();
                break;
            case 'n':  // New line
                editor.newLine();
                break;
            case 'x':  // For deleting char
                editor.backspace();
                break;
            case 999:  // For deleting whole line dd
                editor.deleteCurrentLine();
                break;
            case 888:  // yy
                editor.yankLine();
                break;
            case 'D':
                editor.deleteToEndOfLine();
                break;
            case 'p':
                editor.pasteAfter();
                break;
            case 'P':
                editor.pasteBefore();
                break;
            case '0':
                editor.moveToStartOfLine();
                break;
            case 1234:
                editor.unindent();
                break;
            case 4321:
                editor.indent();
                break;
            case 'J':
                editor.joinWithPrevLine();
                break;
            case '$':
                editor.moveToEndOfLine();
                break;
            case 444:
                editor.enterCommandMode();
                break;
            case 555:
                editor.enterCommandMode();
                break;
            case 'q':  // Quit editor
                return 0;
            case 'w':
                editor.moveToNextWord();
                break;
            case 'e':
                editor.moveToWordEnd();
                break;
            case 'b':
                editor.moveToPreviousWord();
                break;
            default:
                // Do nothing for unrecognized commands
                break;
            }

            if (editor.isCommandMode())
            {
                if (prev_hit == "/")
                {
                    cout << "/";
                }
                editor.getCommand();
            }
        }
    }
}

int main()
{
    vim();
    return 0;
}