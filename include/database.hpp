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

#ifndef DATABASE_HPP_
#define DATABASE_HPP_

#include <array>
#include <exception>
#include <fstream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace AS
{
namespace CAN
{

static constexpr std::array<unsigned char, 16> DLC_LENGTH =
{ 0,  1,  2,  3,  4,  5,  6,  7,
  8, 12, 16, 20, 24, 32, 48, 64 };

unsigned char dlcToLength(const unsigned char & dlc)
{
  return DLC_LENGTH[dlc];
}

static constexpr std::array<const char[5], 9> PREAMBLES =
{
  "VERS",
  "BS_:",
  "BU_:",
  "BO_ ",
  " SG_",
  "CM_ ",
  "VAL_",
  "BA_D",
  "BA_ "
};

enum class AttributeDefType
{
  INT,
  FLOAT,
  STRING,
  ENUM
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
    return "Exception when reading DBC file.";
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

class AttributeDef
{
public:
  AttributeDef(std::string && name);
  virtual ~AttributeDef() {};

  virtual AttributeDefType getType() = 0;

  const std::string name;
};

class EnumAttributeDef
  : public AttributeDef
{
public:
  EnumAttributeDef(std::string && name, std::vector<std::string> && enum_values);
  ~EnumAttributeDef() = default;
  AttributeDefType getType() override;
  const std::vector<std::string> enum_values;
};

class FloatAttributeDef
  : public AttributeDef
{
public:
  FloatAttributeDef(std::string && name, float min, float max);
  ~FloatAttributeDef() = default;
  AttributeDefType getType() override;
  const float min, max;
};

class IntAttributeDef
  : public AttributeDef
{
public:
  IntAttributeDef(std::string && name, int min, int max);
  ~IntAttributeDef() = default;
  AttributeDefType getType() override;
  const int min, max;
};

class StringAttributeDef
  : public AttributeDef
{
public:
  StringAttributeDef(std::string && name);
  ~StringAttributeDef() = default;
  AttributeDefType getType() override;
};

// TODO(jwhitleyastuff): Figure out how to add these later.
class DbcObject
{
public:
  DbcObject(
    std::string && description = std::string(),
    std::unordered_map<std::string, std::string> && attribute_values =
      std::unordered_map<std::string, std::string>());

  const std::string getDescription();
  const bool hasDescription();
  const std::unordered_map<std::string, std::string> getAttributeValues();
  const bool hasAttributeValues();

private:
  std::string description_;
  std::unordered_map<std::string, std::string> attribute_values_;
};

class BusNode
  : public DbcObject
{
public:
  BusNode(std::string && node_name);

  const std::string name;
};

class Signal
  : public DbcObject
{
public:
  Signal(std::string && signal_text);
  Signal(
    std::string && name,
    bool is_multiplexed,
    std::string && multiplexer_identifier,
    unsigned char start_bit,
    unsigned char length,
    Order endianness,
    bool is_signed,
    float factor,
    float offset,
    float min,
    float max,
    std::string && unit,
    std::vector<BusNode> && receiving_nodes,
    std::map<int, std::string> && value_definitions =
      std::map<int, std::string>());

  const std::string getName();
  const bool isMultiplexed();
  const std::string getMultiplexerIdentifier();
  const unsigned char getStartBit();
  const unsigned char getLength();
  const Order getEndianness();
  const bool isSigned();
  const float getFactor();
  const float getOffset();
  const float getMinVal();
  const float getMaxVal();
  const std::string getUnit();
  const std::vector<BusNode> getReceivingNodes();
  const std::map<int, std::string> getValueDefinitions();

  const std::string text;

private:
  std::string name_;
  bool is_multiplexed_;
  std::string multiplexer_identifier_;
  unsigned char start_bit_;
  unsigned char length_;
  Order endianness_;
  bool is_signed_;
  float factor_;
  float offset_;
  float min_;
  float max_;
  std::string unit_;
  std::vector<BusNode> receiving_nodes_;
  std::map<int, std::string> value_defs_;
  
  void generateText();
  void parse();
};

class Message
  : public DbcObject
{
public:
  Message(std::string && message_text);
  Message(
    unsigned int id,
    std::string && name,
    unsigned char dlc,
    BusNode && transmitting_node,
    std::vector<Signal> && signals);

  const unsigned int getId();
  const std::string getName();
  const unsigned char getDlc();
  const unsigned char getLength();
  const BusNode getTransmittingNode();
  const std::vector<Signal> getSignals();

  const std::string text;

private:
  unsigned int id_;
  std::string name_;
  unsigned char dlc_;
  BusNode transmitting_node_;
  std::vector<Signal> signals_;

  void generateText();
  void parse();
};

class Database
{
public:
  Database(const std::string & dbc_path);
  Database(
    std::string && version,
    std::string && bus_config,
    std::vector<BusNode> && bus_nodes,
    std::vector<Message> && messages,
    std::vector<std::shared_ptr<AttributeDef>> && attribute_definitions,
    std::unordered_map<std::shared_ptr<AttributeDef>, std::string> && attribute_default_vlaues);

  const std::string getVersion();
  const std::string getBusConfig();
  const std::vector<BusNode> getBusNodes();
  const std::vector<Message> getMessages();
  const std::vector<AttributeDef> getAttributeDefinitions();
  const std::unordered_map<std::shared_ptr<AttributeDef>, std::string> getAttributeDefaultValues();

private:
  std::ifstream file_reader;
  std::string version_;
  std::string bus_config_;
  std::vector<BusNode> bus_nodes_;
  std::vector<Message> messages_;
  std::vector<std::shared_ptr<AttributeDef>> attribute_defs_;
  std::unordered_map<std::shared_ptr<AttributeDef>, std::string> attribute_default_values_;

  void generateText();
  void parse();
};

}  // namespace CAN
}  // namespace AS

#endif
