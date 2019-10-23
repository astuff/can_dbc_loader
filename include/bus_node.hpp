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

#ifndef BUS_NODE_HPP_
#define BUS_NODE_HPP_

#include "common_defs.hpp"
#include "comment.hpp"

#include <memory>
#include <string>

namespace AS
{
namespace CAN
{
namespace DbcLoader
{

class BusNode
  : public AttrObj
{
public:
  BusNode(std::string && node_name);
  ~BusNode() = default;
  BusNode(const BusNode & other);
  BusNode(BusNode && other) = default;
  BusNode & operator=(const BusNode & other);
  BusNode & operator=(BusNode && other) = default;

  std::string getName() const;
  const std::string * getComment() const;

  friend class Database;
  friend class Message;
  friend class Signal;

private:
  std::string name_;
  std::unique_ptr<std::string> comment_;
};

}  // namespace DbcLoader
}  // namespace CAN
}  // namespace AS

#endif  // BUS_NODE_HPP_
