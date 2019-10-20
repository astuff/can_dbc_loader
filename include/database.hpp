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

#include "common_defs.hpp"
#include "attribute.hpp"
#include "bus_node.hpp"
#include "comment.hpp"
#include "message.hpp"

#include <fstream>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

class Database
{
public:
  Database(const std::string & dbc_path);
  Database(std::istream & mem_stream);
  Database(
    std::string && version,
    std::string && bus_config,
    std::vector<BusNode> && bus_nodes,
    std::unordered_map<unsigned int, Message> && messages,
    std::vector<std::shared_ptr<Attribute>> && attribute_definitions);

  void generateDbcFile(const std::string & dbc_path);
  std::string getVersion();
  std::string getBusConfig();
  std::vector<BusNode> getBusNodes();
  std::unordered_map<unsigned int, Message> getMessages();
  std::vector<std::shared_ptr<Attribute>> getAttributeDefinitions();

private:
  std::string version_;
  std::string bus_config_;
  std::vector<BusNode> bus_nodes_;
  std::unordered_map<unsigned int, Message> messages_;
  std::vector<std::shared_ptr<Attribute>> attribute_defs_;

  void parse(std::istream & reader);
  void saveMsg(std::unique_ptr<Message> & msg_ptr);
};

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS

#endif
