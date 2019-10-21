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
#include <sstream>
#include <vector>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

std::string Attribute::getDefaultValueDbcText()
{
  return default_value_dbc_text_;
}

std::string Attribute::getName()
{
  return name_;
}

DbcObjType Attribute::getDbcObjType()
{
  return dbc_obj_type_;
}

AttributeType Attribute::getAttrType()
{
  return attr_type_;
}

EnumAttribute::EnumAttribute(std::string && dbc_text)
{
  dbc_text_ = dbc_text;
  parse();
}

EnumAttribute::EnumAttribute(
  std::string && name,
  DbcObjType && dbc_obj_type,
  std::vector<std::string> && enum_values)
    : enum_values_(enum_values), default_value_(nullptr)
{
  name_ = std::move(name);
  dbc_obj_type_ = std::move(dbc_obj_type);
  attr_type_ = AttributeType::ENUM;
  generateText();
}

EnumAttribute::EnumAttribute(const EnumAttribute & other)
{
  dbc_text_ = other.dbc_text_;
  default_value_dbc_text_ = other.default_value_dbc_text_;
  name_ = other.name_;
  
  if (other.default_value_) {
    default_value_ = std::make_unique<std::string>(*(other.default_value_));
  } else {
    default_value_ = nullptr;
  }

  dbc_obj_type_ = other.dbc_obj_type_;
  attr_type_ = other.attr_type_;
}

EnumAttribute & EnumAttribute::operator=(const EnumAttribute & other)
{
  return *this = EnumAttribute(other);
}

const std::string * EnumAttribute::getDefaultValue()
{
  return default_value_.get();
}

void EnumAttribute::parseDefaultValue(std::string && dbc_text)
{
  default_value_dbc_text_ = dbc_text;

  std::istringstream input(default_value_dbc_text_);
  std::string temp_string;

  input.ignore(12);
  input >> temp_string;

  // Sometimes 2 spaces between preamble and def
  if (temp_string.empty()) {
    input >> temp_string;
  }

  // Attribute name
  input >> temp_string;
  // Default value
  input >> temp_string;

  setDefaultValue(std::move(temp_string));
}

void EnumAttribute::setDefaultValue(std::string && default_value)
{
  default_value_ = std::make_unique<std::string>(std::move(default_value));
  generateDefaultValueText();
}

void EnumAttribute::generateDefaultValueText()
{
  // TODO(jwhitleyastuff): DO THE THING!
}

void EnumAttribute::generateText()
{
  std::ostringstream output;

  output << "BA_DEF_ ";

  switch (dbc_obj_type_) {
    case DbcObjType::BUS_NODES:
      output << "BU_ ";
      break;
    case DbcObjType::MESSAGE:
      output << "BO_ ";
      break;
    case DbcObjType::SIGNAL:
      output << "SG_ ";
      break;
  }

  output << "\"" << name_ << "\" ENUM ";

  for (const auto & val : enum_values_) {
    output << "\"" << val << "\"";
    
    if (val != enum_values_[enum_values_.size() - 1]) {
      output << ",";
    }
  }

  output << ";" << std::endl;

  dbc_text_ = output.str();

  generateDefaultValueText();
}

void EnumAttribute::parse()
{
  std::istringstream input(dbc_text_);
  std::string temp_string;
  std::string enum_val;

  input.ignore(12);
  input >> temp_string;

  // Sometimes two spaces between preamble and def
  if (temp_string.empty()) {
    input >> temp_string;
  }

  // Attribute name
  input >> name_;
  // Attribute type
  input >> temp_string;

  // Sometimes two spaces between type and values
  if (temp_string.empty()) {
    input >> temp_string;
  }

  auto first_comma = temp_string.find(",");

  if (first_comma != std::string::npos) {
    while (std::getline(std::istringstream(temp_string), enum_val, ',')) {
      enum_val = enum_val.substr(1, enum_val.length() - 2);

      // Remove ending semicolon
      if (enum_val[enum_val.length() - 1] == ';') {
        enum_val = enum_val.substr(0, enum_val.length() - 1);
      }

      enum_values_.emplace_back(std::move(enum_val));
    }
  } else {
    // Remove parentheses and ending semicolon
    temp_string = temp_string.substr(1, temp_string.length() - 3);
    enum_values_.emplace_back(std::move(temp_string));
  }
}

FloatAttribute::FloatAttribute(std::string && dbc_text)
{
  dbc_text_ = dbc_text;
  parse();
}

