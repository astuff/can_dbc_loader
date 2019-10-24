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
#include <sstream>
#include <string>
#include <vector>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

Message::Message(std::string && message_text)
  : transmitting_node_(BusNode("")),
    comment_(nullptr)
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
    comment_(nullptr)
{
  for (auto & signal : signals) {
    signals_.emplace(std::make_pair(signal.getName(), std::move(signal)));
  }

  generateText();
}

Message::Message(const Message & other)
  : id_(other.id_),
    name_(other.name_),
    dlc_(other.dlc_),
    transmitting_node_(other.transmitting_node_),
    signals_(other.signals_)
{
  if (other.comment_) {
    comment_ = std::make_unique<std::string>(*(other.comment_));
  } else {
    comment_ = nullptr;
  }
}

Message & Message::operator=(const Message & other)
{
  return *this = Message(other);
}

unsigned int Message::getId() const
{
  return id_;
}

std::string Message::getName() const
{
  return name_;
}

unsigned char Message::getDlc() const
{
  return dlc_;
}

unsigned char Message::getLength() const
{
  return dlcToLength(dlc_);
}

BusNode Message::getTransmittingNode() const
{
  return BusNode(transmitting_node_);
}

std::unordered_map<std::string, const Signal *> Message::getSignals() const
{
  std::unordered_map<std::string, const Signal *> sigs;

  for (auto & sig : signals_) {
    sigs[sig.first] = &(sig.second);
  }

  return sigs;
}

const std::string * Message::getComment() const
{
  return comment_.get();
}

void Message::generateText()
{
  std::ostringstream output;

  output << "BO_ " << id_ << " ";
  output << name_ << ": ";
  output << dlc_ << " ";
  output << transmitting_node_.name_;
  output << std::endl;

  dbc_text_ = output.str();
}

void Message::parse()
{
  std::istringstream input(dbc_text_);

  input.ignore(4);
  input >> id_;
  input >> name_;
  input >> dlc_;
  input >> transmitting_node_.name_;

  // Remove colon after name
  name_ = name_.substr(0, name_.length() - 1);
}

unsigned char Message::dlcToLength(const unsigned char & dlc)
{
  return DLC_LENGTH[dlc];
}

MessageTranscoder::MessageTranscoder(Message * dbc_msg)
  : msg_def_(dbc_msg),
    data_()
{
  data_.assign(Message::dlcToLength(dbc_msg->getDlc()), 0);

  for (auto sig = msg_def_->signals_.begin(); sig != msg_def_->signals_.end(); ++sig) {
    signal_xcoders_.emplace(sig->first, &(sig->second));
  }
}

const Message * MessageTranscoder::getMessageDef()
{
  return msg_def_;
}

void MessageTranscoder::decode(std::vector<uint8_t> && raw_data, TranscodeError * err)
{
  data_ = std::move(raw_data);
  decodeRawData(err);
}

void MessageTranscoder::decodeRawData(TranscodeError * err)
{
  err = new TranscodeError(TranscodeErrorType::NONE, "");

  // TODO(jwhitleyastuff): Do the thing.
}

std::vector<uint8_t> MessageTranscoder::encode(TranscodeError * err)
{
  return std::vector<uint8_t>(data_.begin(), data_.end());
}

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS
