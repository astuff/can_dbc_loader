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

#include <iostream>

#include <common_defs.hpp>
#include <database.hpp>

static std::string dbc_file = "example.dbc";

using AS::CAN::DbcLoader::AttributeType;
using AS::CAN::DbcLoader::Database;
using AS::CAN::DbcLoader::DbcObjType;
using AS::CAN::DbcLoader::EnumAttribute;
using AS::CAN::DbcLoader::FloatAttribute;
using AS::CAN::DbcLoader::IntAttribute;
using AS::CAN::DbcLoader::StringAttribute;

int main(int argc, char ** argv)
{
  Database dbc(dbc_file);

  size_t signal_counter = 0;
  size_t bus_node_comment_counter = 0;
  size_t message_comment_counter = 0;
  size_t signal_comment_counter = 0;
  size_t bus_node_attr_counter = 0;
  size_t message_attr_counter = 0;
  size_t signal_attr_counter = 0;
  size_t attr_def_default_counter = 0;

  auto bus_nodes = dbc.getBusNodes();
  auto messages = dbc.getMessages();
  auto attr_defs = dbc.getAttributeDefinitions();

  for (auto & bus_node : bus_nodes) {
    if (bus_node->getComment() != nullptr) {
      bus_node_comment_counter++;
    }
  }

  for (auto & msg : messages) {
    auto signals = msg.second->getSignals();

    if (msg.second->getComment() != nullptr) {
      message_comment_counter++;
    }

    signal_counter += signals.size();

    for (auto & sig : signals) {
      if (sig.second->getComment() != nullptr) {
        signal_comment_counter++;
      }
    }
  }

  for (const auto & attr_def : attr_defs) {
    switch (attr_def->getDbcObjType()) {
      case DbcObjType::BUS_NODES:
        bus_node_attr_counter++;
        break;
      case DbcObjType::MESSAGE:
        message_attr_counter++;
        break;
      case DbcObjType::SIGNAL:
        signal_attr_counter++;
        break;
    }

    if (attr_def->getAttrType() == AttributeType::ENUM) {
      auto temp_attr = dynamic_cast<const EnumAttribute *>(attr_def);

      if (temp_attr->getDefaultValue() != nullptr) {
        attr_def_default_counter++;
      }
    } else if (attr_def->getAttrType() == AttributeType::FLOAT) {
      auto temp_attr = dynamic_cast<const FloatAttribute *>(attr_def);

      if (temp_attr->getDefaultValue() != nullptr) {
        attr_def_default_counter++;
      }
    } else if (attr_def->getAttrType() == AttributeType::INT) {
      auto temp_attr = dynamic_cast<const IntAttribute *>(attr_def);

      if (temp_attr->getDefaultValue() != nullptr) {
        attr_def_default_counter++;
      }
    } else if (attr_def->getAttrType() == AttributeType::STRING) {
      auto temp_attr = dynamic_cast<const StringAttribute *>(attr_def);

      if (temp_attr->getDefaultValue() != nullptr) {
        attr_def_default_counter++;
      }
    }
  }

  std::cout << "Found " << bus_nodes.size() << " bus nodes.\n";
  std::cout << "Found " << dbc.getMessages().size() << " messages.\n";
  std::cout << "Found " << signal_counter << " signals.\n";

  size_t total_comments = bus_node_comment_counter + message_comment_counter + signal_comment_counter;

  std::cout << "Found " << total_comments << " comments (Bus nodes: " << bus_node_comment_counter;
  std::cout << ", Messages: " << message_comment_counter << ", Signals: " << signal_comment_counter << ").\n";
  std::cout << "Found " << attr_defs.size() << " attribute definitions (Bus nodes: " << bus_node_attr_counter;
  std::cout << ", Messages: " << message_attr_counter << ", Signals: " << signal_attr_counter << ").\n";
  std::cout << "Found " << attr_def_default_counter << " attribute default values.";
  std::cout << std::endl;
  
  return 0;
}
