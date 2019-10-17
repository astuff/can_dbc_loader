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

#include "common_defs.hpp"
#include "attribute.hpp"

#include <memory>
#include <vector>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

Attribute::Attribute(
  std::string && name,
  DbcObjType && dbc_obj_type,
  AttributeType && attr_type)
  : name(name),
    dbc_obj_type(dbc_obj_type),
    attr_type(attr_type)
{
}

EnumAttribute::EnumAttribute(
  std::string && name,
  DbcObjType && dbc_obj_type,
  std::vector<std::string> && enum_values)
  : Attribute(std::move(name), std::move(dbc_obj_type), AttributeType::ENUM),
    enum_values(enum_values), value_(nullptr), default_value_(nullptr)
{
}

const std::unique_ptr<std::string> EnumAttribute::getValue()
{
  if (value_) {
    return std::make_unique<std::string>(*value_);
  } else if (default_value_) {
    return std::make_unique<std::string>(*default_value_);
  } else {
    return nullptr;
  }
}

FloatAttribute::FloatAttribute(
  std::string && name,
  DbcObjType && dbc_obj_type,
  float min, float max)
  : Attribute(std::move(name), std::move(dbc_obj_type), AttributeType::FLOAT),
    min(min), max(max), value_(nullptr), default_value_(nullptr)
{
}

const std::unique_ptr<float> FloatAttribute::getValue()
{
  if (value_) {
    return std::make_unique<float>(*value_);
  } else if (default_value_) {
    return std::make_unique<float>(*default_value_);
  } else {
    return nullptr;
  }
}

IntAttribute::IntAttribute(
  std::string && name,
  DbcObjType && dbc_obj_type,
  int min, int max)
  : Attribute(std::move(name), std::move(dbc_obj_type), AttributeType::INT),
    min(min), max(max), value_(nullptr), default_value_(nullptr)
{
}

const std::unique_ptr<int> IntAttribute::getValue()
{
  if (value_) {
    return std::make_unique<int>(*value_);
  } else if (default_value_) {
    return std::make_unique<int>(*default_value_);
  } else {
    return nullptr;
  }
}

StringAttribute::StringAttribute(
  std::string && name,
  DbcObjType && dbc_obj_type)
  : Attribute(std::move(name), std::move(dbc_obj_type)),
    value_(nullptr), default_value_(nullptr)
{
}

const std::unique_ptr<std::string> StringAttribute::getValue()
{
  if (value_) {
    return std::make_unique<std::string>(*value_);
  } else if (default_value_) {
    return std::make_unique<std::string>(*default_value_);
  } else {
    return nullptr;
  }
}

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS
