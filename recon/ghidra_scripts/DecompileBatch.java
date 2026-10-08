// Decompile many functions in one headless session.
// arg0 = path to a list file; each non-blank, non-# line is "<hex addr> <output path>".
// Pass 1 force-creates a function at every listed address that lacks one (so
// cross-calls between listed LAB_ entries resolve as calls in pass 2); pass 2
// decompiles each and writes it to its output path (same header format as
// DecompileAddr.java). With -readOnly the created functions are discarded at
// exit, so re-runs are idempotent.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

public class DecompileBatch extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    List<String[]> jobs = new ArrayList<>();
    for (String line : Files.readAllLines(Paths.get(args[0]))) {
      line = line.trim();
      if (line.isEmpty() || line.startsWith("#")) continue;
      String[] parts = line.split("\\s+", 2);
      if (parts.length == 2) jobs.add(parts);
    }
    List<Address> addrs = new ArrayList<>();
    for (String[] j : jobs) {
      long v = Long.parseLong(j[0].replace("0x", ""), 16);
      Address a = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(v);
      addrs.add(a);
      if (getFunctionAt(a) == null) {
        Function f = createFunction(a, "FUN_" + String.format("%08x", v));
        if (f != null) println("created function at " + a);
      }
    }
    DecompInterface d = new DecompInterface();
    d.openProgram(currentProgram);
    for (int i = 0; i < jobs.size(); i++) {
      Address a = addrs.get(i);
      String outPath = jobs.get(i)[1];
      Function fn = getFunctionAt(a);
      if (fn == null) fn = getFunctionContaining(a);
      if (fn == null) { println("no function at/containing " + a); continue; }
      DecompileResults r = d.decompileFunction(fn, 60, monitor);
      try (PrintWriter w = new PrintWriter(outPath)) {
        w.println("// " + fn.getName() + " @ " + fn.getEntryPoint() +
                  " size=" + fn.getBody().getNumAddresses());
        if (r != null && r.getDecompiledFunction() != null) {
          w.println(r.getDecompiledFunction().getC());
        } else {
          w.println("// decompile failed: " + (r != null ? r.getErrorMessage() : "null"));
        }
      }
      println("wrote: " + outPath + " fn=" + fn.getName() + " @ " + fn.getEntryPoint());
    }
  }
}
