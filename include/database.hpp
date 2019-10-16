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
  "VERS",  // VERSION
  "BS_:",  // BUS_CONFIG
  "BU_:",  // BUS_NODES
  "BO_ ",  // MESSAGE
  " SG_",  // SIGNAL
  "CM_ ",  // DESCRIPTION
  "VAL_",  // SIGNAL_VAL_DEF
  "BA_D",  // ATTRIBUTE_DEF / ATTRIBUTE_DFLT_VAL
  "BA_ "   // ATTRIBUTE_VAL
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

class DbcObj
{
public:
  virtual ~DbcObj() {};
  const std::string getDbcText();

protected:
  std::string dbc_text_;

private:
  virtual void generateText() = 0;
  virtual void parse() = 0;
};

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

class BusNodeComment
  : public DbcObj
{
public:
  BusNodeComment(std::string && dbc_text);
  BusNodeComment(
    std::string && bus_node_name,
    std::string && comment);

  const std::string node_name;
  const std::string comment;

private:
  void generateText() override;
  void parse() override;
};

class MessageComment
  : public DbcObj
{
public:
  MessageComment(std::string && dbc_text);
  MessageComment(unsigned int msg_id, std::string && comment);

  const unsigned int msg_id;
  const std::string comment;

private:
  void generateText() override;
  void parse() override;
};

class SignalComment
  : public DbcObj
{
public:
  SignalComment(std::string && dbc_text);
  SignalComment(
    unsigned int msg_id,
    std::string && signal_name,
    std::string && comment);

  const unsigned int msg_id;
  const std::string signal_name;
  const std::string comment;

private:
  void generateText() override;
  void parse() override;
};

class AttributeObject
{
public:
  const std::unordered_map<std::string, std::string> getAttributeValues();
  const bool hasAttributeValues();

protected:
  std::unordered_map<std::string, std::string> attribute_values_;
};

class BusNode
  : public AttributeObject
{
public:
  BusNode(std::string && node_name);

  const std::shared_ptr<BusNodeComment> getComment();

  const std::string name;

private:
  std::shared_ptr<BusNodeComment> comment_;
};

class Signal
  : public DbcObj, public AttributeObject
{
public:
  Signal(std::string && dbc_text);
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
  const std::shared_ptr<SignalComment> getComment();

  friend class Message;
  friend class Database;

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
  std::shared_ptr<SignalComment> comment_;
  
  void generateText() override;
  void parse() override;
};

class Message
  : public DbcObj, public AttributeObject
{
public:
  Message(std::string && dbc_text);
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
  const std::shared_ptr<MessageComment> getComment();

  friend class Database;

private:
  unsigned int id_;
  std::string name_;
  unsigned char dlc_;
  BusNode transmitting_node_;
  std::vector<Signal> signals_;
  std::shared_ptr<MessageComment> comment_;

  void generateText() override;
  void parse() override;
};

class Database
{
public:
  Database(const std::string & dbc_path);
  Database(
    std::string && version,
    std::string && bus_config,
    std::vector<BusNode> && bus_nodes,
    std::unordered_map<unsigned int, Message> && messages,
    std::vector<std::shared_ptr<Attribute>> && attribute_definitions);

  void generateDbcFile(const std::string & dbc_path);
  const std::string getVersion();
  const std::string getBusConfig();
  const std::vector<BusNode> getBusNodes();
  const std::unordered_map<unsigned int, Message> getMessages();
  const std::vector<std::shared_ptr<Attribute>> getAttributeDefinitions();

private:
  std::ifstream file_reader;
  std::string version_;
  std::string bus_config_;
  std::vector<BusNode> bus_nodes_;
  std::unordered_map<unsigned int, Message> messages_;
  std::vector<std::shared_ptr<Attribute>> attribute_defs_;

  void parse();
  void saveMsg(std::unique_ptr<Message> & msg_ptr);
};

}  // namespace CAN
}  // namespace AS

#endif
