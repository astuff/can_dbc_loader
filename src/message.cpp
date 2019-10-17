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

#include "message.hpp"

#include <memory>
#include <string>
#include <vector>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

Message::Message(std::string && message_text)
  : transmitting_node_(BusNode(""))
{
  dbc_text_ = std::move(message_text);
  parse();
}

Message::Message(
  unsigned int id,
  std::string && name,
  unsigned char dlc,
  BusNode && transmitting_node,
  std::vector<Signal> && signals)
  : id_(id),
    name_(name),
    dlc_(dlc),
    transmitting_node_(transmitting_node),
    signals_(signals)
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

const std::shared_ptr<MessageComment> Message::getComment()
{
  return std::shared_ptr<MessageComment>(comment_);
}

void Message::generateText()
{
  // TODO(jwhitleyastuff): Do the thing!
}

void Message::parse()
{
  // TODO(jwhitleyastuff): Do the thing!
}

const unsigned char Message::dlcToLength(const unsigned char & dlc)
{
  return DLC_LENGTH[dlc];
}

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS
