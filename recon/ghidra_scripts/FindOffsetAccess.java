// Finds instructions that reference a given struct offset (e.g. [reg+0xb4])
// Usage: FindOffsetAccess.java <offset_hex>  -- searches whole program.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.address.Address;

public class FindOffsetAccess extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    long off = Long.parseLong(args[0].replace("0x",""), 16);
    String pat1 = String.format("+ 0x%x]", off);
    String pat2 = String.format("+ 0x%X]", off);
    Listing lst = currentProgram.getListing();
    InstructionIterator it = lst.getInstructions(true);
    int hits = 0;
    while (it.hasNext()) {
      Instruction ins = it.next();
      String s = ins.toString();
      if (s.contains(pat1) || s.contains(pat2)) {
        Function fn = getFunctionContaining(ins.getAddress());
        println(String.format("  %s  %s  (%s)", ins.getAddress(), s,
          fn == null ? "<none>" : fn.getName()+"@"+fn.getEntryPoint()));
        if (++hits > 5000) { println("... (truncated)"); break; }
      }
    }
    println("hits: " + hits);
  }
}
