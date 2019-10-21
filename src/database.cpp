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
#include <iterator>
#include <memory>
#include <string>
#include <sstream>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

Database::Database(const std::string & dbc_path)
{
  std::ifstream file_reader;
  file_reader.open(dbc_path);

  if (file_reader.is_open()) {
    parse(file_reader);
  } else {
    throw DbcReadException();
  }

  file_reader.close();
}

Database::Database(std::istream & mem_stream)
{
  parse(mem_stream);
}

Database::Database(
  std::string && version,
  std::string && bus_config,
  std::vector<BusNode> && bus_nodes,
  std::unordered_map<unsigned int, Message> && messages,
  std::vector<Attribute *> && attribute_definitions)
  : version_(version),
    bus_config_(bus_config),
    bus_nodes_(bus_nodes),
    messages_(messages)
{
  for (auto & attr : attribute_definitions) {
    auto attr_type = attr->getAttrType();

    if (attr_type == AttributeType::ENUM) {
      attribute_defs_.emplace_back(std::move(dynamic_cast<EnumAttribute *>(attr)));
    } else if (attr_type == AttributeType::FLOAT) {
      attribute_defs_.emplace_back(std::move(dynamic_cast<FloatAttribute *>(attr)));
    } else if (attr_type == AttributeType::INT) {
      attribute_defs_.emplace_back(std::move(dynamic_cast<IntAttribute *>(attr)));
    } else if (attr_type == AttributeType::STRING) {
      attribute_defs_.emplace_back(std::move(dynamic_cast<StringAttribute *>(attr)));
    }
  }
}

void Database::generateDbcFile(const std::string & dbc_path)
{
  // TODO(jwhitleyastuff): Do the thing!
}

std::string Database::getVersion()
{
  return version_;
}

std::string Database::getBusConfig()
{
  return bus_config_;
}

std::vector<BusNode> Database::getBusNodes()
{
  return std::vector<BusNode>(bus_nodes_);
}

std::unordered_map<unsigned int, Message> Database::getMessages()
{
  return std::unordered_map<unsigned int, Message>(messages_);
}

const std::vector<const Attribute *> Database::getAttributeDefinitions()
{
  std::vector<const Attribute *> temp_attr_defs;

  for (auto & attr : attribute_defs_) {
    temp_attr_defs.push_back(attr.get());
  }

  return temp_attr_defs;
}

void Database::parse(std::istream & reader)
{
  std::string line;
  bool version_found = false;
  bool bus_config_found = false;
  bool bus_nodes_found = false;
  std::unique_ptr<Message> current_msg(nullptr);
  std::vector<BusNodeComment> bus_node_comments;
  std::vector<MessageComment> message_comments;
  std::vector<SignalComment> signal_comments;
  std::unordered_map<std::string, std::string> attr_texts;
  std::unordered_map<std::string, std::string> attr_def_val_texts;

  while (std::getline(reader, line)) {
    if (!line.empty()) {
      std::istringstream iss_line(line);
      std::string preamble;

      iss_line >> preamble;

      // Some lines begin with a space
      if (preamble.empty()) {
        iss_line >> preamble;
      }

      if (!version_found && preamble == PREAMBLES[0]) {  // VERSION
        iss_line >> version_;

        // Remove parentheses
        version_ = version_.substr(1, version_.length() - 2);
        version_found = true;
      } else if (!bus_config_found && preamble == PREAMBLES[1]) {  // BUS CONFIG
        iss_line >> bus_config_;
        bus_config_found = true;
      } else if (!bus_nodes_found && preamble == PREAMBLES[2]) {  // BUS NODES
        std::string node;

        // Get everything after 'BU_: ' and split by spaces
        while (iss_line >> node) {
          bus_nodes_.emplace_back(std::move(node));
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
          Signal temp_sig(std::move(line));
          current_msg->signals_.emplace(std::make_pair(temp_sig.getName(), std::move(temp_sig)));
        } else {
          throw DbcParseException();
        }
      } else if (preamble == PREAMBLES[5]) {  // COMMENT
        saveMsg(current_msg);

        // Comments can only be added to their associated
        // database objects after the rest of the DBC has been parsed.
        // This is why they are stored in separate vectors here.
        std::string desc_type;
        iss_line >> desc_type;

        if (desc_type == "BU_") {  // BUS NODE COMMENT
          bus_node_comments.emplace_back(std::move(line));
        } else if (desc_type == PREAMBLES[3]) {  // MESSAGE COMMENT
          message_comments.emplace_back(std::move(line));
        } else if (desc_type == PREAMBLES[4]) {  // SIGNAL COMMENT
          signal_comments.emplace_back(std::move(line));
        }
      } else if (preamble == PREAMBLES[6]) {  // SIGNAL VALUE LIST
        saveMsg(current_msg);
      } else if (preamble == PREAMBLES[7]) {  // ATTRIBUTE DEFINITION
        saveMsg(current_msg);

        std::string attr_name;
        iss_line.ignore(4);
        iss_line >> attr_name;

        // Sometimes two spaces between preamble and name
        if (attr_name.empty()) {
          iss_line >> attr_name;
        }

        attr_texts[attr_name] = std::move(line);
      } else if (preamble == PREAMBLES[8]) {  // ATTRIBUTE DEFAULT VALUE
        saveMsg(current_msg);

        std::string attr_name;
        iss_line >> attr_name;

        // Sometimes two spaces between preamble and name
        if (attr_name.empty()) {
          iss_line >> attr_name;
        }

        attr_def_val_texts[attr_name] = std::move(line);
      } else if (preamble == PREAMBLES[9]) {  // ATTRIBUTE VALUE
        saveMsg(current_msg);
      }
    }
  }

  // Just in case we still have a message open
  saveMsg(current_msg);
  
  // Add bus node comments
  for (auto & bus_node_comment : bus_node_comments) {
    for (auto & bus_node : bus_nodes_) {
      if (bus_node.name_ == bus_node_comment.getNodeName()) {
        bus_node.comment_ = std::make_unique<std::string>(std::move(bus_node_comment.comment_));
      }
    }
  }

  // Add message comments
  for (auto & message_comment : message_comments) {
    auto msg_itr = messages_.find(message_comment.getMsgId());

    if (msg_itr != messages_.end()) {
      msg_itr->second.comment_ = std::make_unique<std::string>(std::move(message_comment.comment_));
    }
  }

  // Add signal comments
  for (auto & signal_comment : signal_comments) {
    auto msg_itr = messages_.find(signal_comment.getMsgId());

    if (msg_itr != messages_.end()) {
      auto signal_itr = msg_itr->second.signals_.find(signal_comment.getSignalName());

      if (signal_itr != msg_itr->second.signals_.end()) {
        signal_itr->second.comment_ = std::make_unique<std::string>(std::move(signal_comment.comment_));
      }
    }
  }

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

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS
