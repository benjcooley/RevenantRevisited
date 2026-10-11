"""Compile the actual CLI parser and framing expressions, including signed origins."""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = (ROOT / 'src/revmain.cpp').read_text()
    parser = parser[parser.index('        if (arg_param(cmd, "vfx-camera", p))'):]
    parser = parser[:parser.index('        if (arg_param(cmd, "vfx-origin", p))')]
    render = (ROOT / 'src/vfxtest.cpp').read_text()
    expressions = '\n'.join(line for line in render.splitlines()
                            if 'const float cam_ox =' in line or 'const float cam_oy =' in line)
    source = r'''
#include <string>
#include <cstdio>
#include <cassert>
float StartupVfxCamera[2]={-1,-1};
bool StartupVfxCameraExplicit=false;
bool arg_param(const std::string& cmd,const char*,std::string& p) { p=cmd; return !cmd.empty(); }
void parse(const std::string& cmd) { std::string p;
''' + parser + r'''
}
void check(int vw,int vh,float x,float y) {
''' + expressions + r'''
assert(cam_ox==x && cam_oy==y);
}
int main() {
 check(640,480,320,240); check(1920,1080,960,540);
 parse("1280,-220"); assert(StartupVfxCameraExplicit); check(1920,1080,1280,-220);
 parse("-64,100"); check(640,480,-64,100);
 parse("-1,-1"); check(640,480,-1,-1);
 parse("320,170"); check(640,480,320,170);
 parse("100"); check(640,480,320,170);
 StartupVfxCameraExplicit=false; StartupVfxCamera[0]=120; StartupVfxCamera[1]=-1;
 check(640,480,120,240);
}
'''
    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory)
        (path/'camera.cpp').write_text(source)
        subprocess.run(['c++','-std=c++17',str(path/'camera.cpp'),'-o',str(path/'camera')],check=True)
        subprocess.run([str(path/'camera')],check=True)
    print('PASS actual parser/framing: default, signed, mixed, invalid and legacy positive origins')


if __name__ == '__main__': main()
