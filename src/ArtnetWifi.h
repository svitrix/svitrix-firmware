/*The MIT License (MIT)

Copyright (c) 2014 Nathanaël Lécaudé
https://github.com/natcl/Artnet, http://forum.pjrc.com/threads/24688-Artnet-to-OctoWS2811

Copyright (c) 2016,2019 Stephan Ruloff
https://github.com/rstephan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#include <functional>
#include <cstring>
#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_SAMD)
#if defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#else
#include <WiFiNINA.h>
#endif
#else
#error "Architecture not supported!"
#endif

#include <WiFiUdp.h>

namespace artnetlib
{

// Art-Net protocol constants
static constexpr uint16_t Port = 6454;
static constexpr size_t MaxBuffer = 530;
static constexpr size_t DmxStart = 18;
static constexpr size_t MaxDmxChannels = 512;
static constexpr uint16_t ProtocolVersion = 14;
static constexpr char Id[] = "Art-Net";

enum class OpCode : uint16_t
{
    None = 0x0000,
    Poll = 0x2000,
    Dmx = 0x5000,
    Sync = 0x5200,
};

using DmxCallback = std::function<void(uint16_t universe, uint16_t length,
                                       uint8_t sequence, uint8_t *data)>;

template <typename UdpType = WiFiUDP>
class Node
{
  public:
    // ── Receiver ──────────────────────────────────────────────────────

    void begin(uint16_t port = Port)
    {
        udp_.begin(port);
        txSequence_ = 1;
        physical_ = 0;
    }

    OpCode read()
    {
        auto size = udp_.parsePacket();
        if (size <= 0 || static_cast<size_t>(size) > MaxBuffer)
        {
            return OpCode::None;
        }

        senderIp_ = udp_.remoteIP();
        udp_.read(buffer_, MaxBuffer);

        if (std::memcmp(buffer_, Id, sizeof(Id)) != 0)
        {
            return OpCode::None;
        }

        auto opcode = static_cast<OpCode>(
            static_cast<uint16_t>(buffer_[8]) | (static_cast<uint16_t>(buffer_[9]) << 8));

        if (opcode == OpCode::Dmx)
        {
            rxSequence_ = buffer_[12];
            rxUniverse_ = static_cast<uint16_t>(buffer_[14]) | (static_cast<uint16_t>(buffer_[15]) << 8);
            rxLength_ = static_cast<uint16_t>(buffer_[17]) | (static_cast<uint16_t>(buffer_[16]) << 8);

            // Clamp to DMX maximum to prevent out-of-bounds access
            if (rxLength_ > MaxDmxChannels)
            {
                rxLength_ = MaxDmxChannels;
            }

            if (dmxCallback_)
            {
                dmxCallback_(rxUniverse_, rxLength_, rxSequence_, buffer_ + DmxStart);
            }
        }

        return opcode;
    }

    void onDmx(DmxCallback cb)
    {
        dmxCallback_ = std::move(cb);
    }

    // Receiver getters
    uint8_t *dmxData()
    {
        return buffer_ + DmxStart;
    }
    uint16_t universe() const
    {
        return rxUniverse_;
    }
    uint16_t dmxLength() const
    {
        return rxLength_;
    }
    uint8_t sequence() const
    {
        return rxSequence_;
    }
    IPAddress senderIp() const
    {
        return senderIp_;
    }

    // ── Sender ────────────────────────────────────────────────────────

    void setUniverse(uint16_t universe)
    {
        txUniverse_ = universe;
    }
    void setPhysical(uint8_t port)
    {
        physical_ = port;
    }
    void setLength(uint16_t len)
    {
        txLength_ = len;
    }

    void setChannel(uint16_t channel, uint8_t value)
    {
        if (channel >= MaxDmxChannels)
            return;
        buffer_[DmxStart + channel] = value;
    }

    int write(const char *host, uint16_t port = Port)
    {
        auto len = buildTxPacket();
        udp_.beginPacket(host, port);
        udp_.write(buffer_, DmxStart + len);
        return udp_.endPacket();
    }

    int write(IPAddress ip, uint16_t port = Port)
    {
        auto len = buildTxPacket();
        udp_.beginPacket(ip, port);
        udp_.write(buffer_, DmxStart + len);
        return udp_.endPacket();
    }

    // ── Debug ─────────────────────────────────────────────────────────

    void printHeader() const
    {
        Serial.print("universe=");
        Serial.print(rxUniverse_);
        Serial.print(" len=");
        Serial.print(rxLength_);
        Serial.print(" seq=");
        Serial.println(rxSequence_);
    }

    void printContent() const
    {
        for (uint16_t i = 0; i < rxLength_; i++)
        {
            Serial.print(buffer_[DmxStart + i], DEC);
            Serial.print("  ");
        }
        Serial.println();
    }

  private:
    uint16_t buildTxPacket()
    {
        std::memcpy(buffer_, Id, sizeof(Id));

        auto op = static_cast<uint16_t>(OpCode::Dmx);
        buffer_[8] = static_cast<uint8_t>(op);
        buffer_[9] = static_cast<uint8_t>(op >> 8);
        buffer_[10] = static_cast<uint8_t>(ProtocolVersion >> 8);
        buffer_[11] = static_cast<uint8_t>(ProtocolVersion);
        buffer_[12] = txSequence_++;
        if (txSequence_ == 0)
            txSequence_ = 1;
        buffer_[13] = physical_;
        buffer_[14] = static_cast<uint8_t>(txUniverse_);
        buffer_[15] = static_cast<uint8_t>(txUniverse_ >> 8);

        uint16_t len = txLength_ + (txLength_ % 2); // pad to even
        buffer_[16] = static_cast<uint8_t>(len >> 8);
        buffer_[17] = static_cast<uint8_t>(len);

        return len;
    }

    // Transport
    UdpType udp_;
    uint8_t buffer_[MaxBuffer]{};

    // Receiver state
    uint16_t rxUniverse_{0};
    uint16_t rxLength_{0};
    uint8_t rxSequence_{0};
    IPAddress senderIp_;

    // Sender state
    uint16_t txUniverse_{0};
    uint16_t txLength_{0};
    uint8_t txSequence_{1};
    uint8_t physical_{0};

    // Callback
    DmxCallback dmxCallback_;
};

} // namespace artnetlib

// Backward-compatible alias
using ArtnetWifi = artnetlib::Node<>;
