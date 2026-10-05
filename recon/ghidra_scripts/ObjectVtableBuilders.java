// Name retail object vtables by their class.def builder.
// arg0.. = object vtable addresses (hex).
//
// Chain: vtable -> functions storing it (constructors) -> callers of those
// constructors (TObjectBuilder::Build overrides) -> builder vtables holding a
// Build pointer (found by scanning .rdata, which has no references without
// analysis) -> code storing that builder vtable (the REGISTER_BUILDER static
// initializer) -> the string pushed just before it (the builder name).
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.*;
import java.util.*;

public class ObjectVtableBuilders extends GhidraScript {
  Address addr(long v) { return currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(v); }

  Set<Function> functionsStoring(long value) {
    Set<Function> out = new LinkedHashSet<>();
    InstructionIterator it = currentProgram.getListing().getInstructions(true);
    while (it.hasNext()) {
      Instruction ins = it.next();
      if (!ins.getMnemonicString().equals("MOV")) continue;
      for (Object o : ins.getOpObjects(1)) {
        if (o instanceof Scalar && ((Scalar) o).getUnsignedValue() == value) {
          Function f = getFunctionContaining(ins.getAddress());
          if (f != null) out.add(f);
        }
      }
    }
    return out;
  }

  Set<Function> callers(Function f) {
    Set<Function> out = new LinkedHashSet<>();
    for (Reference r : getReferencesTo(f.getEntryPoint())) {
      if (!r.getReferenceType().isCall()) continue;
      Function c = getFunctionContaining(r.getFromAddress());
      if (c != null) out.add(c);
    }
    return out;
  }

  List<Address> pointersInRdata(long value) throws Exception {
    List<Address> out = new ArrayList<>();
    MemoryBlock rdata = currentProgram.getMemory().getBlock(".rdata");
    Memory mem = currentProgram.getMemory();
    for (Address a = rdata.getStart(); a.compareTo(rdata.getEnd().subtract(4)) < 0; a = a.add(4)) {
      if ((mem.getInt(a) & 0xffffffffL) == value) out.add(a);
    }
    return out;
  }

  String pushedStringBefore(Instruction store) throws Exception {
    Instruction ins = store;
    for (int i = 0; i < 8 && ins != null; i++) {
      ins = ins.getPrevious();
      if (ins == null) break;
      if (ins.getMnemonicString().equals("PUSH")) {
        for (Object o : ins.getOpObjects(0)) {
          long v = -1;
          if (o instanceof Scalar) v = ((Scalar) o).getUnsignedValue();
          else if (o instanceof Address) v = ((Address) o).getOffset();
          if (v > 0) {
            try {
              byte[] b = new byte[48];
              currentProgram.getMemory().getBytes(addr(v), b);
              int n = 0; while (n < b.length && b[n] >= 0x20 && b[n] < 0x7f) n++;
              if (n >= 2 && n < b.length && b[n] == 0) return new String(b, 0, n);
            } catch (Exception e) {}
          }
        }
      }
    }
    return null;
  }

  @Override public void run() throws Exception {
    for (String tok : getScriptArgs()) {
      long vt = Long.parseLong(tok.replace("0x", ""), 16);
      Set<String> names = new TreeSet<>();
      Set<String> builds = new TreeSet<>();
      for (Function ctor : functionsStoring(vt)) {
        for (Function build : callers(ctor)) {
          for (Address slot : pointersInRdata(build.getEntryPoint().getOffset())) {
            // the builder vtable starts at most 2 slots before this Build pointer
            for (int back = 0; back <= 8; back += 4) {
              long bvt = slot.getOffset() - back;
              InstructionIterator it = currentProgram.getListing().getInstructions(true);
              while (it.hasNext()) {
                Instruction ins = it.next();
                if (!ins.getMnemonicString().equals("MOV")) continue;
                boolean hit = false;
                for (Object o : ins.getOpObjects(1))
                  if (o instanceof Scalar && ((Scalar) o).getUnsignedValue() == bvt) hit = true;
                if (!hit) continue;
                String s = pushedStringBefore(ins);
                if (s != null) { names.add(s); builds.add(build.getEntryPoint().toString()); }
              }
            }
          }
        }
      }
      println(String.format("VT %08x builders=%s via=%s", vt, names, builds));
    }
  }
}
