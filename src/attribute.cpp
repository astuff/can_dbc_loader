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

// Begin Attribute Base Class

std::string Attribute::getDefaultValueDbcText() const
{
  return default_value_dbc_text_;
}

std::string Attribute::getName() const
{
  return name_;
}

DbcObjType Attribute::getDbcObjType() const
{
  return dbc_obj_type_;
}

void Attribute::generateText()
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

  output << "\"" << name_ << "\" ";
  output << generateTypeSpecificText();
  output << ";" << std::endl;

  dbc_text_ = output.str();

  generateDefaultValueText();
}

void Attribute::parse()
{
  std::istringstream input(dbc_text_);
  std::string temp_string;

  input.ignore(8);
  input >> temp_string;

  // Sometimes two spaces between preamble and def
  if (temp_string.empty()) {
    input >> temp_string;
  }

  if (temp_string == "BO_") {
    dbc_obj_type_ = DbcObjType::MESSAGE;
  } else if (temp_string == "BU_") {
    dbc_obj_type_ = DbcObjType::BUS_NODES;
  } else if (temp_string == "EV_") {
    return;  // NOT SUPPORTED
  } else if (temp_string == "SG_") {
    dbc_obj_type_ = DbcObjType::SIGNAL;
  }

  // Attribute name
  input >> name_;
  // Attribute type
  input >> temp_string;

  parseTypeSpecificValues(input);
  parseDefaultValue();
}

// End Attribute Base Class
// Begin EnumAttribute

EnumAttribute::EnumAttribute(
  std::string && dbc_text,
  std::string && default_value_dbc_text)
{
  dbc_text_ = dbc_text;
  default_value_dbc_text_ = default_value_dbc_text;
  parse();
}

EnumAttribute::EnumAttribute(
  std::string && name,
  DbcObjType && dbc_obj_type,
  std::vector<std::string> && enum_values,
  std::string * default_value)
  : enum_values_(enum_values)
{
  name_ = name;
  dbc_obj_type_ = dbc_obj_type;
  enum_values_ = enum_values;

  if (default_value != nullptr) {
    default_value_ = std::make_unique<std::string>(*default_value);
  } else {
    default_value_ = nullptr;
  }

  generateText();
}

EnumAttribute::EnumAttribute(const EnumAttribute & other)
  : enum_values_(other.enum_values_)
{
  dbc_text_ = other.dbc_text_;
  default_value_dbc_text_ = other.default_value_dbc_text_;
  name_ = other.name_;
  dbc_obj_type_ = other.dbc_obj_type_;

  if (other.default_value_) {
    default_value_ = std::make_unique<std::string>(*(other.default_value_));
  } else {
    default_value_ = nullptr;
  }
}

EnumAttribute & EnumAttribute::operator=(const EnumAttribute & other)
{
  return *this = EnumAttribute(other);
}

std::vector<const std::string *> EnumAttribute::getEnumValues() const
{
  std::vector<const std::string *> enum_ptrs_;

  for (const auto & enum_val_ : enum_values_) {
    enum_ptrs_.push_back(&enum_val_);
  }

  return enum_ptrs_;
}

const std::string * EnumAttribute::getDefaultValue() const
{
  return default_value_.get();
}

void EnumAttribute::generateDefaultValueText()
{
  if (default_value_ != nullptr) {
    std::ostringstream output;

    output << "BA_DEF_DEF_ \"" << name_ << "\" ENUM \"";
    output << *default_value_ << "\";" << std::endl;

    default_value_dbc_text_ = output.str();
  }
}

std::string EnumAttribute::generateTypeSpecificText()
{
  std::ostringstream output;

  output << "ENUM ";

  for (const auto & val : enum_values_) {
    output << "\"" << val << "\"";
    
    if (val != enum_values_[enum_values_.size() - 1]) {
      output << ",";
    }
  }

  return output.str();
}

