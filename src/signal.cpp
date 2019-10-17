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

#include "signal.hpp"

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

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS
