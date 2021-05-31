#ifndef STRECVALUES
#define STRECVALUES

#include "strstunbase.hpp"
class Visitor;

enum ValType: int {
    INT,
    BOOL,
    STRING,
    UNKNOWN
};

const std::string ValTypeRepr[] =
{
    "INT",
    "BOOL",
    "STRING",
    "UNKNOWN"
};

enum ValStatus: int {
    VALID,
    DONT_CARE,
    ERR
};

const std::string ValStatusRepr[] =
{
    "VALID",
    "DONT_CARE",
    "ERR"
};

class ValBase : public ObjBase {
public:
  ValType type;
  ValStatus status;
  virtual bool equalTo(ValBase* compareVal) = 0;
  bool isDontCare();
  bool isErr();
  bool isValid();
  void print(std::ostream& out);
  void accept(Visitor* visitor) override;
};

class ValInt : public ValBase {
public:
    int value;
    ValInt(int val);
    ValInt(int val, ValStatus status);
    bool equalTo(ValBase* compareVal) override;
};

class ValBool : public ValBase {
public:
    bool value;
    ValBool(bool val);
    ValBool(bool val, ValStatus status);
    bool equalTo(ValBase* compareVal) override;
};

class ValStr : public ValBase {
public:
    std::string value;
    ValStr(std::string val);
    ValStr(std::string val, ValStatus status);
    bool equalTo(ValBase* compareVal) override;
};

#endif