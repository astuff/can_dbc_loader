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

#ifndef ATTRIBUTE_HPP_
#define ATTRIBUTE_HPP_

#include "common_defs.hpp"

#include <limits>
#include <memory>
#include <sstream>
#include <vector>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

class Attribute
  : public DbcObj
{
public:
  virtual std::string getDefaultValueDbcText() const;
  virtual std::string getName() const;
  virtual DbcObjType getDbcObjType() const;
  virtual AttributeType getAttrType() const = 0;

protected:
  void generateText() override;
  void parse() override;

  std::string default_value_dbc_text_;
  std::string name_;
  DbcObjType dbc_obj_type_;

private:
  virtual void generateDefaultValueText() = 0;
  virtual std::string generateTypeSpecificText() = 0;
  virtual void parseDefaultValue() = 0;
  virtual void parseTypeSpecificValues(std::istringstream & stream) = 0;
};

class EnumAttribute
  : public Attribute
{
public:
  EnumAttribute(
    std::string && dbc_text,
    std::string && default_value_dbc_text);
  EnumAttribute(
    std::string && name,
    DbcObjType && dbc_obj_type,
    std::vector<std::string> && enum_values,
    std::string * default_value = nullptr);
  ~EnumAttribute() = default;
  EnumAttribute(const EnumAttribute & other);
  EnumAttribute(EnumAttribute && other) = default;
  EnumAttribute & operator=(const EnumAttribute & other);
  EnumAttribute & operator=(EnumAttribute && other) = default;

  std::vector<const std::string *> getEnumValues() const;
  const std::string * getDefaultValue() const;
  AttributeType getAttrType() const { return AttributeType::ENUM; };

private:
  void generateDefaultValueText();
  std::string generateTypeSpecificText();
  void parseDefaultValue();
  void parseTypeSpecificValues(std::istringstream & stream);

  std::vector<std::string> enum_values_;
  std::unique_ptr<std::string> default_value_;
};

class FloatAttribute
  : public Attribute
{
public:
  FloatAttribute(
    std::string && dbc_text,
    std::string && default_value_dbc_text);
  FloatAttribute(
    std::string && name,
    DbcObjType && dbc_obj_type,
    float min, float max,
    float * default_value = nullptr);
  ~FloatAttribute() = default;
  FloatAttribute(const FloatAttribute & other);
  FloatAttribute(FloatAttribute && other) = default;
  FloatAttribute & operator=(const FloatAttribute & other);
  FloatAttribute & operator=(FloatAttribute && other) = default;

  float getMin() const;
  float getMax() const;
  const float * getDefaultValue() const;
  AttributeType getAttrType() const { return AttributeType::FLOAT; };

private:
  void generateDefaultValueText();
  std::string generateTypeSpecificText();
  void parseDefaultValue();
  void parseTypeSpecificValues(std::istringstream & stream);

  float min_, max_;
  std::unique_ptr<float> default_value_;
};

class IntAttribute
  : public Attribute
{
public:
  IntAttribute(
    std::string && dbc_text,
    std::string && default_value_dbc_text);
  IntAttribute(
    std::string && name,
    DbcObjType && dbc_obj_type,
    int min, int max,
    int * default_value = nullptr);
  ~IntAttribute() = default;
  IntAttribute(const IntAttribute & other);
  IntAttribute(IntAttribute && other) = default;
  IntAttribute & operator=(const IntAttribute & other);
  IntAttribute & operator=(IntAttribute && other) = default;

  int getMin() const;
  int getMax() const;
  const int * getDefaultValue() const;
  AttributeType getAttrType() const { return AttributeType::INT; };

private:
  void generateDefaultValueText();
  std::string generateTypeSpecificText();
  void parseDefaultValue();
  void parseTypeSpecificValues(std::istringstream & stream);

  int min_, max_;
  std::unique_ptr<int> default_value_;
};

class StringAttribute
  : public Attribute
{
public:
  StringAttribute(
    std::string && dbc_text,
    std::string && default_value_dbc_text);
  StringAttribute(
    std::string && name,
    DbcObjType && dbc_obj_type,
    std::string * default_value = nullptr);
  ~StringAttribute() = default;
  StringAttribute(const StringAttribute & other);
  StringAttribute(StringAttribute && other) = default;
  StringAttribute & operator=(const StringAttribute & other);
  StringAttribute & operator=(StringAttribute && other) = default;

  AttributeType getAttrType() const { return AttributeType::STRING; };
  const std::string * getDefaultValue() const;

private:
  void generateDefaultValueText();
  std::string generateTypeSpecificText();
  void parseDefaultValue();
  void parseTypeSpecificValues(std::istringstream & stream);

  std::unique_ptr<std::string> default_value_;
};

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS

#endif  // ATTRIBUTE_HPP_
