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
#include <sstream>
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
  bool is_multiplex_def,
  unsigned int multiplex_id,
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
    is_multiplex_def_(is_multiplex_def),
    multiplex_id_(new unsigned int(multiplex_id)),
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
    value_defs_(value_definitions),
    comment_(nullptr)
{
  generateText();
}

Signal::Signal(const Signal & other)
  : name_(other.name_),
    is_multiplex_def_(other.is_multiplex_def_),
    start_bit_(other.start_bit_),
    length_(other.length_),
    endianness_(other.endianness_),
    is_signed_(other.is_signed_),
    factor_(other.factor_),
    offset_(other.offset_),
    min_(other.min_),
    max_(other.max_),
    unit_(other.unit_),
    receiving_nodes_(other.receiving_nodes_),
    value_defs_(other.value_defs_)
{
  if (other.multiplex_id_) {
    multiplex_id_ = std::make_unique<unsigned int>(*(other.multiplex_id_));
  } else {
    multiplex_id_ = nullptr;
  }

  if (other.comment_) {
    comment_ = std::make_unique<std::string>(*(other.comment_));
  } else {
    comment_ = nullptr;
  }
}

Signal & Signal::operator=(const Signal & other)
{
  return *this = Signal(other);
}

std::string Signal::getName()
{
  return name_;
}

bool Signal::isMultiplexDef()
{
  return is_multiplex_def_;
}

const unsigned int * Signal::getMultiplexId()
{
  return multiplex_id_.get();
}

unsigned char Signal::getStartBit()
{
  return start_bit_;
}

unsigned char Signal::getLength()
{
  return length_;
}

Order Signal::getEndianness()
{
  return endianness_;
}

bool Signal::isSigned()
{
  return is_signed_;
}

float Signal::getFactor()
{
  return factor_;
}

float Signal::getOffset()
{
  return offset_;
}

float Signal::getMinVal()
{
  return min_;
}

float Signal::getMaxVal()
{
  return max_;
}

std::string Signal::getUnit()
{
  return unit_;
}

std::vector<BusNode> Signal::getReceivingNodes()
{
  return receiving_nodes_;
}

std::map<int, std::string> Signal::getValueDefinitions()
{
  return value_defs_;
}

const std::string * Signal::getComment()
{
  return comment_.get();
}

void Signal::generateText()
{
  std::ostringstream output;

  output << " SG_ " << name_;

  if (is_multiplex_def_) {
    output << " M";
  } else if (multiplex_id_) {
    output << " m" << *multiplex_id_;
  }

  output << " : " << start_bit_ << "|";
  output << length_ << "@";

  if (endianness_ == Order::LE) {
    output << 1;
  } else {
    output << 0;
  }

  if (is_signed_) {
    output << "-";
  } else {
    output << "+";
  }

  output << " (" << factor_ << ",";
  output << offset_ << ") [";
  output << min_ << "|" << max_ << "] \"";
  output << unit_ << "\" ";

  if (receiving_nodes_.size() < 1) {
    output << "Vector__XXX";
  } else {
    for (auto i = 0; i < receiving_nodes_.size(); ++i) {
      output << receiving_nodes_[i].name_;

      if (i != receiving_nodes_.size() - 1) {
        output << ",";
      }
    }
  }
  
  dbc_text_ = output.str();
}

void Signal::parse()
{
  std::istringstream input(dbc_text_);
  std::string temp_string;

  input.ignore(5);
  input >> name_;
  input >> temp_string;

  if (temp_string != ":") {
    if (temp_string == "M") {
      is_multiplex_def_ = true;
    } else {
      // Assumed to be multiplex identifier
      multiplex_id_ = std::make_unique<unsigned int>(
        static_cast<unsigned int>(
          std::stoul(
            temp_string.substr(1, temp_string.length() - 1))));
    }

    input.ignore(3);
  }

  input >> temp_string;

  auto bar = temp_string.find("|");
  auto at = temp_string.find("@");

  if (bar != std::string::npos && at != std::string::npos) {
    start_bit_ = static_cast<unsigned char>(std::stoul(temp_string.substr(0, bar)));
    length_ = static_cast<unsigned char>(std::stoul(temp_string.substr(bar + 1, at - bar - 1)));

    if (temp_string[at + 1] == '0') {
      endianness_ = Order::BE;
    } else if (temp_string[at + 1] == '1') {
      endianness_ = Order::LE;
    } else {
      throw DbcParseException();
    }

    if (temp_string[at + 2] == '+') {
      is_signed_ = false;
    } else if (temp_string[at + 2] == '-') {
      is_signed_ = true;
    } else {
      throw DbcParseException();
    }
  } else {
    throw DbcParseException();
  }

  input >> temp_string;

  auto comma = temp_string.find(",");

  if (comma != std::string::npos) {
    factor_ = std::stof(temp_string.substr(1, comma - 1));
    offset_ = std::stof(temp_string.substr(comma + 1, temp_string.length() - comma - 2));
  } else {
    throw DbcParseException();
  }

  input >> temp_string;

  bar = temp_string.find("|");

  if (bar != std::string::npos) {
    min_ = std::stof(temp_string.substr(1, bar - 1));
    max_ = std::stof(temp_string.substr(bar + 1, temp_string.length() - bar - 2));
  } else {
    throw DbcParseException();
  }

  input >> temp_string;

  if (temp_string.length() > 3) {
    unit_ = temp_string.substr(1, temp_string.length() - 2);
  }

  input >> temp_string;

  if (temp_string.empty()) {
    // There are sometimes 2 spaces after the unit
    input >> temp_string;
  }

  auto first_comma = temp_string.find(",");

  if (first_comma != std::string::npos) {
    while (std::getline(input, temp_string, ',')) {
      if (temp_string != "Vector__XXX") {
        receiving_nodes_.emplace_back(std::move(temp_string));
      }
    }
  } else {
    receiving_nodes_.emplace_back(std::move(temp_string));
  }
}

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS
