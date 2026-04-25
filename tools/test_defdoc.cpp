// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Reveneant Revisited 2026                         *
// *                  test_defdoc.cpp - DEF Parser Tests                    *
// *************************************************************************

#include "../src/defdoc.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static void test_area_style()
{
    const char* text = R"DEF(
// Revenant-ish legacy DEF
AREA "The Forest of Ahkuilon"
BEGIN
  LEVEL 10
  AMBLIGHT 30
  AMBCOLOR 250, 250, 250
  NIGHTAMBCOLOR 75, 100, 250
  SCRIPT "demo.s"
  AUDIOENV 15 // EAX_ENVIRONMENT_FOREST
END

Area "House Interiors"
BEGIN
  LEVEL 12
  AMBLIGHT 4
  SCRIPT "demo.s"
END
)DEF";

    defdoc::Document doc = defdoc::Document::parse(text);
    auto areas = doc.root().blocks("area");
    assert(areas.size() == 2);
    assert((*areas[0])["$name"].as_string() == "The Forest of Ahkuilon");
    assert(areas[0]->get_int("LEVEL") == 10);
    assert(areas[0]->get_int("AMBLIGHT") == 30);
    assert((*areas[0])["AMBCOLOR"].is_array());
    assert((*areas[0])["AMBCOLOR"].as_array().size() == 3);
    assert((*areas[0])["AMBCOLOR"].as_array()[0].as_int() == 250);
    assert(areas[0]->get_string("SCRIPT") == "demo.s");
    assert(areas[1]->get_int("level") == 12);

    const std::string out = doc.to_def();
    defdoc::Document reparsed = defdoc::Document::parse(out);
    assert(reparsed.root().blocks("AREA").size() == 2);
}

static void test_policy_style()
{
    const char* text = R"DEF(
RENDERPOLICY "Water" {
  pass = "transparent_world"
  blend = additive
  z_test = true
  z_write = false
  color = [0.2, 0.5, 1.0]
  modes = ["alpha", "additive"]
}

RENDERPOLICY "HelperBox" {
  submesh = { name = "rectangle0", blend = "alpha", shadow = true }
}
)DEF";

    defdoc::Document doc = defdoc::Document::parse(text);
    auto policies = doc.root().blocks("renderpolicy");
    assert(policies.size() == 2);
    assert((*policies[0])["$name"].as_string() == "Water");
    assert(policies[0]->get_string("blend") == "additive");
    assert(policies[0]->get_bool("z_test") == true);
    assert(policies[0]->get_bool("z_write") == false);
    assert((*policies[0])["color"].as_array().size() == 3);
    assert((*policies[0])["color"].as_array()[1].as_double() == 0.5);
    assert((*policies[1])["submesh"].is_object());
    assert((*policies[1])["submesh"].get_bool("shadow") == true);

    const std::string out = doc.to_def();
    assert(out.find("modes = [alpha, additive]") != std::string::npos);
    assert(out.find("submesh = {") != std::string::npos);
    defdoc::Document reparsed = defdoc::Document::parse(out);
    auto reparsed_policies = reparsed.root().blocks("renderpolicy");
    assert(reparsed_policies.size() == 2);
    assert((*reparsed_policies[1])["submesh"].get_bool("shadow") == true);
}

static void test_writer_heuristic()
{
    defdoc::Document doc;
    auto& root = doc.root();

    defdoc::Node small(defdoc::Node::Object{});
    small["items"] = defdoc::Node::Array{
        defdoc::Node::Object{{{"name", "a"}, {"mode", "alpha"}}},
        defdoc::Node::Object{{{"name", "b"}, {"mode", "additive"}}},
    };

    defdoc::Node big_child(defdoc::Node::Object{});
    big_child["$name"] = "rectangle0";
    big_child["blend"] = "alpha";
    big_child["lighting"] = "unlit_shadow";

    defdoc::Node parent(defdoc::Node::Object{});
    parent["small"] = small;
    parent["SUBMESH"] = defdoc::Node::Array{big_child};
    root["POLICY"] = defdoc::Node::Array{parent};

    const std::string out = doc.to_def();
    assert(out.find("items = [") != std::string::npos);
    assert(out.find("{ mode = alpha, name = a }") != std::string::npos);
    assert(out.find("{ mode = additive, name = b }") != std::string::npos);
    assert(out.find("SUBMESH rectangle0") != std::string::npos);
    assert(out.find("BEGIN") != std::string::npos);
}

