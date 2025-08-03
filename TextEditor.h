#pragma once
#include <iostream>
#include <vector>
#include <conio.h>
#include <string.h>
#include <fstream>
#include <string>
#include <sstream>
#include "LinkList.h"

std::vector<std::string> split(std::string& input, char delimiter)
{

    // Creating an input string stream from the input string
    std::istringstream stream(input);

    // Vector to store the tokens
    std::vector<std::string> tokens;

    // Temporary string to store each token
    std::string token;

    // Read tokens from the string stream separated by the
    // delimiter
    while (getline(stream, token, delimiter)) {
        // Add the token to the vector of tokens
        tokens.push_back(token);
    }

    // Return the vector of tokens
    return tokens;
}

class TextEditor;

class FileManager {
private:
    std::string currentFileName;
    bool modified;
public:
    // Core file operations
    bool loadFile(const std::string& filename,TextEditor& t);
    bool saveFile(const std::string& filename, TextEditor& t);
    bool hasUnsavedChanges() const;
    // Helper methods
    void markAsModified();
    std::string getCurrentFileName() const;
};

class SearchEngine {
private:
    std::string lastPattern;
    int lastMatchLine;
    int lastMatchColumn;
public:
    bool search(const std::string& pattern, TextEditor& t);
    bool findNext(TextEditor& t);
    bool findPrevious(TextEditor& t);
};

std::string FileManager::getCurrentFileName() const {
    return currentFileName;
}

bool FileManager::hasUnsavedChanges() const {
    return modified;
}

void FileManager::markAsModified() {
    modified = true;
}

class TextEditor {
    friend FileManager;
    friend SearchEngine;
private:
    int currentLine;
    FileManager fm;
    SearchEngine se;
    std::vector<LinkList<char>*> lines;
    LinkList<char>::Iterator cursor;
    bool insertmode;
    bool commandmode;
    std::string copyBuffer;
    std::string command = "";
    bool start;
    bool wantToQuit;
    bool cmdlist;
    bool invalidCmd;
    std::vector<std::string> commands;
    int command_marker;
    bool warning;
    std::string punctuation = "!#$ % &'()*+, -./:;<=>?@[/\]^_`{|}~";
public:
    TextEditor();
    ~TextEditor();
    void insertChar(char c);
    void moveRight();
    void moveLeft();
    void moveUp();
    void moveDown();
    void display();
    void newLine();
    void enterInsertMode();
    void exitInsertMode();
    void enterCommandMode();
    void warningGiven();
    bool isCommandMode();
    bool isInsertMode();
    void set_cmdlist(bool val);
    void backspace();
    void exitCommandMode();
    void deleteCurrentLine();
    void deleteToEndOfLine();
    void yankLine();
    void pasteAfter();
    void pasteBefore();
    void moveToStartOfLine();
    void moveToEndOfLine();
    void moveToWordEnd();
    void moveToNextWord();
    void moveToPreviousWord();
    void getCommand();
    bool quit();
    void deleteNline(int n);
    void joinWithPrevLine();
    void indent();
    void next();
    void prev();
    void unindent();
    void ndd(int n);
    void nyy(int n);
    void nj(int n);
    void save(const std::string& filename);
    void load(const std::string& filename);
    bool parser(const std::string& cmd, std::string& oldWord, std::string& newWord, bool& global) {
        int first = cmd.find('/');
        int second = cmd.find('/', first + 1);
        int third = cmd.find('/', second + 1);

        if (first == std::string::npos || second == std::string::npos || third == std::string::npos) {
            return false;
        }

        oldWord = cmd.substr(first + 1, second - first - 1);
        newWord = cmd.substr(second + 1, third - second - 1);
        global = cmd.substr(third + 1) == "g";

        return true;
    }

    std::string replacer(const std::string& line, const std::string& old, const std::string& newer, bool isg) {
        std::string result = line;
        int pos = result.find(old);

        if (!isg && pos != std::string::npos) {
            result.replace(pos, old.length(), newer);
        }
        else if (isg) {
            while (pos != std::string::npos) {
                result.replace(pos, old.length(), newer);
                pos = result.find(old, pos + newer.length());
            }
        }
        return result;
    }

