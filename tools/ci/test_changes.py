"""Exercise CI selection against real commit histories, including later pushes."""

import subprocess
import tempfile
import unittest
from pathlib import Path

from changes import cpp_changed


class ChangedFilesTests(unittest.TestCase):
    def setUp(self):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.root = Path(directory.name)
        self.git("init", "-b", "main")
        self.git("config", "user.name", "CI test")
        self.git("config", "user.email", "ci@example.invalid")
        self.git("config", "commit.gpgsign", "false")
        self.git("config", "core.hooksPath", str(self.root / "no-hooks"))
        self.base = self.commit("README.md")
        self.git("update-ref", "refs/remotes/origin/main", self.base)
        self.git("checkout", "-b", "feature")

    def git(self, *args):
        return subprocess.run(["git", *args], cwd=self.root, capture_output=True,
                              text=True, check=True).stdout.strip()

    def commit(self, path):
        target = self.root / path
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(path + "\n", encoding="utf-8")
        self.git("add", "--", path)
        self.git("commit", "-m", "fixture")
        return self.git("rev-parse", "HEAD")

    def selected(self, *, ref="refs/heads/feature", before=None, event_name="push"):
        return cpp_changed(self.root, event_name, {
            "repository": {"default_branch": "main"}, "before": before or self.base,
        }, ref)

    def test_rust_docs_and_rust_workflow_do_not_build_cpp(self):
        self.commit("rust/src/main.rs")
        self.commit("docs/design.md")
        self.commit(".github/workflows/rust.yml")
        self.assertFalse(self.selected())

    def test_cpp_sources_and_build_inputs_select_cpp(self):
        for path in ("src/platform/windows/render.cpp", "thirdparty/lib/api.h",
                     "assets/effects.def", "cmake/BuildInfo.cmake", "tools/test_mesh.cpp",
                     "tools/ci/ci.py", "CMakeLists.txt", "editor.def", ".gitattributes",
                     ".github/workflows/ci.yml"):
            with self.subTest(path=path):
                self.git("checkout", "-B", "fixture", self.base)
                self.commit(path)
                self.assertTrue(self.selected())

    def test_cpp_change_survives_a_rust_only_followup_push(self):
        previous = self.commit("src/engine/render/renderer.cpp")
        self.commit("rust/src/main.rs")
        self.assertTrue(self.selected(before=previous))

    def test_main_only_compares_the_current_push(self):
        previous = self.commit("src/engine/render/renderer.cpp")
        self.commit("rust/src/main.rs")
        self.assertFalse(self.selected(ref="refs/heads/main", before=previous))
        previous = self.git("rev-parse", "HEAD")
        self.commit("src/camera.cpp")
        self.assertTrue(self.selected(ref="refs/heads/main", before=previous))

    def test_unrelated_new_main_changes_do_not_select_feature_build(self):
        self.git("checkout", "main")
        current_main = self.commit("src/engine/render/renderer.cpp")
        self.git("update-ref", "refs/remotes/origin/main", current_main)
        self.git("checkout", "feature")
        self.commit("rust/src/main.rs")
        self.assertFalse(self.selected())

    def test_cpp_removal_and_rename_out_of_src_select_build(self):
        base = self.commit("src/engine/render/renderer.cpp")
        self.git("update-ref", "refs/remotes/origin/main", base)
        self.git("mv", "src/engine/render/renderer.cpp", "notes.txt")
        self.git("commit", "-m", "rename fixture")
        self.assertTrue(self.selected())
        self.git("checkout", "-B", "deletion", base)
        self.git("rm", "src/engine/render/renderer.cpp")
        self.git("commit", "-m", "delete fixture")
        self.assertTrue(self.selected())

    def test_manual_new_main_and_missing_history_build_conservatively(self):
        self.assertTrue(self.selected(event_name="workflow_dispatch"))
        self.assertTrue(self.selected(ref="refs/heads/main", before="0" * 40))
        self.assertTrue(self.selected(ref="refs/heads/main", before="f" * 40))
        self.git("update-ref", "-d", "refs/remotes/origin/main")
        self.assertTrue(self.selected())


if __name__ == "__main__":
    unittest.main()