static void test_structured_array_roundtrip_forms()
{
    const char* text = R"DEF(
POLICY "Water"
BEGIN
  inline_items = [
    { name "a", blend "alpha" },
    { name = "b", blend = "additive" }
  ]

  SUBMESH "foam"
  BEGIN
    BLEND additive
    ZWRITE false
  END
END
)DEF";

    defdoc::Document doc = defdoc::Document::parse(text);
    auto policies = doc.root().blocks("policy");
    assert(policies.size() == 1);
    const defdoc::Node& policy = *policies[0];
    assert(policy["inline_items"].is_array());
    assert(policy["inline_items"].as_array().size() == 2);
    assert(policy["inline_items"].as_array()[0].get_string("name") == "a");
    assert(policy["inline_items"].as_array()[1].get_string("blend") == "additive");
    assert(policy.blocks("submesh").size() == 1);
    assert(policy.blocks("submesh")[0]->get_bool("ZWRITE", true) == false);

    const std::string out = doc.to_def();
    assert(out.find("inline_items = [") != std::string::npos);
    assert(out.find("{ blend = alpha, name = a }") != std::string::npos);
    assert(out.find("SUBMESH foam") != std::string::npos);
    assert(out.find("ZWRITE = false") != std::string::npos);

    defdoc::Document reparsed = defdoc::Document::parse(out);
    auto reparsed_policies = reparsed.root().blocks("POLICY");
    assert(reparsed_policies.size() == 1);
    assert((*reparsed_policies[0])["inline_items"].as_array()[0].get_string("blend") == "alpha");
    assert(reparsed_policies[0]->blocks("SUBMESH")[0]->get_string("BLEND") == "additive");
}

static void test_writer_path_options()
{
    const char* text = R"DEF(
POLICY "Water"
BEGIN
  inline_items = [
    { name "a", blend "alpha" },
    { name "b", blend "additive" }
  ]
  settings = { pass transparent_world, z_write false, lighting fullbright }
END
)DEF";

    defdoc::Document doc = defdoc::Document::parse(text);

    defdoc::WriteOptions force_blocks;
    force_blocks.force_block("POLICY.inline_items")
                .force_block("POLICY.settings");
    const std::string blocky = doc.to_def(force_blocks);
    assert(blocky.find("inline_items") != std::string::npos);
    assert(blocky.find("settings\n") != std::string::npos);
    assert(blocky.find("BEGIN") != std::string::npos);

    defdoc::WriteOptions force_inline;
    force_inline.force_inline("POLICY.inline_items")
                .force_inline("POLICY.settings");
    const std::string compact = doc.to_def(force_inline);
    assert(compact.find("inline_items = [") != std::string::npos);
    assert(compact.find("settings = {") != std::string::npos);

    defdoc::WriteOptions classic;
    classic.force_inline("POLICY.inline_items")
           .force_inline("POLICY.settings")
           .assign_space("POLICY.inline_items")
           .assign_space("POLICY.settings")
           .assign_space("POLICY.settings.pass")
           .assign_space("POLICY.settings.z_write")
           .assign_space("POLICY.settings.lighting");
    const std::string classic_out = doc.to_def(classic);
    assert(classic_out.find("inline_items [") != std::string::npos);
    assert(classic_out.find("settings {") != std::string::npos);
    assert(classic_out.find("pass transparent_world") != std::string::npos);
    assert(classic_out.find("z_write false") != std::string::npos);

    defdoc::WriteOptions mixed;
    mixed.use_space_assignment()
         .assign_equals("POLICY.settings")
         .assign_equals("POLICY.settings.pass");
    const std::string mixed_out = doc.to_def(mixed);
    assert(mixed_out.find("settings = ") != std::string::npos);
    assert(mixed_out.find("pass = transparent_world") != std::string::npos);
}