FloatAttribute::FloatAttribute(
  std::string && name,
  DbcObjType && dbc_obj_type,
  float min, float max)
    : min_(min), max_(max), default_value_(nullptr)
{
  name_ = std::move(name);
  dbc_obj_type_ = std::move(dbc_obj_type);
  attr_type_ = AttributeType::FLOAT;
  generateText();
}

FloatAttribute::FloatAttribute(const FloatAttribute & other)
  : min_(other.min_), max_(other.max_)
{
  dbc_text_ = other.dbc_text_;
  default_value_dbc_text_ = other.default_value_dbc_text_;
  name_ = other.name_;
  
  if (other.default_value_) {
    default_value_ = std::make_unique<float>(*(other.default_value_));
  } else {
    default_value_ = nullptr;
  }

  dbc_obj_type_ = other.dbc_obj_type_;
  attr_type_ = other.attr_type_;
}

const float * FloatAttribute::getDefaultValue()
{
  return default_value_.get();
}

FloatAttribute & FloatAttribute::operator=(const FloatAttribute & other)
{
  return *this = FloatAttribute(other);
}

void FloatAttribute::parseDefaultValue(std::string && dbc_text)
{
  default_value_dbc_text_ = dbc_text;

  std::istringstream input(default_value_dbc_text_);
  std::string temp_string;

  input.ignore(12);

  input >> temp_string;

  // Sometimes 2 spaces between preamble and def
  if (temp_string.empty()) {
    input >> temp_string;
  }

  // Attribute name
  input >> temp_string;
  // Default value
  input >> temp_string;

  setDefaultValue(std::stof(temp_string));
}

void FloatAttribute::setDefaultValue(float default_value)
{
  default_value_ = std::make_unique<float>(default_value);
  generateDefaultValueText();
}

void FloatAttribute::generateDefaultValueText()
{
  // TODO(jwhitleyastuff): DO THE THING!
}

void FloatAttribute::generateText()
{
  std::ostringstream output;

  output << "BA_DEF_ ";

  switch (dbc_obj_type_) {
    case DbcObjType::BUS_NODES:
      output << "BU_ ";
      break;
    case DbcObjType::MESSAGE:
      output << "BO_ ";
      break;
    case DbcObjType::SIGNAL:
      output << "SG_ ";
      break;
  }

  output << "\"" << name_ << "\" FLOAT ";
  output << min_ << " " << max_ << ";";
  output << std::endl;

  dbc_text_ = output.str();

  generateDefaultValueText();
}

void FloatAttribute::parse()
{
  std::istringstream input(dbc_text_);
  std::string temp_string;
  std::string enum_val;

  input.ignore(12);
  input >> temp_string;

  // Sometimes two spaces between preamble and def
  if (temp_string.empty()) {
    input >> temp_string;
  }

  // Attribute name
  input >> name_;
  // Attribute type
  input >> temp_string;

  // Sometimes two spaces between type and values
  if (temp_string.empty()) {
    input >> temp_string;
  }

  input >> temp_string;
  min_ = std::stof(temp_string);
  input >> temp_string;

  // Remove ending semicolon
  temp_string = temp_string.substr(0, temp_string.length() - 1);

  max_ = std::stof(temp_string);
}

IntAttribute::IntAttribute(std::string && dbc_text)
{
  dbc_text_ = dbc_text;
  parse();
}

IntAttribute::IntAttribute(
  std::string && name,
  DbcObjType && dbc_obj_type,
  int min, int max)
    : min_(min), max_(max), default_value_(nullptr)
{
  name_ = std::move(name);
  dbc_obj_type_ = std::move(dbc_obj_type);
  attr_type_ = AttributeType::INT;
  generateText();
}

IntAttribute::IntAttribute(const IntAttribute & other)
  : min_(other.min_), max_(other.max_)
{
  dbc_text_ = other.dbc_text_;
  default_value_dbc_text_ = other.default_value_dbc_text_;
  name_ = other.name_;
  
  if (other.default_value_) {
    default_value_ = std::make_unique<int>(*(other.default_value_));
  } else {
    default_value_ = nullptr;
  }

  dbc_obj_type_ = other.dbc_obj_type_;
  attr_type_ = other.attr_type_;
}

IntAttribute & IntAttribute::operator=(const IntAttribute & other)
{
  return *this = IntAttribute(other);
}

const int * IntAttribute::getDefaultValue()
{
  return default_value_.get();
}

void IntAttribute::parseDefaultValue(std::string && dbc_text)
{
  default_value_dbc_text_ = dbc_text;

  std::istringstream input(default_value_dbc_text_);
  std::string temp_string;

  input.ignore(12);

  input >> temp_string;

  // Sometimes 2 spaces between preamble and def
  if (temp_string.empty()) {
    input >> temp_string;
  }

  // Attribute name
  input >> temp_string;
  // Default value
  input >> temp_string;

  setDefaultValue(std::stoi(temp_string));
}

