# TextEditor

A lightweight terminal-based text editor inspired by `vim`, written in C++.  
Supports three modes: **Normal**, **Insert**, and **Command**.  
Implements line-wise operations, search/replace, yank/paste, indentation, and file I/O via a simple linked-list-based buffer.

## Features

- Three modes: Normal, Insert, Command  
- Navigation: move by character, word, line start/end  
- Line operations: delete, yank (copy), paste before/after, join  
- Indentation/unindentation  
- Search (`/pattern`) and navigate matches (`n`/`N` in Command Mode)  
- Search and replace with optional global flag: `s/old/new/` or `s/old/new/g`  
- File operations: open, save, quit, write+quit  
- Command history display  
- Unsaved change warning  

## Dependencies

- C++ compiler (tested on Windows due to use of `<conio.h>`)  
- `LinkList.h` – underlying linked list implementation for buffer lines (must be provided in the same directory)  
- Standard C++ headers: `<iostream>`, `<vector>`, `<string>`, `<fstream>`, etc.  

> ⚠️ `conio.h` is platform-specific (usually Windows). Porting to Unix-like systems would require replacing `_getch()` logic (e.g., using termios).

Modes:
  - Normal Mode: Default. Navigate and invoke commands.
  - Insert Mode: For typing text. Enter with i. Exit back to Normal with Esc.
  - Command Mode: Enter by typing : or / in Normal Mode. Used for file operations, search, replace, etc. Exit automatically after processing or by executing appropriate commands.

| Key / Input                     | Action                                         |
| ------------------------------- | ---------------------------------------------- |
| `i`                             | Enter Insert Mode                              |
| Arrow keys (Up/Down/Left/Right) | Move cursor                                    |
| `n`                             | New line (splits at cursor)                    |
| `x`                             | Backspace / delete char before cursor          |
| `dd`                            | Delete current line                            |
| `yy`                            | Yank (copy) current line                       |
| `p`                             | Paste after current line                       |
| `P`                             | Paste before current line                      |
| `0`                             | Move to start of line                          |
| `$`                             | Move to end of line                            |
| `J`                             | Join with previous line                        |
| `>>`                            | Indent current line (adds 4 spaces)            |
| `<<`                            | Unindent current line (removes leading spaces) |
| `w`                             | Move to next word                              |
| `b`                             | Move to previous word                          |
| `e`                             | Move to end of word                            |
| `/pattern`                      | Search for `pattern` (enters Command Mode)     |
| `n` (after search)              | Next match                                     |
| `N` (after search)              | Previous match                                 |

Command Mode Commands
Enter by typing : or / in Normal Mode. Then type the command and press Enter.

| Command          | Description                                                                                    |
| ---------------- | ---------------------------------------------------------------------------------------------- |
| `:q`             | Quit. If there are unsaved changes, issues a warning unless repeated (`:q` after a prior `:q`) |
| `:q!`            | Force quit, discarding unsaved changes                                                         |
| `:w filename`    | Save current buffer to `filename`                                                              |
| `:wq`            | Save current file (if name known) and quit                                                     |
| `:e filename`    | Load/open `filename` into editor                                                               |
| `:c`             | Toggle/display command history list                                                            |
| `pattern <text>` | Alias for search; searches for `<text>`                                                        |
| `s/old/new/`     | Replace first occurrence of `old` with `new` on current line                                   |
| `s/old/new/g`    | Replace all occurrences of `old` with `new` on current line                                    |

File Management:
  - The editor tracks whether the buffer has unsaved changes. The current file name and [+] indicator are shown in the status bar.
  - Loading a file (:e filename) inserts its contents line by line, entering insert mode internally to populate buffer.
  - Saving (:w filename) writes all lines with newline separators.

Search:
  - Use /pattern or pattern something in Command Mode to search.
  - After a successful search, use n to go to next match and N for previous. Matches are highlighted.

Internal Clipboard:
  - yy yanks (copies) the current line(s) into an internal string buffer.
  - p and P paste after/before using that buffer, preserving line boundaries (null character used internally to separate yanked lines).

Status & Feedback:
  - The bottom of the screen shows:
  - Current mode (INSERT / Command / NORMAL)
  - File name and unsaved change marker
  - Line and column position
  - Last command executed
  - Command history when :c is used
  - Warning about unsaved changes if attempting to quit without saving

Limitations / TODO:
  - No undo/redo system.
  - No multi-file tabs or buffers.
  - Search/replace is line-local; no global across all lines.
  - Commands parsing is somewhat brittle (relies on specific formatting).
  - Platform dependency due to <conio.h>; not portable to Unix without adaptation.
  - No syntax highlighting beyond simple match highlighting.