static void test_comment_roundtrip()
{
    const char* text = R"DEF(// Area list
AREA "Forest"
BEGIN
  // Main level id
  LEVEL = 10

  // Ambient color
  AMBCOLOR = [250, 250, 250]
END

// Render policy block
RENDERPOLICY "Water"
BEGIN
  // Use additive legacy water
  BLEND = additive
  ZWRITE = false
END
)DEF";

    defdoc::Document doc = defdoc::Document::parse(text);

    const auto* area_meta = doc.find_metadata("AREA:Forest");
    assert(area_meta);
    assert(area_meta->leading_comments.size() == 1);
    assert(area_meta->leading_comments[0] == "Area list");

    const auto* level_meta = doc.find_metadata("AREA:Forest.LEVEL");
    assert(level_meta);
    assert(level_meta->leading_comments.size() == 1);
    assert(level_meta->leading_comments[0] == "Main level id");

    const auto* amb_meta = doc.find_metadata("AREA:Forest.AMBCOLOR");
    assert(amb_meta);
    assert(amb_meta->leading_blank_lines == 1);
    assert(amb_meta->leading_comments.size() == 1);
    assert(amb_meta->leading_comments[0] == "Ambient color");

    // Metadata is out-of-band and keyed by semantic statements/blocks, not by
    // scalar value nodes or array element ids.
    assert(!doc.root().blocks("AREA")[0]->contains("__meta"));
    assert(!doc.find_metadata("AREA:Forest.LEVEL.value"));
    assert(!doc.find_metadata("AREA:Forest.AMBCOLOR[0]"));

    const std::string first = doc.to_def();
    defdoc::Document reparsed = defdoc::Document::parse(first);
    const std::string second = reparsed.to_def();
    assert(first == second);
    assert(first.find("// Area list\nAREA Forest") != std::string::npos);
    assert(first.find("  // Main level id\n  LEVEL = 10") != std::string::npos);
    assert(first.find("\n\n  // Ambient color\n  AMBCOLOR = [250, 250, 250]") != std::string::npos);
    assert(first.find("// Render policy block\nRENDERPOLICY Water") != std::string::npos);
}

static void test_parse_without_comments()
{
    const char* text = R"DEF(// Ignored top comment
AREA "Forest"
BEGIN
  // Ignored field comment
  LEVEL = 10
END
)DEF";

    defdoc::ParseOptions options;
    options.preserve_comments = false;
    defdoc::Document doc = defdoc::Document::parse(text, options);
    auto areas = doc.root().blocks("area");
    assert(areas.size() == 1);
    assert((*areas[0])["$name"].as_string() == "Forest");
    assert(areas[0]->get_int("level") == 10);
    assert(doc.metadata().empty());

    const std::string out = doc.to_def();
    assert(out.find("//") == std::string::npos);
}

static void test_existing_defs_parse()
{
    namespace fs = std::filesystem;
    const fs::path cwd = fs::current_path();
    std::vector<fs::path> roots = {
        cwd / ".." / "data",
        fs::path("/tmp/revenant_archive_dump"),
    };

    defdoc::ParseOptions options;
    options.preserve_comments = false;

    int parsed = 0;
    std::vector<std::string> failures;
    for (const fs::path& root : roots)
    {
        std::error_code ec;
        if (!fs::exists(root, ec))
            continue;
        for (fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, ec), end;
             it != end && !ec; it.increment(ec))
        {
            if (!it->is_regular_file(ec))
                continue;
            const std::string ext = it->path().extension().string();
            if (!(ext == ".def" || ext == ".DEF"))
                continue;
            std::ifstream f(it->path());
            if (!f)
            {
                failures.push_back(it->path().string() + ": open failed");
                continue;
            }
            std::ostringstream ss;
            ss << f.rdbuf();
            try
            {
                (void)defdoc::Document::parse(ss.str(), options);
                ++parsed;
            }
            catch (const std::exception& e)
            {
                failures.push_back(it->path().string() + ": " + e.what());
            }
        }
    }

    if (!failures.empty())
    {
        std::cerr << "DEF parse failures:\n";
        for (const std::string& failure : failures)
            std::cerr << "  " << failure << "\n";
    }
    assert(failures.empty());
    assert(parsed > 0);
}

int main()
{
    test_area_style();
    test_policy_style();
    test_writer_heuristic();
    test_structured_array_roundtrip_forms();
    test_writer_path_options();
    test_comment_roundtrip();
    test_parse_without_comments();
    test_existing_defs_parse();
    std::cout << "defdoc tests passed\n";
    return 0;
}