    void afterreplaceputter(const std::string& cmd) {
        std::string oldWord, newWord;
        bool global;

        if (!parser(cmd, oldWord, newWord, global)) {
            std::cout << "Invalid replace command format.\n";
            return;
        }

        // Get the current line as a string
        std::string currentLineStr;
        for (auto it = lines[currentLine]->begin(); it != lines[currentLine]->end(); ++it) {
            currentLineStr += *it;
        }

        // Perform the replacement
        std::string updatedLineStr = replacer(currentLineStr, oldWord, newWord, global);

        // Update the current line in the text editor
        lines[currentLine]->clear();
        for (char ch : updatedLineStr) {
            lines[currentLine]->push_back(ch);
        }

        display();
    }
};

TextEditor::TextEditor() : currentLine(0), cursor(nullptr), insertmode(false), start(false), commandmode(false), wantToQuit(false), cmdlist(false), command_marker(0), invalidCmd(false), warning(false)
{
    // Initialize with one empty line (allocate memory for the first LinkList)
    lines.push_back(new LinkList<char>());
    cursor = lines[0]->begin();  // Set cursor to the beginning of the first line
}

void TextEditor::next()
{
    se.findNext(*this);
}

void TextEditor::prev()
{
    se.findPrevious(*this);
}

void TextEditor::set_cmdlist(bool val)
{
    cmdlist = val;
}

void TextEditor::warningGiven()
{
    warning = false;
}

void TextEditor::ndd(int n)
{
    for (int i = 0; i < n; i++)
    {
        deleteCurrentLine();
        commands.pop_back();
    }
}

void TextEditor::nyy(int n)
{
    copyBuffer = "";

    for (int j = 0; j < n; j++)
    {
        LinkList<char>::Iterator i = lines[currentLine]->begin();
        while (i != lines[currentLine]->end())
        {
            copyBuffer.push_back(i.current->data);
            i++;
        }
        if (currentLine == lines.size() - 1)
        {
            break;
        }
        copyBuffer.push_back('\0');
        currentLine++;
    }
}

void TextEditor::nj(int n)
{
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (currentLine < lines.size())
        {
            count++;
        }
    }
    for (int i = 0; i < count; i++)
    {
        joinWithPrevLine();
        commands.pop_back();
    }
}

void TextEditor::unindent()
{
    commands.push_back("<<");
    cursor = lines[currentLine]->begin();
    while (cursor.current->data == ' ')
    {
        cursor++;
        lines[currentLine]->pop_front();
    }
}

void TextEditor::indent()
{
    commands.push_back(">>");
    for (int i = 0; i < 4; i++)
    {
        cursor = lines[currentLine]->begin();
        lines[currentLine]->push_front(' ');
    }
}

void TextEditor::joinWithPrevLine()
{
    commands.push_back("J");
    if (currentLine == lines.size() - 1)
    {
        return;
    }
    currentLine++;
    cursor = lines[currentLine]->begin();
    start = true;
    backspace();
}

bool TextEditor::quit()
{
    return wantToQuit;
}

void TextEditor::deleteNline(int n)
{
    if (n > lines.size() - 1)
    {
        std::cout << n << " lines doesn't exist" << std::endl;
        return;
    }
    currentLine = n;
    deleteCurrentLine();
}

void TextEditor::exitCommandMode()
{
    commandmode = false;
}

