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

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS
