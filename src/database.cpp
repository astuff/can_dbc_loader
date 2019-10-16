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

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <sstream>

namespace AS
{
namespace CAN
{

// Begin DbcObj

const std::string DbcObj::getDbcText()
{
  return dbc_text_;
}

// End DbcObj

// Begin Attribute Definitions

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

// End Attribute Definitions

// Begin Comments

BusNodeComment::BusNodeComment(std::string && dbc_text)
{
  dbc_text_ = std::move(dbc_text);
  parse();
}

BusNodeComment::BusNodeComment(std::string && bus_node_name, std::string && comment)
  : node_name(bus_node_name),
    comment(comment)
{
  generateText();
}

void BusNodeComment::generateText()
{
  // TODO(jwhitleyastuff): Do the thing!
}

void BusNodeComment::parse()
{
  // TODO(jwhitleyastuff): Do the thing!
}

MessageComment::MessageComment(std::string && dbc_text)
  : msg_id(0)
{
  dbc_text_ = std::move(dbc_text);
  parse();
}

MessageComment::MessageComment(unsigned int msg_id, std::string && comment)
  : msg_id(msg_id),
    comment(comment)
{
  generateText();
}

void MessageComment::generateText()
{
  // TODO(jwhitleyastuff): Do the thing!
}

void MessageComment::parse()
{
  // TODO(jwhitleyastuff): Do the thing!
}

SignalComment::SignalComment(std::string && dbc_text)
  : msg_id(0),
    signal_name("")
{
  dbc_text_ = std::move(dbc_text);
  parse();
}

SignalComment::SignalComment(
  unsigned int msg_id,
  std::string && signal_name,
  std::string && comment)
  : msg_id(msg_id),
    signal_name(signal_name),
    comment(comment)
{
  generateText();
}

void SignalComment::generateText()
{
  // TODO(jwhitleyastuff): Do the thing!
}

void SignalComment::parse()
{
  // TODO(jwhitleyastuff): Do the thing!
}

// End Comments

// Begin AttributeObject

const std::unordered_map<std::string, std::string> AttributeObject::getAttributeValues()
{
  return attribute_values_;
}

const bool AttributeObject::hasAttributeValues()
{
  return (attribute_values_.size() < 1);
}

// End AttributeObject

// Begin BusNode

BusNode::BusNode(std::string && node_name)
  : name(node_name),
    comment_(nullptr)
{
}

const std::shared_ptr<BusNodeComment> BusNode::getComment()
{
  return std::shared_ptr<BusNodeComment>(comment_);
}

// End BusNode

// Begin Signal

Signal::Signal(std::string && dbc_text)
{
  dbc_text_ = std::move(dbc_text);
  parse();
}

Signal::Signal(
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
  std::map<int, std::string> && value_definitions)
  : name_(name),
    is_multiplexed_(is_multiplexed),
    multiplexer_identifier_(multiplexer_identifier),
    start_bit_(start_bit),
    length_(length),
    endianness_(endianness),
    is_signed_(is_signed),
    factor_(factor),
    offset_(offset),
    min_(min),
    max_(max),
    unit_(unit),
    receiving_nodes_(receiving_nodes),
    value_defs_(value_definitions)
{
  generateText();
}

const std::string Signal::getName()
{
  return name_;
}

const bool Signal::isMultiplexed()
{
  return is_multiplexed_;
}

const std::string Signal::getMultiplexerIdentifier()
{
  return multiplexer_identifier_;
}

const unsigned char Signal::getStartBit()
{
  return start_bit_;
}

const unsigned char Signal::getLength()
{
  return length_;
}

const Order Signal::getEndianness()
{
  return endianness_;
}

const bool Signal::isSigned()
{
  return is_signed_;
}

const float Signal::getFactor()
{
  return factor_;
}

const float Signal::getOffset()
{
  return offset_;
}

const float Signal::getMinVal()
{
  return min_;
}

const float Signal::getMaxVal()
{
  return max_;
}

const std::string Signal::getUnit()
{
  return unit_;
}

const std::vector<BusNode> Signal::getReceivingNodes()
{
  return receiving_nodes_;
}

const std::map<int, std::string> Signal::getValueDefinitions()
{
  return value_defs_;
}

const std::shared_ptr<SignalComment> Signal::getComment()
{
  return std::shared_ptr<SignalComment>(comment_);
}

void Signal::generateText()
{
  // TODO(jwhitleyastuff): Do the thing!
}

void Signal::parse()
{
  // TODO(jwhitleyastuff): Do the thing!
}

// End Signal

// Begin Message

Message::Message(std::string && message_text)
  : transmitting_node_(BusNode(""))
{
  dbc_text_ = std::move(message_text);
  parse();
}

Message::Message(
  unsigned int id,
  std::string && name,
  unsigned char dlc,
  BusNode && transmitting_node,
  std::vector<Signal> && signals)
  : id_(id),
    name_(name),
    dlc_(dlc),
    transmitting_node_(transmitting_node),
    signals_(signals)
{
  generateText();
}

const unsigned int Message::getId()
{
  return id_;
}

const std::string Message::getName()
{
  return name_;
}

const unsigned char Message::getDlc()
{
  return dlc_;
}

const unsigned char Message::getLength()
{
  return dlcToLength(dlc_);
}

const BusNode Message::getTransmittingNode()
{
  return transmitting_node_;
}

const std::vector<Signal> Message::getSignals()
{
  return signals_;
}

const std::shared_ptr<MessageComment> Message::getComment()
{
  return std::shared_ptr<MessageComment>(comment_);
}

void Message::generateText()
{
  // TODO(jwhitleyastuff): Do the thing!
}

void Message::parse()
{
  // TODO(jwhitleyastuff): Do the thing!
}

// End Message

// Begin Database

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

Database::Database(
  std::string && version,
  std::string && bus_config,
  std::vector<BusNode> && bus_nodes,
  std::unordered_map<unsigned int, Message> && messages,
  std::vector<std::shared_ptr<Attribute>> && attribute_definitions)
  : version_(version),
    bus_config_(bus_config),
    bus_nodes_(bus_nodes),
    messages_(messages),
    attribute_defs_(attribute_definitions)
{
}

void Database::generateDbcFile(const std::string & dbc_path)
{
  // TODO(jwhitleyastuff): Do the thing!
}

const std::string Database::getVersion()
{
  return version_;
}

const std::string Database::getBusConfig()
{
  return bus_config_;
}

const std::vector<BusNode> Database::getBusNodes()
{
  return std::vector<BusNode>(bus_nodes_);
}

const std::unordered_map<unsigned int, Message> Database::getMessages()
{
  return std::unordered_map<unsigned int, Message>(messages_);
}

const std::vector<std::shared_ptr<Attribute>> Database::getAttributeDefinitions()
{
  return std::vector<std::shared_ptr<Attribute>>(attribute_defs_);
}

void Database::parse()
{
  std::string line;
  bool version_found = false;
  bool bus_config_found = false;
  bool bus_nodes_found = false;
  std::unique_ptr<Message> current_msg(nullptr);
  std::vector<BusNodeComment> bus_node_comments;
  std::vector<MessageComment> message_comments;
  std::vector<SignalComment> signal_comments;

  while (getline(file_reader, line)) {
    if (!line.empty()) {
      auto preamble = line.substr(0, 4).c_str();

      if (!version_found && preamble == PREAMBLES[0]) {  // VERSION
        // Get everything after 'VERSION "', excluding last '"'
        version_ = line.substr(9, line.length() - 10);
        version_found = true;
      } else if (!bus_config_found && preamble == PREAMBLES[1]) {  // BUS_CONFIG
        // Get everything after 'BS_: '
        bus_config_ = line.substr(5, line.length() - 5);
        bus_config_found = true;
      } else if (!bus_nodes_found && preamble == PREAMBLES[2]) {  // BUS_NODES
        // Get everything after 'BU_: ' and split by spaces
        std::istringstream node_list(line.substr(5, line.length() - 5));
        std::vector<std::string> nodes(
          std::istream_iterator<std::string>{node_list},
          std::istream_iterator<std::string>());

        // Create new BusNodes and remove from 
        auto node_len = nodes.size();
        for (auto i = 0; i < node_len; ++i) {
          bus_nodes_.emplace_back(std::move(nodes.back()));
          nodes.pop_back();
        }

        bus_nodes_found = true;
      } else if (preamble == PREAMBLES[3]) {  // MESSAGE
        saveMsg(current_msg);

        // Create new message
        current_msg = std::unique_ptr<Message>(new Message(std::move(line)));
      } else if (preamble == PREAMBLES[4]) {  // SIGNAL
        if (current_msg) {
          // Signal found and current message is active
          // Add signal to existing message
          current_msg->signals_.emplace_back(std::move(line));
        }
      } else if (preamble == PREAMBLES[5]) {  // COMMENT
        saveMsg(current_msg);

        // Descriptions can only be added to their associated
        // database objects after the rest of the DBC has been parsed.
        // This is why they are stored in separate vectors.
        auto desc_type = line.substr(4, 4).c_str();
        auto desc_begin = line.find('"', 8);

        if (desc_begin != std::string::npos)
        {
          // Everything between the parentheses
          auto desc = line.substr(desc_begin, line.length() - desc_begin - 1);

          if (desc_type == PREAMBLES[2]) {  // BUS_NODE COMMENT
            bus_node_comments.emplace_back(std::move(line));
          } else if (desc_type == PREAMBLES[3]) {  // MESSAGE COMMENT
            message_comments.emplace_back(std::move(line));
          } else if (desc_type == PREAMBLES[4]) {  // SIGNAL COMMENT
            signal_comments.emplace_back(std::move(line));
          }
        } else {
          throw DbcParseException();
        }
      } else if (preamble == PREAMBLES[6]) {  // SIGNAL_VAL_DEF
        saveMsg(current_msg);
      } else if (preamble == PREAMBLES[7]) {  // ATTRIBUTE_DEF / ATTRIBUTE_DFLT_VAL
        saveMsg(current_msg);
      } else if (preamble == PREAMBLES[8]) {  // ATTRIBUTE_VAL
        saveMsg(current_msg);
      }
    }
  }

  // Just in case we still have a message open
  saveMsg(current_msg);
  
  // TODO(jwhitleyastuff): Apply comments to DB objects

  // TODO(jwhitleyastuff): Apply attributes to DB objects

  // TODO(jwhitleyastuff): Add value lists to signals
}

void Database::saveMsg(std::unique_ptr<Message> & msg_ptr)
{
  if (msg_ptr) {
    unsigned int id = msg_ptr->getId();
    messages_.emplace(id, std::move(*(msg_ptr.release())));
  }
}

// End Database

}  // namespace CAN
}  // namespace AS
