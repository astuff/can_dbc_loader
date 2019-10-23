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

class Comment
{
public:
  std::string getComment() const;

  friend class BusNode;
  friend class Database;
  friend class Message;
  friend class Signal;

protected:
  std::string comment_;
};

class BusNodeComment
  : public Comment, public DbcObj
{
public:
  BusNodeComment(std::string && dbc_text);
  BusNodeComment(
    std::string && node_name,
    std::string && comment);

  std::string getNodeName() const;

  friend class BusNode;

private:
  void generateText() override;
  void parse() override;

  std::string node_name_;
};

class MessageComment
  : public Comment, public DbcObj
{
public:
  MessageComment(std::string && dbc_text);
  MessageComment(unsigned int msg_id, std::string && comment);

  unsigned int getMsgId() const;

  friend class Message;

private:
  void generateText() override;
  void parse() override;

  unsigned int msg_id_;
};

class SignalComment
  : public Comment, public DbcObj
{
public:
  SignalComment(std::string && dbc_text);
  SignalComment(
    unsigned int msg_id,
    std::string && signal_name,
    std::string && comment);

  unsigned int getMsgId() const;
  std::string getSignalName() const;

  friend class Signal;

private:
  void generateText() override;
  void parse() override;

  unsigned int msg_id_;
  std::string signal_name_;
};

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS

#endif  // COMMENT_HPP_
