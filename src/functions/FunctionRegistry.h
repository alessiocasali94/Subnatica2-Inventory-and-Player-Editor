#pragma once

#include "functions/TrainerFunction.h"

#include <vector>

namespace functions {

class FunctionRegistry {
public:
    FunctionRegistry();

    std::vector<TrainerFunction>& All() { return functions_; }
    const std::vector<TrainerFunction>& All() const { return functions_; }

    TrainerFunction* Find(const std::string& id);
    std::vector<TrainerFunction*> ByCategory(Category cat);

    int FakeDepthMeters() const { return fakeDepth_; }
    void SetFakeDepth(int m) { fakeDepth_ = m; }

private:
    void RegisterDefaults();
    TrainerFunction& Add(TrainerFunction fn);

    std::vector<TrainerFunction> functions_;
    int fakeDepth_ = 312;
};

const char* CategoryName(Category c);
const char* CategoryShortName(Category c);
const char* CategoryIcon(Category c);

} // namespace functions
