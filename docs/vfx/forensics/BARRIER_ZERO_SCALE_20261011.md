# BarrierEW/NS original flare rejection

The previous legacy fixture produces a static wall even though the custom
controller continues updating its30 flare slots. This is not an empty pool or
missing texture: original Render sends active objects2/4 to RenderObject with
flags0x48 and a literal scaleY0. The original CalcObjectMatrix returns0 for those
flare draws, while object0's wall matrix succeeds.

`barrier_zero_scale_probe.py` observes both calls and their actual return values
for90 complete custom Animate/Render steps for each exact EW/NS factory and
asset. Every active flare has scaleY0 and is rejected before the software draw
dispatcher; exactly one wall draw remains. No matrix, vertex, scale, or color is
replaced. The full native wall reference has **12,914 visible pixels** for both
identities at every sampled phase0/1/6/16/30/60/90.

The native resource provider executes original SetupObjects, CopyVertices,
authored key-cache refresh and texture/material COM access. Null owner command
and script interfaces are explicit boundaries. A production state/RNG and full
wall-render comparison is still needed before advancing either primary row;
this diagnostic grants no appearance credit and does not disable zero-scale
admission globally.

Private evidence:
`RevenantRetailLab/research/vfx-next-barrier-20261011/native-current/manifest.json`
and `zero-scale-proof/manifest.json`. The latter pins90 actual matrix-return
traces per identity. Reproduce with:

```sh
python tools/retail_runtime/barrier_zero_scale_probe.py --output <new-directory>
```

Further work should reproduce the complete pool, Render-order UV accumulation,
wall geometry/materials, actual owner command handshake and lifetime. Natural
map/scripting/device behavior remains separate; inventing visible flares would
contradict the executed software reference.
