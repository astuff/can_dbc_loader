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

// Begin Attribute Definitions

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

// Begin DbcObject

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

// Begin BusNode

BusNode::BusNode(std::string && node_name)
  : name(std::move(node_name))
{
}

// End BusNode

// Begin Signal

Signal::Signal(std::string && signal_text)
  : text(std::move(signal_text))
{
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
  : name_(std::move(name)),
    is_multiplexed_(is_multiplexed),
    multiplexer_identifier_(std::move(multiplexer_identifier)),
    start_bit_(start_bit),
    length_(length),
    endianness_(endianness),
    is_signed_(is_signed),
    factor_(factor),
    offset_(offset),
    min_(min),
    max_(max),
    unit_(std::move(unit)),
    receiving_nodes_(std::move(receiving_nodes)),
    value_defs_(std::move(value_definitions))
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

void Signal::generateText()
{
  // TODO(jwhitleyastuff): Do the thing!
}

void Signal::parse()
{
}

// End Signal

// Begin Message

Message::Message(std::string && message_text)
  : text(std::move(message_text)),
    transmitting_node_(BusNode(""))
{
  parse();
}

Message::Message(
  unsigned int id,
  std::string && name,
  unsigned char dlc,
  BusNode && transmitting_node,
  std::vector<Signal> && signals)
  : id_(id),
    name_(std::move(name)),
    dlc_(dlc),
    transmitting_node_(std::move(transmitting_node)),
    signals_(std::move(signals))
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

void Database::parse()
{
  std::string line;
  bool version_found = false;
  bool bus_config_found = false;
  bool bus_nodes_found = false;
  std::unique_ptr<Message> current_msg(nullptr);

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
      } else if (preamble == PREAMBLES[5]) {  // DESCRIPTION
        saveMsg(current_msg);
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
}

void Database::saveMsg(std::unique_ptr<Message> & msg_ptr)
{
  if (msg_ptr) {
    messages_.push_back(std::move(*(msg_ptr.release())));
  }
}

// End Database

}  // namespace CAN
}  // namespace AS
