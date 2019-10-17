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

#include "comment.hpp"

#include <string>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

BusNodeComment::BusNodeComment(std::string && dbc_text)
{
  dbc_text_ = std::move(dbc_text);
  parse();
}

BusNodeComment::BusNodeComment(std::string && bus_node_name, std::string && comment)
  : node_name(bus_node_name),
    comment(comment)
{
  generateText();
}

void BusNodeComment::generateText()
{
  // TODO(jwhitleyastuff): Do the thing!
}

void BusNodeComment::parse()
{
  // TODO(jwhitleyastuff): Do the thing!
}

MessageComment::MessageComment(std::string && dbc_text)
  : msg_id(0)
{
  dbc_text_ = std::move(dbc_text);
  parse();
}

MessageComment::MessageComment(unsigned int msg_id, std::string && comment)
  : msg_id(msg_id),
    comment(comment)
{
  generateText();
}

void MessageComment::generateText()
{
  // TODO(jwhitleyastuff): Do the thing!
}

void MessageComment::parse()
{
  // TODO(jwhitleyastuff): Do the thing!
}

SignalComment::SignalComment(std::string && dbc_text)
  : msg_id(0),
    signal_name("")
{
  dbc_text_ = std::move(dbc_text);
  parse();
}

SignalComment::SignalComment(
  unsigned int msg_id,
  std::string && signal_name,
  std::string && comment)
  : msg_id(msg_id),
    signal_name(signal_name),
    comment(comment)
{
  generateText();
}

void SignalComment::generateText()
{
  // TODO(jwhitleyastuff): Do the thing!
}

void SignalComment::parse()
{
  // TODO(jwhitleyastuff): Do the thing!
}

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS
