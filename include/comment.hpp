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

#ifndef COMMENT_HPP_
#define COMMENT_HPP_

#include "common_defs.hpp"

#include <string>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

class BusNodeComment
  : public DbcObj
{
public:
  BusNodeComment(std::string && dbc_text);
  BusNodeComment(
    std::string && bus_node_name,
    std::string && comment);

  const std::string node_name;
  const std::string comment;

private:
  void generateText() override;
  void parse() override;
};

class MessageComment
  : public DbcObj
{
public:
  MessageComment(std::string && dbc_text);
  MessageComment(unsigned int msg_id, std::string && comment);

  const unsigned int msg_id;
  const std::string comment;

private:
  void generateText() override;
  void parse() override;
};

class SignalComment
  : public DbcObj
{
public:
  SignalComment(std::string && dbc_text);
  SignalComment(
    unsigned int msg_id,
    std::string && signal_name,
    std::string && comment);

  const unsigned int msg_id;
  const std::string signal_name;
  const std::string comment;

private:
  void generateText() override;
  void parse() override;
};

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS

#endif  // COMMENT_HPP_