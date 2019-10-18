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
namespace DbcLoader
{

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

  while (std::getline(file_reader, line)) {
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
        std::string node;

        // Get everything after 'BU_: ' and split by spaces
        std::istringstream node_list(line.substr(5, line.length() - 5));

        while (node_list >> node) {
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
        auto desc_type = line.substr(4, 4).c_str();

        if (desc_type == PREAMBLES[2]) {  // BUS_NODE COMMENT
          bus_node_comments.emplace_back(std::move(line));
        } else if (desc_type == PREAMBLES[3]) {  // MESSAGE COMMENT
          message_comments.emplace_back(std::move(line));
        } else if (desc_type == PREAMBLES[4]) {  // SIGNAL COMMENT
          signal_comments.emplace_back(std::move(line));
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
  
  // Add bus node comments
  for (auto & bus_node_comment : bus_node_comments) {
    for (auto & bus_node : bus_nodes_) {
      if (bus_node.name_ == bus_node_comment.getNodeName()) {
        bus_node.comment_ = std::shared_ptr<BusNodeComment>(new BusNodeComment(std::move(bus_node_comment)));
      }
    }
  }

  // Add message comments
  for (auto & message_comment : message_comments) {
    auto msg_itr = messages_.find(message_comment.getMsgId());

    if (msg_itr != messages_.end()) {
      msg_itr->second.comment_ = std::shared_ptr<MessageComment>(new MessageComment(std::move(message_comment)));
    }
  }

  // Add signal comments
  for (auto & signal_comment : signal_comments) {
    auto msg_itr = messages_.find(signal_comment.getMsgId());

    if (msg_itr != messages_.end()) {
      auto signal_itr = msg_itr->second.signals_.find(signal_comment.getSignalName());

      if (signal_itr != msg_itr->second.signals_.end()) {
        signal_itr->second.comment_ = std::shared_ptr<SignalComment>(new SignalComment(std::move(signal_comment)));
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

// End Database

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS
