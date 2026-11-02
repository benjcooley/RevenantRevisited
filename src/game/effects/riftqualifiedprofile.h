#pragma once
#include <cctype>
#include <cstring>
#include <cstdint>
#include <string>
namespace rift_qualified_profile{
inline uint64_t Hash(const void*p,size_t n){uint64_t h=1469598103934665603ull;for(auto*b=static_cast<const uint8_t*>(p);n--;){h^=*b++;h*=1099511628211ull;}return h;}
inline bool Matches(const char*filename,const void*body,size_t size,int states,int frames,int aflags){
 if(!filename||!body||size!=33329||states!=1||frames!=100||aflags!=0x2000)return false;std::string path(filename);for(char&c:path)c=c=='\\'?'/' :char(std::tolower(static_cast<unsigned char>(c)));constexpr auto suffix="misc/rift1.i3d";const size_t n=std::strlen(suffix);if(path.size()<n||path.compare(path.size()-n,n,suffix)||(path.size()!=n&&path[path.size()-n-1]!='/'))return false;
 return Hash(body,size)==0x2e10d5f8f21ad4a7ull;
}
}
