// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *                 packed.cpp - tagged byte-buffer codec                 *
// *************************************************************************

#include "packed.h"

#include <cstring>

// ---- writer ---------------------------------------------------------------

void TPackedWriter::PutVarint(uint32_t v)
{
    while (v >= 0x80u)
    {
        buf_.push_back(uint8_t(v | 0x80u));
        v >>= 7;
    }
    buf_.push_back(uint8_t(v));
}

void TPackedWriter::Float(uint16_t id, float v)
{
    Tag(id, EPackedWire::Fixed32);
    uint8_t b[4];
    std::memcpy(b, &v, 4);
    buf_.insert(buf_.end(), b, b + 4);
}

void TPackedWriter::Vec3(uint16_t id, const float v[3])
{
    Bytes(id, v, 12);
}

void TPackedWriter::Color(uint16_t id, const float c[4])
{
    Bytes(id, c, 16);
}

void TPackedWriter::Bytes(uint16_t id, const void* data, uint32_t size)
{
    Tag(id, EPackedWire::Bytes);
    PutVarint(size);
    const uint8_t* p = static_cast<const uint8_t*>(data);
    buf_.insert(buf_.end(), p, p + size);
}

// ---- reader ---------------------------------------------------------------

bool TPackedReader::GetVarint(uint32_t& out)
{
    out = 0;
    for (int32_t shift = 0; shift < 35 && cur_ < end_; shift += 7)
    {
        const uint8_t b = *cur_++;
        out |= uint32_t(b & 0x7Fu) << shift;
        if (!(b & 0x80u))
            return true;
    }
    return false;
}

bool TPackedReader::Next(SPackedField& f)
{
    if (cur_ >= end_)
        return false;
    uint32_t tag = 0;
    if (!GetVarint(tag))
        return false;
    f.id   = uint16_t(tag >> 2);
    f.wire = EPackedWire(tag & 3u);
    switch (f.wire)
    {
        case EPackedWire::Varint:
        {
            const uint8_t* start = cur_;
            uint32_t dummy = 0;
            if (!GetVarint(dummy))
                return false;
            f.value = start;
            f.size  = uint32_t(cur_ - start);
            return true;
        }
        case EPackedWire::Fixed32:
            if (end_ - cur_ < 4)
                return false;
            f.value = cur_;
            f.size  = 4;
            cur_ += 4;
            return true;
        case EPackedWire::Bytes:
        {
            uint32_t len = 0;
            if (!GetVarint(len) || uint32_t(end_ - cur_) < len)
                return false;
            f.value = cur_;
            f.size  = len;
            cur_ += len;
            return true;
        }
        default:
            return false;
    }
}

namespace {

bool DecodeVarint(const SPackedField& f, uint32_t& out)
{
    if (f.wire != EPackedWire::Varint)
        return false;
    out = 0;
    for (uint32_t i = 0; i < f.size; ++i)
        out |= uint32_t(f.value[i] & 0x7Fu) << (7 * i);
    return true;
}

} // namespace

bool TPackedReader::Bool(const SPackedField& f, bool fallback)
{
    uint32_t u = 0;
    return DecodeVarint(f, u) ? (u != 0) : fallback;
}

int32_t TPackedReader::Int(const SPackedField& f, int32_t fallback)
{
    uint32_t u = 0;
    return DecodeVarint(f, u) ? TPackedWriter::Unzigzag(u) : fallback;
}

float TPackedReader::Float(const SPackedField& f, float fallback)
{
    if (f.wire != EPackedWire::Fixed32 || f.size != 4)
        return fallback;
    float v;
    std::memcpy(&v, f.value, 4);
    return v;
}

bool TPackedReader::Vec3(const SPackedField& f, float out[3])
{
    if (f.wire != EPackedWire::Bytes || f.size != 12)
        return false;
    std::memcpy(out, f.value, 12);
    return true;
}

bool TPackedReader::Color(const SPackedField& f, float out[4])
{
    if (f.wire != EPackedWire::Bytes || f.size != 16)
        return false;
    std::memcpy(out, f.value, 16);
    return true;
}

std::string_view TPackedReader::String(const SPackedField& f)
{
    if (f.wire != EPackedWire::Bytes)
        return {};
    return std::string_view(reinterpret_cast<const char*>(f.value), f.size);
}
