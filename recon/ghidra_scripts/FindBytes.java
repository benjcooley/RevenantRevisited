// Find a 4-byte little-endian dword pattern in initialized memory.
// arg0 = hex address to search for
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.mem.Memory;

public class FindBytes extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    long v = Long.parseLong(args[0].replace("0x",""), 16);
    byte[] pat = new byte[]{
      (byte)(v & 0xff), (byte)((v>>8) & 0xff),
      (byte)((v>>16) & 0xff), (byte)((v>>24) & 0xff)
    };
    Memory mem = currentProgram.getMemory();
    Address found = mem.findBytes(mem.getMinAddress(), pat, null, true, monitor);
    int hits = 0;
    while (found != null) {
      println("hit @ " + found);
      hits++;
      Address next = found.add(1);
      if (next.compareTo(mem.getMaxAddress()) >= 0) break;
      found = mem.findBytes(next, pat, null, true, monitor);
    }
    println("total: " + hits);
  }
}
