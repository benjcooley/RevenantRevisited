// List functions whose entry point is in [arg0, arg1] hex range.
// Output: addr  size  name
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class FnsInRange extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    long lo = Long.parseLong(args[0].replace("0x",""), 16);
    long hi = Long.parseLong(args[1].replace("0x",""), 16);
    int hits = 0;
    for (Function fn : currentProgram.getFunctionManager().getFunctions(true)) {
      long e = fn.getEntryPoint().getOffset();
      if (e >= lo && e <= hi) {
        long size = fn.getBody().getNumAddresses();
        println(String.format("%08x  size=%-5d  %s", e, size, fn.getName()));
        hits++;
      }
    }
    println("hits: " + hits);
  }
}
