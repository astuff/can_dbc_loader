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

#include <memory>
#include <vector>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

class Attribute
{
public:
  virtual void parseDefaultValue(std::string && dbc_text) = 0;
  std::string getDefaultValueDbcText();
  std::string getName();
  DbcObjType getDbcObjType();
  AttributeType getAttrType();

  auto clone() const
  {
    return std::unique_ptr<Attribute>(clone_impl());
  }

  virtual Attribute* clone_impl() const = 0;

protected:
  std::string default_value_dbc_text_;
  std::string name_;
  DbcObjType dbc_obj_type_;
  AttributeType attr_type_;

private:
  virtual void generateDefaultValueText() = 0;
};

class EnumAttribute
  : public Attribute, public DbcObj
{
public:
  EnumAttribute(std::string && dbc_text);
  EnumAttribute(
    std::string && name,
    DbcObjType && dbc_obj_type,
    std::vector<std::string> && enum_values);
  ~EnumAttribute() = default;
  EnumAttribute(const EnumAttribute & other);
  EnumAttribute(EnumAttribute && other) = default;
  EnumAttribute & operator=(const EnumAttribute & other);
  EnumAttribute & operator=(EnumAttribute && other) = default;

  const std::string * getDefaultValue();
  std::vector<std::string> getEnumValues();
  void parseDefaultValue(std::string && dbc_text) override;
  void setDefaultValue(std::string && default_value);

private:
  void generateDefaultValueText() override;
  void generateText() override;
  void parse() override;

  EnumAttribute* clone_impl() const override
  {
    return new EnumAttribute(*this);
  }

  std::vector<std::string> enum_values_;
  std::unique_ptr<std::string> default_value_;
};

class FloatAttribute
  : public Attribute, public DbcObj
{
public:
  FloatAttribute(std::string && dbc_text);
  FloatAttribute(
    std::string && name,
    DbcObjType && dbc_obj_type,
    float min, float max);
  ~FloatAttribute() = default;
  FloatAttribute(const FloatAttribute & other);
  FloatAttribute(FloatAttribute && other) = default;
  FloatAttribute & operator=(const FloatAttribute & other);
  FloatAttribute & operator=(FloatAttribute && other) = default;

  const float * getDefaultValue();
  float getMin();
  float getMax();
  void parseDefaultValue(std::string && dbc_text) override;
  void setDefaultValue(float default_value);

private:
  void generateDefaultValueText() override;
  void generateText() override;
  void parse() override;

  FloatAttribute* clone_impl() const override
  {
    return new FloatAttribute(*this);
  }

  float min_, max_;
  std::unique_ptr<float> default_value_;
};

class IntAttribute
  : public Attribute, public DbcObj
{
public:
  IntAttribute(std::string && dbc_text);
  IntAttribute(
    std::string && name,
    DbcObjType && dbc_obj_type,
    int min, int max);
  ~IntAttribute() = default;
  IntAttribute(const IntAttribute & other);
  IntAttribute(IntAttribute && other) = default;
  IntAttribute & operator=(const IntAttribute & other);
  IntAttribute & operator=(IntAttribute && other) = default;

  const int * getDefaultValue();
  int getMin();
  int getMax();
  void parseDefaultValue(std::string && dbc_text) override;
  void setDefaultValue(int default_value);

private:
  void generateDefaultValueText() override;
  void generateText() override;
  void parse() override;

  IntAttribute* clone_impl() const override
  {
    return new IntAttribute(*this);
  }

  int min_, max_;
  std::unique_ptr<int> default_value_;
};

class StringAttribute
  : public Attribute, public DbcObj
{
public:
  StringAttribute(std::string && dbc_text);
  StringAttribute(
    std::string && name,
    DbcObjType && dbc_obj_type);
  ~StringAttribute() = default;
  StringAttribute(const StringAttribute & other);
  StringAttribute(StringAttribute && other) = default;
  StringAttribute & operator=(const StringAttribute & other);
  StringAttribute & operator=(StringAttribute && other) = default;

  const std::string * getDefaultValue();
  void parseDefaultValue(std::string && dbc_text) override;
  void setDefaultValue(std::string && default_value);

private:
  void generateDefaultValueText() override;
  void generateText() override;
  void parse() override;

  StringAttribute* clone_impl() const override
  {
    return new StringAttribute(*this);
  }

  std::unique_ptr<std::string> default_value_;
};

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS

#endif  // ATTRIBUTE_HPP_
