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

#ifndef SIGNAL_HPP_
#define SIGNAL_HPP_

#include "common_defs.hpp"
#include "bus_node.hpp"
#include "comment.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

class Signal
  : public DbcObj, public AttributeObject
{
public:
  Signal(std::string && dbc_text);
  Signal(
    std::string && name,
    bool is_multiplex_def,
    std::shared_ptr<unsigned int> multiplex_id,
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

  std::string getName();
  bool isMultiplexDef();
  std::shared_ptr<unsigned int> getMultiplexId();
  unsigned char getStartBit();
  unsigned char getLength();
  Order getEndianness();
  bool isSigned();
  float getFactor();
  float getOffset();
  float getMinVal();
  float getMaxVal();
  std::string getUnit();
  std::vector<BusNode> getReceivingNodes();
  std::map<int, std::string> getValueDefinitions();
  std::shared_ptr<SignalComment> getComment();

  friend class Message;
  friend class Database;

private:
  std::string name_;
  bool is_multiplex_def_;
  std::shared_ptr<unsigned int> multiplex_id_;
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

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS

#endif  // SIGNAL_HPP_