void TextEditor::getCommand()
{
    system("cls");
    display();
    invalidCmd = false;
    std::cin >> command;
    std::vector<std::string> input = split(command,'-');
    commands.push_back(input[0]);
    input.push_back("\n");

    if (command == ":q!")
    {
        wantToQuit = true;
        return;
    }
    else if (command == ":q")
    {
        if (commands.size() > 1 && commands[commands.size() - 2] == command)
        {
            wantToQuit = true;
            return;
        }
        warning = true;
        return;
    }
    else if (command == ":wq")
    {
        if (fm.getCurrentFileName() == "")
        {
            std::cout << "Enter File Name";
            return;
        }
        fm.saveFile(fm.getCurrentFileName(), *this);
        wantToQuit = true;
        return;
    }
    else if (input[1] == "dd" && command[input[0].size()] != ' ')
    {
        commands.pop_back();
        commands.push_back("dd");
        int num = stoi(input[0]);
        ndd(num);
        return;
    }
    else if (input[1] == "yy" && command[input[0].size()] != ' ')
    {
        commands.pop_back();
        commands.push_back("yy");
        int num = stoi(input[0]);
        nyy(num);
        return;
    }
    else if (input[1] == "J" && command[input[0].size()] != ' ')
    {
        commands.pop_back();
        commands.push_back("J");
        int num = stoi(input[0]);
        nj(num);
        return;
    }
    else if (input[0] == ":w" && command[input[0].size()] != ' ')
    {
        fm.saveFile(input[1], *this);
        return;
    }
    else if (input[0] == ":e" && command[input[0].size()] != ' ')
    {
        fm.loadFile(input[1], *this);
        return;
    }
    else if (command == ":c") // For displaying command list
    {
        cmdlist = true;
        return;
    }
    else if (input[0] == ":d" && command[input[0].size()] != ' ')
    {
        int num = stoi(input[1]);
        deleteNline(num - 1);
        return;
    }
    else if (input[0] == "pattern" && command[input[0].size()] != ' ')
    {
        se.search(input[1], *this);
        return;
    }
    else if (command[0] == 's' && command[1] == '/')
    {
        afterreplaceputter(command);
        return;
    }
    invalidCmd = true;
}

void TextEditor::enterCommandMode()
{
    commandmode = true;
}

bool TextEditor::isCommandMode()
{
    return commandmode;
}

void TextEditor::save(const std::string& filename)
{
    fm.saveFile(filename, *this);
}

void TextEditor::load(const std::string& filename)
{
    fm.loadFile(filename, *this);
}

void TextEditor::moveToNextWord() {
    commands.push_back("w");
    if (cursor == nullptr) return;

    while (cursor != nullptr && !isspace(cursor.current->data)) {
        cursor++;
    }

    while (cursor != nullptr && isspace(cursor.current->data)) {
        cursor++;
    }
}

void TextEditor::moveToPreviousWord() {
    commands.push_back("b");
    if (cursor == lines[currentLine]->end())
    {
        cursor.current = lines[currentLine]->tail;
    }

    while (cursor != lines[currentLine]->begin()) {
        cursor--;
        if (!isspace(*cursor) && punctuation.find(*cursor) == std::string::npos) {
            // Move until a non-whitespace and non-punctuation character is found
            while (cursor != lines[currentLine]->begin() &&
                !isspace(*cursor) &&
                punctuation.find(*cursor) == std::string::npos) {
                cursor--;
            }
            // Correct the cursor to point to the start of the word
            if (cursor != lines[currentLine]->begin()) cursor++;
            break;
        }
    }
}

void TextEditor::moveToWordEnd() {
    commands.push_back("e");
    if (cursor == nullptr) return; // Ensure cursor is valid

    // Traverse the current word until reaching a word boundary
    while (cursor.current->next && !isspace(cursor.current->data) && cursor.current->data != '\n') {
        cursor++;
    }

    // If the cursor lands on a whitespace or newline, move back to the last character of the word
    if (isspace(cursor.current->data) || cursor.current->data == '\n') {
        cursor--;
    }
}

void TextEditor::moveToStartOfLine()
{
    commands.push_back("0");
    while (cursor != lines[currentLine]->begin())
    {
        moveLeft();
    }
    start = true;
}

void TextEditor::moveToEndOfLine()
{
    commands.push_back("$");
    while (cursor != lines[currentLine]->end())
    {
        moveRight();
    }
}

void TextEditor::pasteAfter()
{
    commands.push_back("p");
    int size = copyBuffer.size();
    if (size == 0)
    {
        return;
    }

    cursor = lines[currentLine]->end();

    enterInsertMode();

    newLine();

    for (int i = 0; i < size; i++)
    {
        if (copyBuffer[i] == '\0')
        {
            newLine();
            continue;
        }
        insertChar(copyBuffer[i]);
    }

    exitInsertMode();

    cursor = lines[currentLine]->begin();
    start = true;
}

