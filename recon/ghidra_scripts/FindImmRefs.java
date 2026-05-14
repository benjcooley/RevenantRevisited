// Find instructions whose operand is a given address (catches string xrefs
// even when auto-analysis didn't promote them to Data references).
// arg0 = hex addr (e.g. 0x5df78c)
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.lang.OperandType;

public class FindImmRefs extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    long target = Long.parseLong(args[0].replace("0x",""), 16);
    InstructionIterator it = currentProgram.getListing().getInstructions(true);
    int hits = 0;
    while (it.hasNext()) {
      Instruction ins = it.next();
      int n = ins.getNumOperands();
      for (int i = 0; i < n; i++) {
        Object[] obs = ins.getOpObjects(i);
        for (Object o : obs) {
          long v = -1;
          if (o instanceof Scalar) v = ((Scalar)o).getUnsignedValue();
          else if (o instanceof Address) v = ((Address)o).getOffset();
          if (v == target) {
            Function fn = getFunctionContaining(ins.getAddress());
            println(String.format("%s  %-20s  fn=%s",
              ins.getAddress(), ins.toString(),
              fn==null ? "<none>" : fn.getName()+"@"+fn.getEntryPoint()));
            hits++;
          }
        }
      }
    }
    println("total hits: " + hits);
  }
}
