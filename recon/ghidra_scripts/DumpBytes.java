import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;

public class DumpBytes extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    long addr = Long.parseLong(args[0].replace("0x",""), 16);
    int n = args.length > 1 ? Integer.parseInt(args[1]) : 16;
    Address a = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(addr);
    byte[] buf = new byte[n];
    currentProgram.getMemory().getBytes(a, buf);
    StringBuilder sb = new StringBuilder();
    for (int i = 0; i < n; i++) sb.append(String.format("%02x ", buf[i] & 0xff));
    println(String.format("%08x: %s", addr, sb.toString()));
    StringBuilder ascii = new StringBuilder();
    for (int i = 0; i < n; i++) {
      int c = buf[i] & 0xff;
      ascii.append((c >= 32 && c < 127) ? (char)c : '.');
    }
    println(String.format("%08x: %s", addr, ascii.toString()));
  }
}