void TextEditor::pasteBefore()
{
    commands.push_back("P");
    int size = copyBuffer.size();
    if (size == 0)
    {
        return;
    }
    
    cursor = lines[currentLine]->begin();
    start = true;

    enterInsertMode();

    newLine();

    currentLine--;

    std::string temp = copyBuffer;
    reverse(temp.begin(), temp.end());

    for (int i = 0; i < size; i++)
    {
        if (temp[i] == '\0')
        {
            lines[currentLine]->begin();
            start = true;
            newLine();
            currentLine--;
            continue;
        }
        lines[currentLine]->push_front(temp[i]);
    }

    exitInsertMode();
}

void TextEditor::yankLine()
{
    commands.push_back("yy");
    copyBuffer = "";
    LinkList<char>::Iterator i = lines[currentLine]->begin();
    while (i != lines[currentLine]->end())
    {
        copyBuffer.push_back(i.current->data);
        i++;
    }
}

void TextEditor::deleteToEndOfLine()
{
    commands.push_back("D");
    cursor.current->next = nullptr;
}

void TextEditor::deleteCurrentLine()
{
    commands.push_back("dd");
    if (currentLine == 0 && cursor == lines[currentLine]->begin())
    {
        return;
    }
    if (currentLine == 0)
    {
        cursor = lines[currentLine]->begin();
        cursor.current->next = nullptr;
        cursor--;
        lines[currentLine]->pop_front();
        return;
    }
    lines.erase(lines.begin() + currentLine);
    currentLine--;
    cursor = lines[currentLine]->end();
}

// Destructor to clean up dynamically allocated LinkLists
TextEditor::~TextEditor() {
    for (auto line : lines) {
        delete line;  // Delete each allocated LinkList
    }
}

void TextEditor::insertChar(char c) {
    if (insertmode) {
        fm.markAsModified();
        // Insert character at the cursor position
        if (start)
        {
            lines[currentLine]->push_front(c);
            start = false;
            cursor--;
            return;
        }

       lines[currentLine]->insert(++cursor, c);  // Dereference pointer to access LinkList
        cursor--;  // Move cursor to the next position
        start = false;
    }
}

void TextEditor::moveRight() {
    if (commandmode)
    {
        se.findNext(*this);
        return;
    }
    if (start)
    {
        start = false;
        return;
    }
    start = false;
    // Move the cursor to the right within the current line or to the next line if at the end
    if (cursor != lines[currentLine]->end()) {
        cursor++;  // Move right in the same line
    }
    else if (currentLine < lines.size() - 1) {
        currentLine++;
        cursor = lines[currentLine]->begin();  // Move to the start of the next line
        start = true;
    }
}

void TextEditor::moveLeft() {
    if (commandmode)
    {
        se.findPrevious(*this);
        return;
    }
    if (cursor == lines[currentLine]->begin() && !start)
    {
        start = true;
        return;
    }
    // Move the cursor to the left within the current line or to the previous line if at the beginning
    if (cursor != lines[currentLine]->begin()) {
        if (cursor == lines[currentLine]->end()) {
            cursor = lines[currentLine]->tail->prev;  // Adjust to your tail pointer logic
        }
        else {
            cursor--;
        } // Move left in the same line
    }
    else if (currentLine > 0) {
        currentLine--;
        cursor = lines[currentLine]->end();  // Move to the end of the previous line
        start = false;
        cursor--;  // One step back to make the cursor valid as .end() is nullptr
    }
}

void TextEditor::moveUp() {
    if (cmdlist)
    {
        if (command_marker < commands.size() - 1)
        {
            command_marker++;
        }
        return;
    }
    if (currentLine > 0) {
        currentLine--;
        cursor = lines[currentLine]->begin();  // Move cursor to the start of the previous line
        start = true;
    }
}

void TextEditor::moveDown() {
    if (cmdlist)
    {
        if (command_marker > 0)
        {
            command_marker--;
        }
        return;
    }
    if (currentLine < lines.size() - 1) {
        currentLine++;
        cursor = lines[currentLine]->begin();  // Move cursor to the start of the next line
        start = true;
    }
}

