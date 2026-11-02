#pragma once

#include <cctype>
#include <string>
#include <unordered_map>

// Retail archives contain duplicate basenames in different directories.
// Keep their full paths distinct; retain a basename alias only when unique.
inline std::string VfsPathKey(const char* path)
{
    std::string key;
    std::string component;
    const auto append = [&]() {
        if (!component.empty() && component != ".") {
            if (!key.empty()) key += '/';
            key += component;
        }
        component.clear();
    };
    for (const unsigned char* p = reinterpret_cast<const unsigned char*>(path); p && *p; ++p) {
        if (*p == '/' || *p == '\\') append();
        else component += static_cast<char>(std::tolower(*p));
    }
    append();
    return key;
}

template <typename Entry>
class TVfsPathIndex
{
public:
    void Add(const char* path, const Entry& entry)
    {
        const std::string key = VfsPathKey(path);
        if (!paths_.try_emplace(key, entry).second) return; // first exact path wins
        const std::string basename = key.substr(key.find_last_of('/') + 1);
        auto result = aliases_.try_emplace(basename, key);
        if (!result.second && result.first->second != key)
            result.first->second.clear(); // ambiguous; never select the first unrelated asset
    }

    const Entry* Exact(const std::string& key) const
    {
        auto found = paths_.find(key);
        return found == paths_.end() ? nullptr : &found->second;
    }

    const Entry* UniqueBasename(const std::string& basename) const
    {
        auto found = aliases_.find(basename);
        return found == aliases_.end() || found->second.empty() ? nullptr : Exact(found->second);
    }

    bool AmbiguousBasename(const std::string& basename) const
    {
        auto found = aliases_.find(basename);
        return found != aliases_.end() && found->second.empty();
    }

    void clear() { paths_.clear(); aliases_.clear(); }

private:
    std::unordered_map<std::string, Entry> paths_;
    std::unordered_map<std::string, std::string> aliases_;
};

template <typename Entry>
const Entry* ResolveVfsArchivePath(const char* request,
                                  const TVfsPathIndex<Entry>& module,
                                  const TVfsPathIndex<Entry>& base)
{
    std::string key = VfsPathKey(request);
    // ResourcePath and absolute host paths can prefix an archive-relative path.
    // Prefer the longest matching path before considering a shorter suffix.
    for (;;) {
        if (const Entry* found = module.Exact(key)) return found;
        if (const Entry* found = base.Exact(key)) return found;
        const auto slash = key.find('/');
        if (slash == std::string::npos) break;
        key.erase(0, slash + 1);
    }
    if (const Entry* found = module.UniqueBasename(key)) return found;
    return base.UniqueBasename(key);
}
