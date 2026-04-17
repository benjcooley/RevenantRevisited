// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             TString.h - Modern game style string module               *
// *************************************************************************

// This is a modern non-owning string class to make string handling easier.

#pragma once

#include <cstdint>
#include <cstring>
#include <string_view>
#include <functional> // For hash
#include <algorithm>  // For std::tolower

constexpr size_t FLAG_DELETABLE = 1UL << (sizeof(size_t) * 8 - 1);
constexpr size_t MAX_SIZE = ~FLAG_DELETABLE;

class TString
{
protected:
    const char* data = nullptr;
    size_t sizeAndFlags = 0;

    void SetSize(size_t size);
    void SetDeletable(bool value);

public:
    TString();
    TString(const char* str);
    TString(const TString& other);
    TString(TString&& other) noexcept;
    TString& operator=(const TString& other);
    TString& operator=(TString&& other) noexcept;
    virtual ~TString();

    size_t GetSize() const;
    bool IsDeletable() const;
    const char* CStr() const;

    // String comparison
    int Compare(const TString& other) const;
    int CompareI(const TString& other) const; // Case-insensitive

    // String appending
    void Append(const TString& other);
    void Append(const char* str);
    void Append(char c);

    // Iterators
    const char* begin() const; // Compatible with C++ range-based for loops
    const char* end() const;

    bool operator==(const TString& other) const;

    void Print() const;

    struct Hash
    {
        size_t operator()(const TString& str) const noexcept;
    };
};

class TRefString : public TString
{
public:
    TRefString(const char* str);
};

template <size_t N = 256>
class TStackString : public TString
{
private:
    char buffer[N + 1] = {};

public:
    TStackString(const char* str);
    TStackString();
};

// Custom string literal for "xxxxx"t (TRefString)
TRefString operator"" t(const char* str, size_t);
