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

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include "common_defs.hpp"
#include "bus_node.hpp"
#include "comment.hpp"
#include "signal.hpp"

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

class Message
  : public DbcObj, public AttrObj
{
public:
  Message(std::string && dbc_text);
  Message(
    unsigned int id,
    std::string && name,
    unsigned char dlc,
    BusNode && transmitting_node,
    std::vector<Signal> && signals);
  ~Message() = default;
  Message(const Message & other);
  Message(Message && other) = default;
  Message & operator=(const Message & other);
  Message & operator=(Message && other) = default;

  unsigned int getId() const;
  std::string getName() const;
  unsigned char getDlc() const;
  unsigned char getLength() const;
  BusNode getTransmittingNode() const;
  std::unordered_map<std::string, const Signal *> getSignals() const;
  const std::string * getComment() const;

  unsigned char dlcToLength(const unsigned char & dlc) const;

  friend class Database;

private:
  unsigned int id_;
  std::string name_;
  unsigned char dlc_;
  BusNode transmitting_node_;
  std::unordered_map<std::string, Signal> signals_;
  std::unique_ptr<std::string> comment_;
  
  void generateText() override;
  void parse() override;
};

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS

#endif  // MESSAGE_HPP_
