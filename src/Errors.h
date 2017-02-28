/* -*- C++ -*- */
#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <string.h>

// a collection of exceptions that can be thrown

class Exception {};

class IOException : public Exception {
 public:
    IOException(const char* msg = NULL) : message(NULL) {
	setMessage(msg);
    }
    ~IOException() {
	if(message) {
	    delete message;
	    message = NULL;
	}
    }
    void setMessage(const char* msg=NULL) {
	if(!msg) {
	    if(message) {
		if(message[0] != 0) {
		    delete message;
		    message = new char[1];
		    message[0] = 0;
		}
	    } else {
		message = new char[1];
		message[0] = 0;
	    }
	    return;
	}
	int len = strlen(msg);
	if(!len) setMessage(NULL);
	message = new char[len+1];
	strcpy(message, msg);
    }
    const char* getMessage() const { return message; }
 protected:
    char *message;
};

class SyntaxErrorException : public IOException {};

#endif
