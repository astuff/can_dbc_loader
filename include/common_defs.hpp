// Copyright (c) 2019 AutonomouStuff, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef COMMON_DEFS_HPP_
#define COMMON_DEFS_HPP_

#include <array>
#include <exception>
#include <unordered_map>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

static constexpr unsigned int MAX_CAN_ID = 0x1FFFFFFF;

static constexpr std::array<unsigned char, 16> DLC_LENGTH =
{ 0,  1,  2,  3,  4,  5,  6,  7,
  8, 12, 16, 20, 24, 32, 48, 64 };

// Currently unsupported types:
//   BA_DEF_DEF_REL_
//   BA_DEF_REL_
//   BA_DEF_SGTYPE_
//   BA_REL_
//   BA_SGTYPE_
//   BO_TX_BU_
//   BU_BO_REL_
//   BU_EV_REL_
//   BU_SG_REL_
//   CAT_
//   CAT_DEF_
//   ENVVAR_DATA_
//   EV_
//   EV_DATA_
//   FILTER
//   NS_DESC_
//   SGTYPE_
//   SGTYPE_VAL_
//   SG_MUL_VAL_
//   SIGTYPE_VALTYPE_
//   SIG_GROUP_
//   SIG_TYPE_REF_
//   SIG_VALTYPE_
//   VAL_TABLE_

static const std::array<std::string, 10> PREAMBLES =
{
  "VERSION",      // VERSION
  "BS_:",         // BUS CONFIG
  "BU_:",         // BUS NODES
  "BO_",          // MESSAGE
  "SG_",          // SIGNAL
  "CM_",          // COMMENT
  "VAL_",         // SIGNAL VALUE LIST
  "BA_DEF_",      // ATTRIBUTE DEFINITION
  "BA_DEF_DEF_",  // ATTRIBUTE DEFAULT VALUE
  "BA_"           // ATTRIBUTE VALUE
};

enum class DbcObjType
{
  VERSION,
  BUS_CONFIG,
  BUS_NODES,
  MESSAGE,
  SIGNAL,
  DESCRIPTION,
  SIGNAL_VAL_DEF,
  ATTRIBUTE_DEF,
  ATTRIBUTE_VAL
};

enum class AttributeType
{
  ENUM,
  FLOAT,
  HEX,
  INT,
  STRING
};

enum class Order
{
  BE,
  LE
};

struct DbcReadException
  : public std::exception
{
  const char * what() const throw()
  {
    return "Exception when reading DBC file from disk.";
  }
};

struct DbcWriteException
  : public std::exception
{
  const char * what() const throw()
  {
    return "Exception when writing DBC file to disk.";
  }
};

struct DbcParseException
  : public std::exception
{
  const char * what() const throw()
  {
    return "Exception when parsing DBC object.";
  }
};

class DbcObj
{
public:
  virtual ~DbcObj() {};
  const std::string getDbcText()
  {
    return dbc_text_;
  }

  friend class Database;

protected:
  std::string dbc_text_;

private:
  virtual void generateText() = 0;
  virtual void parse() = 0;
};

class AttrObj
{
public:
  const std::unordered_map<std::string, std::string> getAttributeValues()
  {
    return attribute_values_;
  }

  const bool hasAttributeValues()
  {
    return (attribute_values_.size() < 1);
  }

protected:
  std::unordered_map<std::string, std::string> attribute_values_;
};

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS

#endif  // COMMON_DEFS_HPP_