void TextEditor::display() {
    system("cls"); // Clear the console
    int position = lines[currentLine]->get_pos(cursor);
    
    for (int i = 0; i < lines.size(); i++) {
        LinkList<char>::Iterator temp = lines[i]->begin();  // Dereference pointer
        std::cout << i + 1 << " | ";
        if (start && i == currentLine)
        {
            std::cout << "|"; // Display cursor
        }
        while (temp != lines[i]->end()) {
            if (temp.current->highlight) {
                std::cout << "\033[42m" << *temp << "\033[0m"; // Green background
            }
            else {
                std::cout << *temp; // Normal display
            }
            // Display cursor after the character if cursor is at the current position
            if (i == currentLine && cursor.current == temp.current && !start) {
                std::cout << "|"; // Display cursor
            }

            temp++; // Move to the next character
        }

        // Display cursor at the end of the line if cursor is null (end of line)
        if (i == currentLine && cursor.current == nullptr && !start) {
            std::cout << "|";
        }

        std::cout << std::endl; // New line after each line of text
    }

    std::cout << "-----------------\n";
    if (insertmode) {
        std::cout << " INSERT MODE  ";
    }
    else if (commandmode)
    {
        std::cout << " Command Mode ";
    }
    else {
        std::cout << " NORMAL MODE  ";
    }
    std::cout << "| " << fm.getCurrentFileName();
    if (fm.hasUnsavedChanges())
    {
        std::cout << " [+]";
    }
    std::cout<< "  Line : " << currentLine + 1 << ", Col : " << position << " | Total Lines : " << lines.size() << std::endl << std::endl;
    
    if (warning)
    {
        std::cout << "Warning Unsaved Changes" << std::endl;
    }

    if (commands.size() != 0)
    {
        std::cout << "Last command : " << commands[commands.size() - 1];
    
        if (invalidCmd)
        {
            std::cout << " Is Not A Vail Command";
        }
        std::cout << std::endl;
    }
    if (cmdlist)
    {
        std::cout << "Commands History : ";
        std::cout << commands[command_marker];
    }
}

void TextEditor::newLine() {
    if (!insertmode)
    {
        commands.push_back("n");
    }
    // Create a new line list
    LinkList<char>* newLineList = new LinkList<char>();  // Allocate new LinkList

    if (start)
    {
        lines.insert(lines.begin() + currentLine, newLineList);
        currentLine++;
        return;
    }

    // Move the remaining part of the current line to the new line
    auto it = cursor;  // Store current cursor position
    it++;
    while (it != lines[currentLine]->end()) {
        newLineList->push_back(*it);  // Move characters to the new line
        it++;  // Increment iterator
    }

    // Erase the moved characters from the current line
    if (cursor != lines[currentLine]->end())
    {
        cursor.current->next = nullptr;
    }
    // Insert the new line after the current line
    lines.insert(lines.begin() + currentLine + 1, newLineList);

    // Move to the new line and set the cursor at the beginning
    currentLine++;
    cursor = lines[currentLine]->begin();  // Set cursor to the beginning of the new line
    start = true;
}

void TextEditor::enterInsertMode() {
    cmdlist = false;
    insertmode = true;
}

void TextEditor::exitInsertMode() {
    insertmode = false;
}

bool TextEditor::isInsertMode() {
    return insertmode;
}

void TextEditor::backspace()
{
    if (!insertmode)
    {
        commands.push_back("x");
    }
    if (start && currentLine == 0)
    {
        return;
    }
    if (!start && cursor == lines[currentLine]->begin())
    {
        if (cursor != nullptr)
        {
            auto i = cursor.current->next;
            lines[currentLine]->pop_front();
            cursor = i;
        }
        start = true;
        return;
    }
    if (cursor != lines[currentLine]->end() && cursor == nullptr && currentLine == 0)
    {
        return;
    }
    if (cursor == nullptr && currentLine > 0 && start)
    {
        lines.erase(lines.begin() + currentLine);
        currentLine--;
        cursor = lines[currentLine]->end();
        start = false;
        return;
    }
    if (cursor == lines[currentLine]->begin() && start)
    {
        auto i = cursor;
        if (lines[currentLine]->tail == nullptr)
        {
            lines.erase(lines.begin() + currentLine + 1);
            return;
        }
        std::string temp;
        while (cursor != lines[currentLine]->end())
        {
            temp.push_back(*cursor);
            cursor++;
        }
        currentLine--;
        start = false;
        cursor = lines[currentLine]->end();
        enterInsertMode();
        for (int i = 0; i < temp.size(); i++)
        {
            insertChar(temp[i]);
        }
        exitInsertMode();
        lines.erase(lines.begin() + currentLine + 1);
        return;
    }
    lines[currentLine]->erase(cursor);
}