void IntAttribute::setDefaultValue(int default_value)
{
  default_value_ = std::make_unique<int>(default_value);
  generateDefaultValueText();
}

void IntAttribute::generateDefaultValueText()
{
  // TODO(jwhitleyastuff): DO THE THING!
}

void IntAttribute::generateText()
{
  std::ostringstream output;

  output << "BA_DEF_ ";

  switch (dbc_obj_type_) {
    case DbcObjType::BUS_NODES:
      output << "BU_ ";
      break;
    case DbcObjType::MESSAGE:
      output << "BO_ ";
      break;
    case DbcObjType::SIGNAL:
      output << "SG_ ";
      break;
  }

  output << "\"" << name_ << "\" INT ";
  output << min_ << " " << max_ << ";";
  output << std::endl;

  dbc_text_ = output.str();

  generateDefaultValueText();
}

void IntAttribute::parse()
{
  std::istringstream input(dbc_text_);
  std::string temp_string;
  std::string enum_val;

  input.ignore(12);
  input >> temp_string;

  // Sometimes two spaces between preamble and def
  if (temp_string.empty()) {
    input >> temp_string;
  }

  // Attribute name
  input >> name_;
  // Attribute type
  input >> temp_string;

  // Sometimes two spaces between type and values
  if (temp_string.empty()) {
    input >> temp_string;
  }

  input >> temp_string;
  min_ = std::stoi(temp_string);
  input >> temp_string;

  // Remove ending semicolon
  temp_string = temp_string.substr(0, temp_string.length() - 1);

  max_ = std::stoi(temp_string);
}

StringAttribute::StringAttribute(std::string && dbc_text)
{
  dbc_text_ = dbc_text;
  parse();
}

StringAttribute::StringAttribute(
  std::string && name,
  DbcObjType && dbc_obj_type)
    : default_value_(nullptr)
{
  name_ = std::move(name);
  dbc_obj_type_ = std::move(dbc_obj_type);
  attr_type_ = AttributeType::STRING;
  generateText();
}

StringAttribute::StringAttribute(const StringAttribute & other)
{
  dbc_text_ = other.dbc_text_;
  default_value_dbc_text_ = other.default_value_dbc_text_;
  name_ = other.name_;

  if (other.default_value_) {
    default_value_ = std::make_unique<std::string>(*(other.default_value_));
  } else {
    default_value_ = nullptr;
  }

  dbc_obj_type_ = other.dbc_obj_type_;
  attr_type_ = other.attr_type_;
}

StringAttribute & StringAttribute::operator=(const StringAttribute & other)
{
  return *this = StringAttribute(other);
}

const std::string * StringAttribute::getDefaultValue()
{
  return default_value_.get();
}

void StringAttribute::parseDefaultValue(std::string && dbc_text)
{
  default_value_dbc_text_ = dbc_text;

  std::istringstream input(default_value_dbc_text_);
  std::string temp_string;

  input.ignore(12);

  input >> temp_string;

  // Sometimes 2 spaces between preamble and def
  if (temp_string.empty()) {
    input >> temp_string;
  }

  // Attribute name
  input >> temp_string;
  // Default value
  input >> temp_string;

  setDefaultValue(std::move(temp_string));
}

void StringAttribute::setDefaultValue(std::string && default_value)
{
  default_value_ = std::make_unique<std::string>(std::move(default_value));
  generateDefaultValueText();
}

void StringAttribute::generateDefaultValueText()
{
  // TODO(jwhitleyastuff): DO THE THING!
}

void StringAttribute::generateText()
{
  std::ostringstream output;

  output << "BA_DEF_ ";

  switch (dbc_obj_type_) {
    case DbcObjType::BUS_NODES:
      output << "BU_ ";
      break;
    case DbcObjType::MESSAGE:
      output << "BO_ ";
      break;
    case DbcObjType::SIGNAL:
      output << "SG_ ";
      break;
  }

  output << "\"" << name_ << "\" STRING;";
  output << std::endl;

  dbc_text_ = output.str();

  generateDefaultValueText();
}

void StringAttribute::parse()
{
  std::istringstream input(dbc_text_);
  std::string temp_string;
  std::string enum_val;

  input.ignore(12);
  input >> temp_string;

  // Sometimes two spaces between preamble and def
  if (temp_string.empty()) {
    input >> temp_string;
  }

  // Attribute name
  input >> name_;
  // Attribute type
  input >> temp_string;
}

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS
