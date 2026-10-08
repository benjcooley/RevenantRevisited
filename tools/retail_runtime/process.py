"""Explicit process startup inputs for a headless Win32 scenario."""
import struct


class Process:
    def __init__(self,vm):
        self.vm=vm
        self.command_line='C:\\REVENANT\\Revenant.exe'
        self.command_pointer=vm.allocate(4096)
        vm.write(self.command_pointer,self.command_line.encode('cp1252')+b'\0')
        self.environment_pointer=vm.allocate(4096)
        vm.write(self.environment_pointer,b'\0\0')
        self.standard={0xfffffff6:0xfffff001,0xfffffff5:0xfffff002,0xfffffff4:0xfffff003}
        for kind,handle in self.standard.items():
            name={0xfffffff6:'$stdin',0xfffffff5:'$stdout',0xfffffff4:'$stderr'}[kind]
            vm.files[name]=bytearray();vm.handles[handle]=[name,0]

    def startup_info(self,args):
        self.vm.write(args[0],struct.pack('<17I',68,*([0]*16)));return 0

    def std_handle(self,args):return self.standard.get(args[0],0xffffffff)
    def file_type(self,args):return 2 if args[0] in self.standard.values() else (1 if args[0] in self.vm.handles else 0)
    def module(self,args):
        if not args[0]:return self.vm.base
        name=self.vm.string(args[0]).replace('\\','/').split('/')[-1].lower()
        if name=='revenant.exe':return self.vm.base
        self.vm.scheduler.set_error((126,));return 0

    def environment(self,args):return self.environment_pointer
    def free_environment(self,args):return int(args[0]==self.environment_pointer)
    def handle_count(self,args):
        self.vm.file_handle_limit=max(20,args[0]);return self.vm.file_handle_limit

    def wide_to_bytes(self,args):
        page,flags,source,count,dest,capacity,default,used_default=args
        if page not in (0,1252) or flags:raise ValueError('Only unflagged CP1252 conversion is supported')
        if count==0xffffffff:
            raw=bytearray()
            for i in range(32768):
                word=bytes(self.vm.uc.mem_read(source+2*i,2));raw+=word
                if word==b'\0\0':break
            else:raise ValueError('Unterminated UTF16 string')
        elif 0<count<=32768:raw=bytes(self.vm.uc.mem_read(source,count*2))
        else:raise ValueError('Invalid UTF16 input length')
        # Reject unimplemented best-fit/default substitutions instead of
        # silently changing the reference text.
        data=raw.decode('utf-16-le').encode('cp1252')
        if not capacity:return len(data)
        if capacity<len(data):self.vm.scheduler.set_error((122,));return 0
        self.vm.write(dest,data)
        if used_default:self.vm.put_u32(used_default,0)
        return len(data)

    def cp_info(self,args):
        page,pointer=args
        if page not in (0,1252):raise ValueError('Only CP1252 character metadata is supported')
        self.vm.write(pointer,struct.pack('<I',1)+b'?\0'+bytes(14));return 1