bool FileManager::loadFile(const std::string& filename, TextEditor& t)
{
    std::ifstream load(filename);
    std::string file;
    std::string res;
    if (!load)
    {
        return false;
    }

    currentFileName = filename;

    while (getline(load, file)) {
        res.append(file);
        res.push_back('\n');
    }
    load.close();
    int size = res.size();
    t.enterInsertMode();
    for (int i = 0; i < size; i++)
    {
        if (res[i] == '\n') {
            t.newLine();
            continue;
        }
        t.insertChar(res[i]);
    }
    t.exitInsertMode();
    return true;
}

bool FileManager::saveFile(const std::string& filename, TextEditor& t)
{

    std::ofstream save(filename);

    if (!save)
    {
        return false;
    }

    modified = false;
    currentFileName = filename;

    int size = t.lines.size();
    for (int i = 0; i < size; i++)
    {
        auto temp = t.lines[i]->begin();

        while (temp != t.lines[i]->end())
        {
            save << *temp;
            temp++;
        }

        save << std::endl;
    }

    return true;
}

bool SearchEngine::search(const std::string& pattern, TextEditor& t)
{
    lastPattern = pattern;

    for (int i = 0; i < t.lines.size(); i++)
    {
        t.cursor = t.lines[i]->begin();
        while (t.cursor != t.lines[i]->end())
        {
            int index = 0;
            int row = i, col = t.lines[i]->get_pos(t.cursor);

            auto tempCursor = t.cursor;
            while (tempCursor != t.lines[i]->end() && index < pattern.size() &&
                pattern[index] == tempCursor.current->data)
            {
                index++;
                tempCursor++;
            }

            if (index == pattern.size()) // Match found
            {
                // Highlight the matched nodes
                auto highlightCursor = t.cursor;
                for (int j = 0; j < index; j++)
                {
                    highlightCursor.current->highlight = true;
                    highlightCursor++;
                }

                // Update the last match state
                lastMatchLine = row;
                lastMatchColumn = col;

                return true;
            }
            t.cursor++;
        }
    }
    return false;
}

bool SearchEngine::findNext(TextEditor& t)
{
    if (lastMatchLine >= 0 && lastMatchLine < t.lines.size())
    {
        auto unhighlightCursor = t.lines[lastMatchLine]->begin();
        for (int j = 0; j < lastMatchColumn && unhighlightCursor != t.lines[lastMatchLine]->end(); j++)
        {
            unhighlightCursor++;
        }

        for (int j = 0; j < lastPattern.size() && unhighlightCursor != t.lines[lastMatchLine]->end(); j++)
        {
            unhighlightCursor.current->highlight = false;
            unhighlightCursor++;
        }
    }

    for (int i = lastMatchLine; i < t.lines.size(); i++)
    {
        if (t.lines[i]->isEmpty())
            continue;

        t.cursor = t.lines[i]->begin();

        if (i == lastMatchLine)
        {
            for (int j = 0; j < lastMatchColumn + lastPattern.size() && t.cursor != t.lines[i]->end(); j++)
            {
                t.cursor++;
            }
        }

        while (t.cursor != t.lines[i]->end())
        {
            int index = 0;
            int row = i, col = t.lines[i]->get_pos(t.cursor);

            auto tempCursor = t.cursor;
            while (tempCursor != t.lines[i]->end() && index < lastPattern.size() &&
                lastPattern[index] == tempCursor.current->data)
            {
                index++;
                tempCursor++;
            }

            if (index == lastPattern.size())
            {
                auto highlightCursor = t.cursor;
                for (int j = 0; j < index; j++)
                {
                    highlightCursor.current->highlight = true;
                    highlightCursor++;
                }

                lastMatchLine = row;
                lastMatchColumn = col;
                return true;
            }

            t.cursor++;
        }
    }

    return false;
}

