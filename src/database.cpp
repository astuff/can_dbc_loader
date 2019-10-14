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

#include "database.hpp"

#include <fstream>
#include <iostream>
#include <string>

namespace AS
{
namespace CAN
{

// Attribute Definitions

AttributeDef::AttributeDef(std::string && name)
  : name(std::move(name))
{
}

EnumAttributeDef::EnumAttributeDef(std::string && name, std::vector<std::string> && enum_values)
  : AttributeDef(std::move(name)),
    enum_values(std::move(enum_values))
{
}

AttributeDefType EnumAttributeDef::getType()
{
  return AttributeDefType::ENUM;
}

FloatAttributeDef::FloatAttributeDef(std::string && name, float min, float max)
  : AttributeDef(std::move(name)),
    min(min),
    max(max)
{
}

AttributeDefType FloatAttributeDef::getType()
{
  return AttributeDefType::FLOAT;
}

IntAttributeDef::IntAttributeDef(std::string && name, int min, int max)
  : AttributeDef(std::move(name)),
    min(min),
    max(max)
{
}

AttributeDefType IntAttributeDef::getType()
{
  return AttributeDefType::INT;
}

StringAttributeDef::StringAttributeDef(std::string && name)
  : AttributeDef(std::move(name))
{
}

AttributeDefType StringAttributeDef::getType()
{
  return AttributeDefType::STRING;
}

// End Attribute Definitions

// DbcObject

DbcObject::DbcObject(
  std::string && description,
  std::unordered_map<std::string, std::string> && attribute_values)
  : description_(std::move(description)),
    attribute_values_(std::move(attribute_values))
{
}

const std::string DbcObject::getDescription()
{
  return description_;
}

const bool DbcObject::hasDescription()
{
  return (!description_.empty());
}

const std::unordered_map<std::string, std::string> DbcObject::getAttributeValues()
{
  return attribute_values_;
}

const bool DbcObject::hasAttributeValues()
{
  return (attribute_values_.size() < 1);
}

// End DbcObject

// BusNode

BusNode::BusNode(std::string && node_name)
  : name(std::move(node_name))
{
}

// End BusNode

// Signal

Database::Database(const std::string & dbc_path)
  : file_reader(dbc_path)
{
  file_reader.open(dbc_path);

  if (file_reader.is_open()) {
    parse();
  } else {
    throw DbcReadException();
  }

  file_reader.close();
}

void Database::parse()
{
  std::string line;

  while (getline(file_reader, line)) {
  }
}

}  // namespace CAN
}  // namespace AS
