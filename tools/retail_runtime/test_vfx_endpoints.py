"""Actual endpoint parser and preview dispatch, including rejected fallbacks."""
import subprocess
import tempfile
import unittest
from pathlib import Path
from test_vfxreviewspawn import ROOT, function

class EndpointTests(unittest.TestCase):
 def test_exact_coordinates_and_actual_dispatch(self):
  actual=function((ROOT/'src/vfxtest.cpp').read_text(),'void* CreatePreviewContext(')
  code=r'''
#include "vfxpointoption.h"
#include <cassert>
#include <functional>
#include <stdexcept>
#include <string>
struct S3DPoint{int x,y,z;};
namespace VfxTest{struct SEffect{std::string id="test";std::function<void*(const S3DPoint&)>factory;std::function<void*(const S3DPoint&,const S3DPoint&)>factory_endpoints;};}
bool StartupVfxTargetSet=false;int32_t StartupVfxTarget[3]={-170,-219,144};
[[noreturn]]void FatalError(const char*error,void*){throw std::runtime_error(error);}
void log_info(const char*,...){}
'''+actual+r'''
int main(){
 int32_t p[3]={1,2,3};assert(ParseVfxPointOption("-2147483648,2147483647,0",p));assert(p[0]==INT32_MIN&&p[1]==INT32_MAX&&p[2]==0);
 for(auto text:{"","1,2","1,2,3,4","1,2,","2147483648,0,0","-2147483649,0,0","1,2,3junk","1.5,2,3","99999999999999999999999,0,0"}){p[0]=1;p[1]=2;p[2]=3;assert(!ParseVfxPointOption(text,p));assert(p[0]==1&&p[1]==2&&p[2]==3);}
 VfxTest::SEffect e;int one=0,two=0;int context=42;
 e.factory=[&](const S3DPoint&){++one;return &context;};
 e.factory_endpoints=[&](const S3DPoint&s,const S3DPoint&t){++two;assert(s.x==0&&s.y==0&&s.z==144&&t.x==-170&&t.y==-219&&t.z==144);return &context;};
 const S3DPoint source{0,0,144};assert(CreatePreviewContext(e,source)==&context&&one==1&&two==0);
 StartupVfxTargetSet=true;assert(CreatePreviewContext(e,source)==&context&&one==1&&two==1);assert(CreatePreviewContext(e,source)==&context&&two==2);
 auto rejects=[&]{try{CreatePreviewContext(e,source);assert(false);}catch(const std::runtime_error&){}assert(one==1);};
 e.factory_endpoints={};rejects();e.factory_endpoints=[](const S3DPoint&,const S3DPoint&)->void*{return nullptr;};rejects();StartupVfxTarget[0]=0;StartupVfxTarget[1]=0;rejects();
}
'''
  with tempfile.TemporaryDirectory(prefix='vfx-endpoint-')as tmp:
   p=Path(tmp);(p/'check.cpp').write_text(code)
   subprocess.run(['clang++','-std=c++17','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(p/'check.cpp'),'-o',str(p/'check')],check=True)
   subprocess.run([str(p/'check')],check=True)

if __name__=='__main__':unittest.main()
