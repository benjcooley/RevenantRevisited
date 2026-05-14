// Read a vtable as a sequence of dword pointers and report each slot's
// function. arg0 = vtable address, arg1 = max slots (default 32)
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.mem.Memory;

public class DumpVtable extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    long addr = Long.parseLong(args[0].replace("0x",""), 16);
    int slots = args.length > 1 ? Integer.parseInt(args[1]) : 32;
    Address a = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(addr);
    Memory mem = currentProgram.getMemory();
    println("vtable @ " + a);
    for (int i = 0; i < slots; i++) {
      Address slot = a.add(i * 4L);
      int p = mem.getInt(slot);
      long off = ((long)p) & 0xffffffffL;
      Address fa = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(off);
      Function fn = getFunctionAt(fa);
      println(String.format("  [%2d] @ %s  -> %08x  %s",
        i, slot, off, fn==null ? "<no fn>" : fn.getName()+"@"+fn.getEntryPoint()));
    }
  }
}