bool SearchEngine::findPrevious(TextEditor& t)
{
    // Unhighlight the previous match if it exists
    if (lastMatchLine >= 0 && lastMatchLine < t.lines.size())
    {
        auto unhighlightCursor = t.lines[lastMatchLine]->begin();
        for (int j = 0; j < lastMatchColumn && unhighlightCursor != t.lines[lastMatchLine]->end(); j++)
        {
            unhighlightCursor++;
        }

        for (int j = 0; j < lastPattern.size() && unhighlightCursor != t.lines[lastMatchLine]->end(); j++)
        {
            unhighlightCursor.current->highlight = false;
            unhighlightCursor++;
        }
    }

    // Traverse lines in reverse
    for (int i = lastMatchLine; i >= 0; i--)
    {
        auto& line = t.lines[i];
        auto cursor = line->end();

        // Adjust cursor to start searching from the last match column in the last matched line
        if (i == lastMatchLine)
        {
            int pos = line->get_pos(cursor);
            while (pos > lastMatchColumn && cursor != line->begin())
            {
                --cursor;
                --pos;
            }
        }

        // Reverse search within the line
        while (cursor != line->begin() && cursor != nullptr)
        {
            --cursor; // Move cursor back
            int index = lastPattern.size() - 1;

            // Temp cursor to check the pattern in reverse
            auto tempCursor = cursor;
            int matchColumn = line->get_pos(tempCursor);

            while (index >= 0 && tempCursor != line->begin() && tempCursor.current != nullptr && lastPattern[index] == tempCursor.current->data)
            {
                --index;
                cursor--;
                --tempCursor;
                --matchColumn;
            }

            // Check the first character at the beginning of the line
            if (index >= 0 && tempCursor == line->begin())
            {
                if (tempCursor.current != nullptr && lastPattern[index] == tempCursor.current->data)
                {
                    --index;
                    --matchColumn;
                }
            }

            // Match found
            if (index == -1)
            {
                // Highlight the match
                auto highlightCursor = cursor;
                for (int j = 0; j < lastPattern.size() && highlightCursor != line->end(); j++)
                {
                    highlightCursor.current->highlight = true;
                    highlightCursor++;
                }

                // Update the last match position
                lastMatchLine = i;
                lastMatchColumn = matchColumn;
                return true;
            }
        }
    }

    // No match found
    return false;
}

int getChar(std::string& prev_hit) {
    int ch = _getch();
    int n = prev_hit.size();

    if (n > 0)
    {
        n = n - 1;
    }

    if (ch == '/')
    {
        prev_hit = "/";
        return 444; // /
    }

    if (ch == ':')
    {
        prev_hit = "";
        return 555; // :
    }

    if (prev_hit[n] == '<' && ch == '<')
    {
        prev_hit = "";
        return 1234;
    }

    if (prev_hit[n] == '>' && ch == '>')
    {
        prev_hit = "";
        return 4321;
    }

    if (prev_hit[n] == 'd' && ch == 'd')
    {
        prev_hit = "";
        
        return 999; // dd
    }

    if (prev_hit[n] == 'y' && ch == 'y')
    {
        prev_hit = "";
        return 888; //yy
    }
    prev_hit = ch;

    if (ch == 0 || ch == 224) {  // Check for special keys
        ch = _getch();  // Get the second byte
        prev_hit = ch;
        switch (ch) {
        case 72: return 65;  // Up arrow
        case 80: return 66;  // Down arrow
        case 77: return 67;  // Right arrow
        case 75: return 1068;  // Left arrow
        default: return ch + 256;  // Other special keys
        }
    }

    if (ch == 13) {
        return 10;  // for Enter
    }

    if (ch == 8) {
        return -1;  // for Backspace
    }
    return ch;
}