// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *            TString.cpp - Modern game style string module              *
// *************************************************************************

// This is a modern non-owning string class to make string handling easier.

#include "string.h"

#include <iostream>
#include <cctype>
#include <stdexcept>

// TString Implementation

void TString::SetSize(size_t size)
{
    sizeAndFlags = (sizeAndFlags & ~MAX_SIZE) | (size & MAX_SIZE);
}

void TString::SetDeletable(bool value)
{
    if (value)
    {
        sizeAndFlags |= FLAG_DELETABLE;
    }
    else
    {
        sizeAndFlags &= ~FLAG_DELETABLE;
    }
}

TString::TString()
{
    SetDeletable(false);
}

TString::TString(const char* str)
{
    if (!str)
    {
        throw std::invalid_argument("Null pointer passed to TString constructor");
    }
    size_t len = std::strlen(str);
    char* heapBuffer = new char[len + 1];
    std::memcpy(heapBuffer, str, len);
    heapBuffer[len] = '\0';
    data = heapBuffer;
    SetSize(len);
    SetDeletable(true);
}

TString::TString(const TString& other)
{
    size_t len = other.GetSize();
    char* heapBuffer = new char[len + 1];
    std::memcpy(heapBuffer, other.data, len);
    heapBuffer[len] = '\0';
    data = heapBuffer;
    SetSize(len);
    SetDeletable(true);
}

TString::TString(TString&& other) noexcept
    : data(other.data), sizeAndFlags(other.sizeAndFlags)
{
    other.data = nullptr;
    other.SetDeletable(false);
}

TString& TString::operator=(const TString& other)
{
    if (this == &other)
    {
        return *this;
    }

    if (IsDeletable())
    {
        delete[] data;
    }

    size_t len = other.GetSize();
    char* heapBuffer = new char[len + 1];
    std::memcpy(heapBuffer, other.data, len);
    heapBuffer[len] = '\0';
    data = heapBuffer;
    SetSize(len);
    SetDeletable(true);

    return *this;
}

TString& TString::operator=(TString&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (IsDeletable())
    {
        delete[] data;
    }

    data = other.data;
    sizeAndFlags = other.sizeAndFlags;
    other.data = nullptr;
    other.SetDeletable(false);

    return *this;
}

TString::~TString()
{
    if (IsDeletable())
    {
        delete[] data;
    }
}

size_t TString::GetSize() const
{
    return sizeAndFlags & MAX_SIZE;
}

bool TString::IsDeletable() const
{
    return sizeAndFlags & FLAG_DELETABLE;
}

const char* TString::CStr() const
{
    return data;
}

int TString::Compare(const TString& other) const
{
    return std::strcmp(data, other.data);
}

int TString::CompareI(const TString& other) const
{
    size_t len = std::min(GetSize(), other.GetSize());
    for (size_t i = 0; i < len; ++i)
    {
        int diff = std::tolower(data[i]) - std::tolower(other.data[i]);
        if (diff != 0)
        {
            return diff;
        }
    }
    return GetSize() - other.GetSize();
}

void TString::Append(const TString& other)
{
    Append(other.CStr());
}

void TString::Append(const char* str)
{
    if (!str)
    {
        throw std::invalid_argument("Null pointer passed to Append");
    }

    size_t oldSize = GetSize();
    size_t newSize = oldSize + std::strlen(str);

    char* newBuffer = new char[newSize + 1];
    std::memcpy(newBuffer, data, oldSize);
    std::memcpy(newBuffer + oldSize, str, std::strlen(str));
    newBuffer[newSize] = '\0';

    if (IsDeletable())
    {
        delete[] data;
    }

    data = newBuffer;
    SetSize(newSize);
    SetDeletable(true);
}

void TString::Append(char c)
{
    char str[2] = {c, '\0'};
    Append(str);
}

const char* TString::begin() const
{
    return data;
}

const char* TString::end() const
{
    return data + GetSize();
}

bool TString::operator==(const TString& other) const
{
    return std::strcmp(data, other.data) == 0;
}

void TString::Print() const
{
    std::cout << "TString: " << CStr() << ", Size: " << GetSize()
              << ", Deletable: " << IsDeletable() << std::endl;
}

// Hash function for TString
size_t TString::Hash::operator()(const TString& str) const noexcept
{
    return std::hash<std::string_view>()(std::string_view(str.CStr(), str.GetSize()));
}

// TRefString Implementation

TRefString::TRefString(const char* str)
{
    data = str;
    SetSize(std::strlen(str));
    SetDeletable(false);
}

// TStackString Implementation

template <size_t N>
TStackString<N>::TStackString(const char* str)
{
    size_t len = std::min(std::strlen(str), N);
    std::memcpy(buffer, str, len);
    buffer[len] = '\0';
    data = buffer;
    SetSize(len);
    SetDeletable(false);
}

template <size_t N>
TStackString<N>::TStackString()
{
    buffer[0] = '\0';
    data = buffer;
    SetSize(0);
    SetDeletable(false);
}

// Custom String Literal
TRefString operator"" t(const char* str, size_t)
{
    return TRefString(str);
}
