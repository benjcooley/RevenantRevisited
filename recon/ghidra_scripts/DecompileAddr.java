// Decompile a single function. arg0=hex addr, arg1=output file path.
// If no function exists at the exact address, force-create one.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
import java.io.PrintWriter;

public class DecompileAddr extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    long addr = Long.parseLong(args[0].replace("0x",""), 16);
    String outPath = args.length>1 ? args[1] : "/tmp/ghidra_decomp.c";
    Address a = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(addr);
    Function fn = getFunctionAt(a);
    if (fn == null) {
      // Try force-creating (auto-analysis may have missed boundary)
      fn = createFunction(a, "FUN_" + Long.toHexString(addr));
      if (fn != null) println("created function at " + a);
    }
    if (fn == null) fn = getFunctionContaining(a);
    if (fn == null) { println("no function at/containing " + a); return; }
    DecompInterface d = new DecompInterface();
    d.openProgram(currentProgram);
    DecompileResults r = d.decompileFunction(fn, 60, monitor);
    try (PrintWriter w = new PrintWriter(outPath)) {
      w.println("// " + fn.getName() + " @ " + fn.getEntryPoint() +
                " size=" + fn.getBody().getNumAddresses());
      if (r != null && r.getDecompiledFunction() != null) {
        w.println(r.getDecompiledFunction().getC());
      } else {
        w.println("// decompile failed: " + (r!=null ? r.getErrorMessage() : "null"));
      }
    }
    println("wrote: " + outPath + " fn=" + fn.getName() + " @ " + fn.getEntryPoint());
  }
}
