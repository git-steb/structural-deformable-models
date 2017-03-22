#ifndef _PARSEFILE_H_
#define _PARSEFILE_H_

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "simpletypes.h"

#ifndef COMMENT_CHAR
#define COMMENT_CHAR '#'
#endif

class ParseFile {
public:
    enum ErrorID {ERR_OK=0, ERR_FILE, ERR_PARSE, ERR_EOF };

ParseFile(const std::string& filename)
    : m_IS(m_IF.rdbuf()), m_Error(ERR_OK), c_CommentChar('#') {
        open(filename);
    }

ParseFile(std::istream& is)
    : m_IS(is.rdbuf()), m_Error(ERR_OK), c_CommentChar('#') {}

    bool open(const std::string& filename) {
        resetStrings();
        m_IF.open(filename.c_str());
        if(m_IF) {
            attach(m_IF);
            m_Filename = filename;
            m_Path = getPath(filename);
            return true;
        } else { m_Error = ERR_FILE; return false; }
    }
    void attach(std::istream& is) {
        resetStrings();
        m_IS.rdbuf(is.rdbuf());
    }
    void close() {
        m_Error = ERR_OK;
        m_LineN = 0;
        m_Line.clear(); m_Key.clear(); m_Value.clear();
        m_Filename.clear(); m_Path.clear();
        m_IF.close();
    }
    bool good() const { return m_Error == ERR_OK; }
    bool eof() const { return m_Error == ERR_EOF; }
    bool error() const { return m_Error!=ERR_OK && m_Error!=ERR_EOF; }

    const std::string& getPath() const { return m_Path; }
    const std::string& getFilename() const { return m_Filename; }

    const std::string& getLine() const { return m_Line; }
    const std::string& getKey() const { return m_Key; }
    const std::string& getValue() const { return m_Value; }
    const int getLineNumber() const { return m_LineN; }
    const bool getNextLine() {
        if(m_Error != ERR_OK) return false;
        m_LastLine = m_Line;
        m_Line.clear(); m_Key.clear(); m_Value.clear();
        while(m_Key.empty() &&
              (!m_PushedLines.empty() ? true : (bool)std::getline(m_IS, m_Line))) {
            if(!m_PushedLines.empty()) {
                m_Line = m_PushedLines.back();
                m_PushedLines.pop_back();
            } else m_LineN++;
            parseLine();
        }
        if(m_Key.empty()) { m_Error = ERR_EOF; return false; }
        return true;
    }
    void pushLine(const std::string& line) { m_PushedLines.push_back(line); }
    void pushLine() {
        pushLine(m_Line);
        if(!m_LastLine.empty())
        { m_Line = m_LastLine; m_LastLine.clear(); parseLine(); }
    }
    void setParseError(const std::string& msg = "")
    { if(m_Error == ERR_OK) { m_Error = ERR_PARSE; m_ErrMsg = msg; } }
    char getCommentChar() const { return c_CommentChar; }
    void setCommentChar(char cchar) { ((char&)c_CommentChar) = cchar; }

    std::string getErrorMsg() const {
        std::ostringstream msg;
        msg << "[file " << m_Filename << "] ";
        switch(m_Error) {
        case ERR_OK: msg << "none"; break;
        case ERR_FILE: msg << "error opening file"; break;
        case ERR_PARSE: msg << "parse error in line " << m_LineN;
            break;
        case ERR_EOF: msg << "end of file"; break;
        }
        if(!m_ErrMsg.empty()) msg << ": " << m_ErrMsg;
        return msg.str();
    }
    void appendErrMsg(const std::string& msg) { m_ErrMsg = msg; }

    operator bool () const { return good(); }
    //operator std::ifstream& () { return m_IF; }
    operator std::istream& () { return m_IS; }

// --------- static -----------

    static std::string getPath(const std::string& sfilename) {
        dword slashpos = sfilename.rfind('/');
        if(slashpos == sfilename.npos) slashpos = sfilename.rfind('\\');
        if(slashpos != sfilename.npos) return sfilename.substr(0,slashpos+1);
        else return std::string("./");
    }
protected:
    void resetStrings() {
        m_Error = ERR_OK;
        m_LineN = 0;
        m_Line.clear(); m_Key.clear(); m_Value.clear(); m_LastLine.clear();
        m_PushedLines.clear();
        m_ErrMsg.clear();
        m_Filename.clear(); m_Path.clear();
    }
    void parseLine() {
        m_Line = m_Line.substr(0,m_Line.find(c_CommentChar));
        std::string::reverse_iterator ch = m_Line.rbegin();
        while(ch!=m_Line.rend() && *ch<=32) ch++; //moves backwards
        m_Line = m_Line.substr(0,m_Line.size()-(ch-m_Line.rbegin()));
        std::istringstream lines(m_Line);
        std::string token, value;
        if(lines >> m_Key) {
            lines >> std::ws;
            std::getline(lines, m_Value);
        }
    }
protected:
    std::ifstream               m_IF;
    std::istream                m_IS;
    std::string                 m_Filename, m_Path;
    enum ErrorID                m_Error;
    std::list<std::string>      m_PushedLines;
    std::string                 m_Line, m_LastLine;
    int                         m_LineN;
    std::string                 m_Key, m_Value;
    std::string                 m_ErrMsg;
    const char                  c_CommentChar;
};

#endif