void EnumAttribute::parseTypeSpecificValues(std::istringstream & input)
{
  std::string temp_string;
  std::string enum_val;

  // Get values list
  input >> temp_string;

  // Sometimes two spaces between type and values
  if (temp_string.empty()) {
    input >> temp_string;
  }

  auto first_comma = temp_string.find(",");

  if (first_comma != std::string::npos) {
    std::istringstream temp_stream(temp_string);

    while (std::getline(temp_stream, enum_val, ',')) {
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

void EnumAttribute::parseDefaultValue()
{
  if (!default_value_dbc_text_.empty()) {
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

    default_value_ = std::make_unique<std::string>(std::move(temp_string));
  }
}

// End EnumAttribute
// Begin FloatAttribute

FloatAttribute::FloatAttribute(
  std::string && dbc_text,
  std::string && default_value_dbc_text)
{
  dbc_text_ = dbc_text;
  default_value_dbc_text_ = default_value_dbc_text;
  parse();
}

FloatAttribute::FloatAttribute(
  std::string && name,
  DbcObjType && dbc_obj_type,
  float min, float max,
  float * default_value)
    : min_(min), max_(max)
{
  name_ = std::move(name);
  dbc_obj_type_ = dbc_obj_type;

  if (default_value != nullptr) {
    default_value_ = std::make_unique<float>(*default_value);
  } else {
    default_value_ = nullptr;
  }

  generateText();
}

FloatAttribute::FloatAttribute(const FloatAttribute & other)
  : min_(other.min_), max_(other.max_)
{
  dbc_text_ = other.dbc_text_;
  default_value_dbc_text_ = other.default_value_dbc_text_;
  name_ = other.name_;
  dbc_obj_type_ = other.dbc_obj_type_;
  
  if (other.default_value_) {
    default_value_ = std::make_unique<float>(*(other.default_value_));
  } else {
    default_value_ = nullptr;
  }
}

FloatAttribute & FloatAttribute::operator=(const FloatAttribute & other)
{
  return *this = FloatAttribute(other);
}

float FloatAttribute::getMin() const
{
  return min_;
}

float FloatAttribute::getMax() const
{
  return max_;
}

const float * FloatAttribute::getDefaultValue() const
{
  return default_value_.get();
}

void FloatAttribute::generateDefaultValueText()
{
  if (default_value_ != nullptr) {
    std::ostringstream output;

    output << "BA_DEF_DEF_ \"" << name_ << "\" FLOAT ";
    output << *default_value_ << ";" << std::endl;

    default_value_dbc_text_ = output.str();
  }
}

std::string FloatAttribute::generateTypeSpecificText()
{
  std::ostringstream output;

  output << "FLOAT " << min_ << " " << max_;

  return output.str();
}

void FloatAttribute::parseTypeSpecificValues(std::istringstream & input)
{
  std::string temp_string;
  input >> temp_string;

  // Sometimes two spaces between type and values
  if (temp_string.empty()) {
    input >> temp_string;
  }

  min_ = std::stof(temp_string);
  input >> temp_string;

  // Remove ending semicolon
  temp_string = temp_string.substr(0, temp_string.length() - 1);

  max_ = std::stof(temp_string);
}

void FloatAttribute::parseDefaultValue()
{
  if (!default_value_dbc_text_.empty()) {
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

    default_value_ = std::make_unique<float>(std::stof(temp_string));
  }
}

// End FloatAttribute
// Begin IntAttribute

IntAttribute::IntAttribute(
  std::string && dbc_text,
  std::string && default_value_dbc_text)
{
  dbc_text_ = dbc_text;
  default_value_dbc_text_ = default_value_dbc_text;
  parse();
}

IntAttribute::IntAttribute(
  std::string && name,
  DbcObjType && dbc_obj_type,
  int min, int max,
  int * default_value)
    : min_(min), max_(max)
{
  name_ = name;
  dbc_obj_type_ = dbc_obj_type;

  if (default_value != nullptr) {
    default_value_ = std::make_unique<int>(*default_value);
  } else {
    default_value_ = nullptr;
  }

  generateText();
}

IntAttribute::IntAttribute(const IntAttribute & other)
  : min_(other.min_), max_(other.max_)
{
  dbc_text_ = other.dbc_text_;
  default_value_dbc_text_ = other.default_value_dbc_text_;
  name_ = other.name_;
  dbc_obj_type_ = other.dbc_obj_type_;
  
  if (other.default_value_) {
    default_value_ = std::make_unique<int>(*(other.default_value_));
  } else {
    default_value_ = nullptr;
  }
}

IntAttribute & IntAttribute::operator=(const IntAttribute & other)
{
  return *this = IntAttribute(other);
}

int IntAttribute::getMin() const
{
  return min_;
}

int IntAttribute::getMax() const
{
  return max_;
}

const int * IntAttribute::getDefaultValue() const
{
  return default_value_.get();
}

void IntAttribute::generateDefaultValueText()
{
  if (default_value_ != nullptr) {
    std::ostringstream output;

    output << "BA_DEF_DEF_ \"" << name_ << "\" INT ";
    output << *default_value_ << ";" << std::endl;

    default_value_dbc_text_ = output.str();
  }
}

std::string IntAttribute::generateTypeSpecificText()
{
  std::ostringstream output;

  output << "INT " << min_ << " " << max_;

  return output.str();
}

void IntAttribute::parseTypeSpecificValues(std::istringstream & input)
{
  std::string temp_string;
  input >> temp_string;

  // Sometimes two spaces between type and values
  if (temp_string.empty()) {
    input >> temp_string;
  }

  min_ = std::stoi(temp_string);
  input >> temp_string;

  // Remove ending semicolon
  temp_string = temp_string.substr(0, temp_string.length() - 1);

  max_ = std::stoi(temp_string);
}

void IntAttribute::parseDefaultValue()
{
  if (!default_value_dbc_text_.empty()) {
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

    default_value_ = std::make_unique<int>(std::stoi(temp_string));
  }
}

// End IntAttribute
// Begin StringAttribute

StringAttribute::StringAttribute(
  std::string && dbc_text,
  std::string && default_value_dbc_text)
{
  dbc_text_ = dbc_text;
  default_value_dbc_text_ = default_value_dbc_text;
  parse();
}

StringAttribute::StringAttribute(
  std::string && name,
  DbcObjType && dbc_obj_type,
  std::string * default_value)
{
  name_ = name;
  dbc_obj_type_ = dbc_obj_type;

  if (default_value != nullptr) {
    default_value_ = std::make_unique<std::string>(*default_value);
  } else {
    default_value_ = nullptr;
  }

  generateText();
}

StringAttribute::StringAttribute(const StringAttribute & other)
{
  dbc_text_ = other.dbc_text_;
  default_value_dbc_text_ = other.default_value_dbc_text_;
  name_ = other.name_;
  dbc_obj_type_ = other.dbc_obj_type_;

  if (other.default_value_) {
    default_value_ = std::make_unique<std::string>(*(other.default_value_));
  } else {
    default_value_ = nullptr;
  }
}

StringAttribute & StringAttribute::operator=(const StringAttribute & other)
{
  return *this = StringAttribute(other);
}

const std::string * StringAttribute::getDefaultValue() const
{
  return default_value_.get();
}

void StringAttribute::generateDefaultValueText()
{
  if (default_value_ != nullptr) {
    std::ostringstream output;

    output << "BA_DEF_DEF_ \"" << name_ << "\" \"";
    output << *default_value_ << "\";" << std::endl;

    default_value_dbc_text_ = output.str();
  }
}

std::string StringAttribute::generateTypeSpecificText()
{
  return std::string("");
}

void StringAttribute::parseTypeSpecificValues(std::istringstream & input)
{
  (void)input;
  return;
}

void StringAttribute::parseDefaultValue()
{
  if (!default_value_dbc_text_.empty()) {
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

    default_value_ = std::make_unique<std::string>(std::move(temp_string));
  }
}

// End StringAttribute

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS
