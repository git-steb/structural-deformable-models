#ifndef _DTYPE_H_
#define _DTYPE_H_

class DType {
 public:
    ~DType() {};
    virtual bool isSameType(const DType* inst) const = 0;
    virtual DType* createInstance() { return NULL; }
};

#endif
