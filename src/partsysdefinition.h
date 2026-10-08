#pragma once

#include <array>
#include <string>
#include <string_view>
#include <vector>

// Immutable, asset-independent metadata for the retail partsys controller.
// Object names resolve against the owning imagery, never a guessed fallback.
namespace authored_partsys {
using Value = std::array<float, 3>;
struct Key {
    float percent = 0;
    Value value{};
};
struct Expression {
    int dimensions = 1;
    Value constant{};
    std::vector<Key> keys;
    bool IsCurve() const { return !keys.empty(); }
    // Retail 0x401390 takes an integer parameter; callers choose animation
    // frame or particle lifetime percent according to the recovered field.
    Value Evaluate(int parameter) const;
};
struct Definition {
    std::vector<std::string> objects;
    std::string particle;
    Expression pps;
    Expression initialvelocity{2};
    Expression lifespan{2};
    Expression globalrotation{3};
    Expression localrotation{3};
    Expression rglobalrotation{3};
    Expression rlocalrotation{3};
    Expression friction;
    Expression gravity;
    Expression color{3};
    Expression alpha{1, {1, 0, 0}};
    Expression spread;
    Expression azimuth;
    Expression relvel;
    Expression charvel{1, {-1000, 0, 0}};
    Expression scale{1, {1, 0, 0}};
    Expression bounce{2, {100, 100, 0}};
    int emittertype = 0; // sphere=0,cube=1,circle=2,square=3
    int blendmode = 16; // literal retail Initialize: litadd
    int emittersize = 0;
};

// Supported subset is the four shipped WFall/WCap tags and the scalar/vector
// grammar recovered from ParseItem 0x4042e0 and expression parser 0x4010d0.
// Output is unchanged on failure. Unknown fields are explicitly unsupported.
bool ParseDefinition(std::string_view tag, Definition& output,
                     std::string& diagnostic);
} // namespace authored_partsys
