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

// TODO(jwhitleyastuff): Store Attribute as def, default val, and value

class Attribute
{
public:
  Attribute(
    std::string && name,
    DbcObjType && dbc_obj_type,
    AttributeType && attr_type = AttributeType::STRING);
  virtual ~Attribute() {};

  const std::string name;
  const DbcObjType dbc_obj_type;
  const AttributeType attr_type;
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

  const std::unique_ptr<std::string> getValue();
  const std::vector<std::string> enum_values;

private:
  void generateText() override;
  void parse() override;

  std::unique_ptr<std::string> value_;
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

  const std::unique_ptr<float> getValue();
  const float min, max;

private:
  void generateText() override;
  void parse() override;

  std::unique_ptr<float> value_;
  std::unique_ptr<float> default_value_;
};

class IntAttribute
  : public Attribute, public DbcObj
{
public:
  IntAttribute(
    std::string && name,
    DbcObjType && dbc_obj_type,
    int min, int max);
  ~IntAttribute() = default;

  const std::unique_ptr<int> getValue();
  const int min, max;

private:
  void generateText() override;
  void parse() override;

  std::unique_ptr<int> value_;
  std::unique_ptr<int> default_value_;
};

class StringAttribute
  : public Attribute, public DbcObj
{
public:
  StringAttribute(
    std::string && name,
    DbcObjType && dbc_obj_type);
  ~StringAttribute() = default;

  const std::unique_ptr<std::string> getValue();

private:
  void generateText() override;
  void parse() override;

  std::unique_ptr<std::string> value_;
  std::unique_ptr<std::string> default_value_;
};

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS

#endif  // ATTRIBUTE_HPP_
