#pragma once
#include <filesystem>
#include <EASTL/string.h>
#include <EASTL/vector.h>

namespace eastlfs {

    class directory_iterator {
        std::filesystem::directory_iterator it;
        std::filesystem::directory_iterator end;

    public:
        directory_iterator() = default;
        directory_iterator(const eastl::string& dir)
            : it(std::filesystem::directory_iterator(dir.c_str())) {
        }
        directory_iterator(const eastl::string& dir, std::error_code& ec)
            : it(std::filesystem::directory_iterator(dir.c_str(), ec)) {
        }

        bool operator!=(const directory_iterator& rhs) const { return it != rhs.it; }
        void operator++() { ++it; }
        auto& operator*() { return *it; }
        auto* operator->() { return &(*it); }
        std::filesystem::directory_entry& entry() { return const_cast<std::filesystem::directory_entry&>(*it); }
    };

    inline bool is_regular_file(const std::filesystem::directory_entry& entry) { return entry.is_regular_file(); }

    inline eastl::string filename(const std::filesystem::directory_entry& entry) {
        return eastl::string(entry.path().filename().string().c_str());
    }

}
