#pragma once
#include "vfxtest.h"
#include <memory>
#include <optional>

struct VfxReviewPreviewInfo
{
    bool supported = false;
    const char* preview_id = "";
    const char* description = "No implemented preview mapping; label only";
    bool requires_endpoints = false;
    bool requires_attachment = false;
    bool lifetime_known = false;
};

// Exact retail-type mappings come from the source ledger; no runtime JSON.
// This fallback does not certify a preview's retail fidelity or caller context.
VfxReviewPreviewInfo DescribeVfxReviewPreview(uint32_t type_id);

class VfxReviewPreview
{
  public:
    VfxReviewPreview(const VfxTest::SEffect& effect, void* context, const S3DPoint& origin);
    ~VfxReviewPreview();
    VfxReviewPreview(const VfxReviewPreview&) = delete;
    VfxReviewPreview& operator=(const VfxReviewPreview&) = delete;
    const std::string& Id() const { return effect_.id; }
    void Submit();
    void SubmitWorld();
    // nullopt: no completion contract; do not destroy on a guessed timer.
    std::optional<bool> Alive() const;
    float RestartInterval() const;
  private:
    VfxTest::SEffect effect_;
    void* context_ = nullptr;
    S3DPoint origin_{};
    float restart_remaining_ = 0;
};

// Null if no exact mapping, unsupported endpoint/attachment contract, or a
// rejected factory. Destroy before shutting down the map/renderer resources.
std::unique_ptr<VfxReviewPreview> CreateVfxReviewPreview(uint32_t type_id,
                                                       const S3DPoint& origin);
