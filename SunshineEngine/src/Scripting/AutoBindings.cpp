#include <Scripting/AutoBindings.h>

namespace AutoBindings {

static eastl::vector<Binder>& Storage() {
    static eastl::vector<Binder> s;
    return s;
}

void AddBinder(Binder b) {
    Storage().push_back(eastl::move(b));
}

void RegisterAll(sol::state& lua) {
    for (auto& b : Storage()) {
        b(lua);
    }
}

} // namespace AutoBindings
