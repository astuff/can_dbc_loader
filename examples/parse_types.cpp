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

#include <message.hpp>
#include <signal.hpp>

#include <iostream>

static std::string message_text = "BO_ 1045 OCCUPANCY_RPT: 2 PACMOD";
static std::string signal_text = " SG_ VEHICLE_SPEED : 7|16@0- (0.01,0) [-327.68|327.67] \"m/s\"  CUSTOMER_ECU";

using AS::CAN::DbcLoader::Message;
using AS::CAN::DbcLoader::Order;
using AS::CAN::DbcLoader::Signal;

int main(int argc, char ** argv)
{
  Message msg(std::move(message_text));

  std::cout << "Message ID: 0x" << std::hex << msg.getId() << std::endl;
  std::cout << "Message name: " << msg.getName() << std::endl;
  std::cout << "Message DLC: " << msg.getDlc() << std::endl;
  std::cout << "Message transmitting node: " << msg.getTransmittingNode().getName() << std::endl << std::endl;

  Signal sig(std::move(signal_text));

  std::cout << "Signal name: " << sig.getName() << std::endl;
  std::cout << "Signal is multiplex def: " << (sig.isMultiplexDef() ? "true" : "false") << std::endl;

  if (sig.getMultiplexId() != nullptr) {
    std::cout << "Signal multiplex identifier: " << *(sig.getMultiplexId()) << std::endl;
  }

  std::cout << "Signal start bit: " << static_cast<unsigned int>(sig.getStartBit()) << std::endl;
  std::cout << "Signal length: " << static_cast<unsigned int>(sig.getLength()) << std::endl;
  std::cout << "Signal endianness: " << (sig.getEndianness() == Order::BE ? "big-endian" : "little-endian") << std::endl;
  std::cout << "Signal is signed: " << (sig.isSigned() ? "true" : "false") << std::endl;
  std::cout << "Signal factor: " << sig.getFactor() << std::endl;
  std::cout << "Signal offset: " << sig.getOffset() << std::endl;
  std::cout << "Signal min value: " << sig.getMinVal() << std::endl;
  std::cout << "Signal max value: " << sig.getMaxVal() << std::endl;
  std::cout << "Signal unit: " << sig.getUnit() << std::endl;
  std::cout << "Signal receiving bus nodes: ";

  for (auto & node : sig.getReceivingNodes()) {
    std::cout << node.getName() << " ";
  }

  std::cout << std::endl;

  return 0;
}
