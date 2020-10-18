/*
    Copyright 2016-2020 (C) Alexey Dynda

    This file is part of Tiny SLIP Library.

    Tiny SLIP Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tiny SLIP Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Tiny SLIP Library.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 This is Tiny SLIP Protocol implementation for microcontrollers

 @file
 @brief Tiny SLIP Protocol Arduino API

*/

#pragma once

#ifdef ARDUINO
#   include <HardwareSerial.h>
#else
#   include <stdint.h>
#   include <string.h>
#endif

#include <stdio.h>

namespace SLIP {

/**
 * Describes packet entity and provides API methods to
 * manipulate the packet.
 */
class IPacket
{
public:
    /**
     * Creates packet object.
     * @param buf - pointer to the buffer to store packet data
     * @param size - size of the buffer to hold packet data
     * @note passed buffer must exist all lifecycle of the Packet object.
     */
    IPacket(char *buf, size_t size)     { m_len = 0; m_size = static_cast<int>(size); m_buf = (uint8_t*)buf; m_p=0; }

    /**
     * Destroys the object
     */
    virtual ~IPacket() = default;

    /**
     * Clears Packet state. Buffer and its size are preserved.
     */
    void clear  ()              { m_len = 0; m_p = 0; }

    /**
     * Puts next byte to the packet. For example, after calling this method
     * twice: put(5), put(10), - the Packet will contain 5,10.
     * @param byte - data byte to put.
     */
    void put    (uint8_t byte)  { m_buf[m_len++] = byte; }

    /**
     * Puts next char to the packet. For example, after calling this method
     * twice: put('a'), put('c'), - the Packet will contain 'ac'.
     * @param chr - character to put.
     */
    void put    (char chr)      { put((uint8_t)chr); }

    /**
     * Puts next 16-bit unsigned integer to the packet.
     * @param data - data to put.
     */
    inline void put    (uint16_t data) { m_buf[m_len++] = data & 0x00FF;
                                         m_buf[m_len++] = data >> 8; }

    /**
     * Puts next 32-bit unsigned integer to the packet.
     * @param data - data to put.
     */
    inline void put    (uint32_t data) { put((uint16_t)(data & 0x0000FFFF));
                                         put((uint16_t)(data >> 16)); }

    /**
     * Puts next 16-bit signed integer to the packet.
     * @param data - data to put.
     */
    inline void put    (int16_t  data) { put((uint16_t)data); }

    /**
     * Puts next null-terminated string to the packet.
     * @param str - string to put.
     */
    inline void put    (const char *str){ strncpy((char *)&m_buf[m_len], str, m_size - m_len);
                                          m_len += static_cast<int>(strlen(str));
                                          m_buf[m_len++] = 0; }

    /**
     * Adds data from packet to the new packet being built.
     * @param pkt - reference to the Packet to add.
     */
    inline void put    (const IPacket &pkt){ memcpy(&m_buf[m_len], pkt.m_buf, pkt.m_len); m_len += pkt.m_len; }

    /**
     * Reads next byte from the packet.
     * @return byte from the packet.
     */
    inline uint8_t getByte   ()        { return m_buf[m_p++]; }

    /**
     * Reads next character from the packet.
     * @return character from the packet.
     */
    inline char getChar      ()        { return (char)IPacket::getByte(); }

    /**
     * Reads next unsigned 16-bit integer from the packet.
     * @return unsigned 16-bit integer.
     */
    inline uint16_t getUint16()        { uint16_t t = m_buf[m_p++]; return t | ((uint16_t)m_buf[m_p++] << 8); }

    /**
     * Reads next signed 16-bit integer from the packet.
     * @return signed 16-bit integer.
     */
    inline int16_t getInt16  ()        { return (int16_t)(getUint16()); }

    /**
     * Reads next unsigned 32-bit integer from the packet.
     * @return unsigned 32-bit integer.
     */
    inline uint32_t getUint32()        { return getUint16() | ((uint32_t)getUint16())<<16; }

    /**
     * Reads zero-terminated string from the packet.
     * @return zero-terminated string.
     */
    inline char* getString   ()        { char *p = (char *)&m_buf[m_p]; m_p += static_cast<int>(strlen(p)) + 1; return p; }

    /**
     * Returns size of payload data in the received packet.
     * @return size of payload data.
     */
    inline size_t size       () const  { return m_len; }

    /**
     * Returns maximum size of packet buffer.
     * @return max size of packet buffer.
     */
    inline size_t maxSize    () const  { return m_size; }

    /**
     * Returns size of payload data in the received packet.
     * @return size of payload data.
     */
    inline char *data        ()        { return (char*)m_buf; }

    /**
     * You may refer to Packet payload data directly by using operator []
     */
    uint8_t &operator[]      (size_t idx) { return m_buf[idx]; }

private:
    friend class        SLIP;

    uint8_t*            m_buf;
    int                 m_size;
    int                 m_len;
    int                 m_p;
};

/**
 * Template  class to create packet with static allocation of buffer
 * Use this class for microcontrollers with few resources.
 */
template <size_t S>
class Packet: public IPacket
{
public:
    /**
     * Creates IPacket instance with statically allocated buffer
     */
    Packet(): IPacket(m_data, S) {}

private:
    char m_data[S]{};
};

/**
 * Class which allocated buffer for packet dynamically.
 * Use this class only on powerful microcontrollers.
 */
class PacketD: public IPacket
{
public:
    /**
     * Creates packet with dynamically allocated buffer.
     * @param size number of bytes to allocate for the packet buffer.
     */
    explicit PacketD(int size): IPacket((char *)(new uint8_t[size]), size) {}

    ~PacketD() { delete[] (uint8_t *)data(); }

private:
};


} // Tiny namespace

