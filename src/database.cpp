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
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

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

std::string Database::getVersion() const
{
  return version_;
}

std::string Database::getBusConfig() const
{
  return bus_config_;
}

std::vector<const BusNode *> Database::getBusNodes() const
{
  std::vector<const BusNode *> nodes;

  for (auto & node : bus_nodes_) {
    nodes.emplace_back(&node);
  }

  return nodes;
}

std::unordered_map<unsigned int, const Message *> Database::getMessages() const
{
  std::unordered_map<unsigned int, const Message *> msgs;

  for (auto & msg : messages_) {
    msgs[msg.first] = &(msg.second);
  }

  return msgs;
}

std::vector<const Attribute *> Database::getAttributeDefinitions() const
{
  std::vector<const Attribute *> temp_attr_defs;

  for (auto & attr : attribute_defs_) {
    auto attr_type = attr->getAttrType();

    switch (attr_type) {
      case AttributeType::ENUM:
      {
        auto enum_ptr = dynamic_cast<const EnumAttribute *>(attr.get());
        temp_attr_defs.emplace_back(std::move(enum_ptr));
      } break;
      case AttributeType::FLOAT:
      {
        auto float_ptr = dynamic_cast<const FloatAttribute *>(attr.get());
        temp_attr_defs.emplace_back(std::move(float_ptr));
      } break;
      case AttributeType::INT:
      {
        auto int_ptr = dynamic_cast<const IntAttribute *>(attr.get());
        temp_attr_defs.emplace_back(std::move(int_ptr));
      } break;
      case AttributeType::STRING:
      {
        auto str_ptr = dynamic_cast<const StringAttribute *>(attr.get());
        temp_attr_defs.emplace_back(std::move(str_ptr));
      } break;
    }
  }

  return temp_attr_defs;
}

void Database::writeDbcToFile(const std::string & dbc_path) const
{
  std::ofstream file_writer;
  file_writer.open(dbc_path);

  if (file_writer.is_open()) {
    generate(file_writer);
  } else {
    throw DbcWriteException();
  }

  file_writer.close();
}

void Database::writeDbcToStream(std::ostream & mem_stream) const
{
  generate(mem_stream);
}

std::unordered_map<unsigned int, MessageTranscoder> Database::getTranscoders()
{
  std::unordered_map<unsigned int, MessageTranscoder> xcoders;

  for (auto msg = messages_.begin(); msg != messages_.end(); ++msg) {
    xcoders.emplace(msg->first, &(msg->second));
  }

  return xcoders;
}

void Database::generate(std::ostream & output) const
{
  std::vector<BusNodeComment> bus_node_comments;
  std::vector<MessageComment> message_comments;
  std::vector<SignalComment> signal_comments;

  output << "VERSION \"" << version_ << "\"\n\n\n";
  output << "NS_ :\n";
  output << "\tNS_DESC_\n";
  output << "\tCM_\n";
  output << "\tBA_DEF_\n";
  output << "\tBA_\n";
  output << "\tVAL_\n";
  output << "\tCAT_DEF_\n";
  output << "\tCAT_\n";
  output << "\tFILTER\n";
  output << "\tBA_DEF_DEF_\n";
  output << "\tEV_DATA_\n";
  output << "\tENVVAR_DATA_\n";
  output << "\tSGTYPE_\n";
  output << "\tSGTYPE_VAL_\n";
  output << "\tBA_DEF_SGTYPE_\n";
  output << "\tBA_SGTYPE_\n";
  output << "\tSIG_TYPE_REF_\n";
  output << "\tVAL_TABLE_\n";
  output << "\tSIG_GROUP_\n";
  output << "\tSIG_VALTYPE_\n";
  output << "\tSIGTYPE_VALTYPE_\n";
  output << "\tBO_TX_BU_\n";
  output << "\tBA_DEF_REL_\n";
  output << "\tBA_REL_\n";
  output << "\tBA_DEF_DEF_REL_\n";
  output << "\tBU_SG_REL_\n";
  output << "\tBU_EV_REL_\n";
  output << "\tBU_BO_REL_\n";
  output << "\tSG_MUL_VAL_\n" << std::endl;
  output << "BS_: " << bus_config_ << "\n\n";
  output << "BU_: ";

  for (auto i = 0; i < bus_nodes_.size(); ++i) {
    output << bus_nodes_[i].name_;

    if (bus_nodes_[i].comment_ != nullptr) {
      std::string comment = *(bus_nodes_[i].getComment());
      bus_node_comments.emplace_back(
        bus_nodes_[i].getName(),
        std::move(comment));
    }

    if (i != bus_nodes_.size() - 1) {
      output << ",";
    }
  }

  output << "\n\n" << std::endl;

  for (auto & msg : messages_) {
    output << msg.second.dbc_text_;

    if (msg.second.comment_ != nullptr) {
      std::string comment = *(msg.second.getComment());
      message_comments.emplace_back(
        msg.second.getId(),
        std::move(comment));
    }

    for (auto & sig : msg.second.signals_) {
      if (sig.second.comment_ != nullptr) {
        std::string comment = *(sig.second.getComment());
        signal_comments.emplace_back(
          msg.second.getId(),
          sig.second.getName(),
          std::move(comment));
      }

      output << sig.second.dbc_text_;
    }

    output << std::endl;
  }

  for (auto & comment : bus_node_comments) {
    output << comment.dbc_text_;
  }

  for (auto & comment : message_comments) {
    output << comment.dbc_text_;
  }

  for (auto & comment : signal_comments) {
    output << comment.dbc_text_;
  }

  // TODO(jwhitleyastuff): Write out attribute defs
  // TODO(jwhitleyastuff): Write out attribute default values
  // TODO(jwhitleyastuff): Write out attribute values
  // TODO(jwhitleyastuff): Write out signal value lists
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
  std::unordered_map<std::string, std::pair<AttributeType, std::string>> attr_texts;
  std::unordered_map<std::string, std::string> attr_def_val_texts;

  while (std::getline(reader, line)) {
    // Ignore empty lines and lines starting with tab
    if (!line.empty() && line.rfind("\t") != 0) {
      std::istringstream iss_line(line);
      std::string preamble;

      iss_line >> preamble;

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

        std::map<unsigned int, std::string> value_descs;

        std::string temp_string;
        std::string sig_name;

        // Message ID
        iss_line >> temp_string;

        unsigned int id = std::stoul(temp_string);

        // Signal Name
        iss_line >> sig_name;

        // Get all remaining values up to the ending semicolon
        std::getline(iss_line, temp_string, ';');

        // TODO(jwhitleyastuff): Finish parsing values
      } else if (preamble == PREAMBLES[7]) {  // ATTRIBUTE DEFINITION
        saveMsg(current_msg);

        std::string attr_name, temp_type;
        AttributeType attr_type;
        iss_line.ignore(4);
        iss_line >> attr_name;

        // Sometimes two spaces between preamble and name
        if (attr_name.empty()) {
          iss_line >> attr_name;
        }

        iss_line >> temp_type;

        if (temp_type == "ENUM") {
          attr_type = AttributeType::ENUM;
        } else if (temp_type == "FLOAT") {
          attr_type = AttributeType::FLOAT;
        } else if (temp_type == "HEX" || temp_type == "INT") {
          attr_type = AttributeType::INT;
        } else if (temp_type.rfind("STRING", 0) == 0) {
          attr_type = AttributeType::STRING;
        }

        attr_texts[attr_name] = std::make_pair(attr_type, std::move(line));
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

  // Add attribute definitions
  for (auto & attr : attr_texts) {
    auto found_def_val = attr_def_val_texts.find(attr.first);

    std::string dbc_text = std::move(attr.second.second);
    std::string def_val_dbc_text = "";

    if (found_def_val != attr_def_val_texts.end()) {
      def_val_dbc_text = std::move(found_def_val->second);
    }

    switch (attr.second.first) {
      case AttributeType::ENUM:
      {
        EnumAttribute enum_attr(std::move(dbc_text), std::move(def_val_dbc_text));
        std::unique_ptr<Attribute> temp_attr = std::make_unique<EnumAttribute>(enum_attr);
        attribute_defs_.push_back(std::move(temp_attr));
      } break;
      case AttributeType::FLOAT:
      {
        FloatAttribute float_attr(std::move(dbc_text), std::move(def_val_dbc_text));
        std::unique_ptr<Attribute> temp_attr = std::make_unique<FloatAttribute>(float_attr);
        attribute_defs_.push_back(std::move(temp_attr));
      } break;
      case AttributeType::INT:
      {
        IntAttribute int_attr(std::move(dbc_text), std::move(def_val_dbc_text));
        std::unique_ptr<Attribute> temp_attr = std::make_unique<IntAttribute>(int_attr);
        attribute_defs_.push_back(std::move(temp_attr));
      } break;
      case AttributeType::STRING:
      {
        StringAttribute str_attr(std::move(dbc_text), std::move(def_val_dbc_text));
        std::unique_ptr<Attribute> temp_attr = std::make_unique<StringAttribute>(str_attr);
        attribute_defs_.push_back(std::move(temp_attr));
      } break;
    }
  }

  // TODO(jwhitleyastuff): Apply attributes to DB objects

  // TODO(jwhitleyastuff): Add signal value description lists to signals
}

void Database::saveMsg(std::unique_ptr<Message> & msg_ptr)
{
  if (msg_ptr) {
    unsigned int id = msg_ptr->getId();

    // Some diagnostic messages are created by Vector tools
    // with CAN IDs > 29 bits. Don't add them.
    if (id <= MAX_CAN_ID) {
      messages_.emplace(id, std::move(*(msg_ptr.release())));
    }
  }
}

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS
