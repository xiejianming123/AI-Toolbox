#include <AIToolbox/Factored/Utils/Core.hpp>

#include <AIToolbox/Utils/Core.hpp>

namespace AIToolbox::Factored {
    PartialFactors removeFactor(const PartialFactors & pf, const size_t f) {
        size_t i = 0;
        while (i < pf.first.size() && pf.first[i] < f) ++i;
        if (i == pf.first.size() || pf.first[i] != f) return pf;

        PartialFactors retval;
        retval.first.reserve(pf.first.size() - 1);
        retval.second.reserve(pf.first.size() - 1);

        for (size_t j = 0; j < pf.first.size(); ++j) {
            if (i == j) continue;
            retval.first.push_back(pf.first[j]);
            retval.second.push_back(pf.second[j]);
        }
        return retval;
    }

    bool match(const PartialFactors & lhs, const PartialFactors & rhs) {
        const PartialFactors * smaller = &rhs, * bigger = &lhs;
        if (lhs.first.size() < rhs.first.size()) std::swap(smaller, bigger);

        size_t i = 0, j = 0;
        while (j < smaller->second.size()) {
            if (bigger->first[i] < smaller->first[j]) ++i;
            else if (bigger->first[i] > smaller->first[j]) ++j;
            else {
                if (bigger->second[i] != smaller->second[j]) return false;
                ++i;
                ++j;
            }
        }
        return true;
    }

    bool match(const PartialKeys & keys, const Factors & lhs, const Factors & rhs) {
        for (auto k : keys)
            if (lhs[k] != rhs[k])
                return false;
        return true;
    }

    bool match(const std::vector<std::pair<size_t, size_t>> & matches, const Factors & lhs, const Factors & rhs) {
        for (const auto & kk : matches)
            if (lhs[kk.first] != rhs[kk.second])
                return false;
        return true;
    }

    PartialFactors join(const size_t S, const PartialFactors & lhs, const PartialFactors & rhs) {
        PartialFactors retval;
        retval.first.reserve(lhs.first.size() + rhs.first.size());
        retval.second.reserve(lhs.first.size() + rhs.first.size());
        // lhs part is the same.
        retval = lhs;
        // rhs part is shifted by S elements (values are the same)
        std::transform(std::begin(rhs.first), std::end(rhs.first), std::back_inserter(retval.first), [S](const size_t a){ return a + S; });
        retval.second.insert(std::end(retval.second), std::begin(rhs.second), std::end(rhs.second));

        return retval;
    }

    Factors join(const Factors & lhs, const Factors & rhs) {
        Factors retval;
        retval.reserve(lhs.size() + rhs.size());
        retval.insert(std::end(retval), std::begin(lhs), std::end(lhs));
        retval.insert(std::end(retval), std::begin(rhs), std::end(rhs));
        return retval;
    }

    void unsafe_join(PartialFactors * lhsp, const PartialFactors & rhs) {
        if (!lhsp) return;
        auto & lhs = *lhsp;

        lhs.first.insert(std::end(lhs.first), std::begin(rhs.first), std::end(rhs.first));
        lhs.second.insert(std::end(lhs.second), std::begin(rhs.second), std::end(rhs.second));
    }

    PartialFactors merge(const PartialFactors & lhs, const PartialFactors & rhs) {
        PartialFactors retval;
        retval.first.reserve(lhs.first.size() + rhs.first.size());
        retval.second.reserve(lhs.first.size() + rhs.first.size());

        size_t i = 0, j = 0;
        while (i < lhs.first.size() && j < rhs.first.size()) {
            if (lhs.first[i] < rhs.first[j]) {
                retval.first.push_back(lhs.first[i]);
                retval.second.push_back(lhs.second[i]);
                ++i;
            } else {
                retval.first.push_back(rhs.first[j]);
                retval.second.push_back(rhs.second[j]);

                if (lhs.first[i] == rhs.first[j]) ++i;
                ++j;
            }
        }
        retval.first.insert(std::end(retval.first),   std::begin(lhs.first) + i, std::end(lhs.first));
        retval.second.insert(std::end(retval.second), std::begin(lhs.second) + i, std::end(lhs.second));

        retval.first.insert(std::end(retval.first),   std::begin(rhs.first) + j, std::end(rhs.first));
        retval.second.insert(std::end(retval.second), std::begin(rhs.second) + j, std::end(rhs.second));

        return retval;
    }

    Factors merge(const PartialKeys & lhsk, const Factors & lhs, const PartialKeys & rhsk, const Factors & rhs) {
        Factors retval;
        retval.reserve(lhsk.size() + rhsk.size());

        size_t i = 0, j = 0;
        while (i < lhsk.size() && j < rhsk.size()) {
            if (lhsk[i] < rhsk[j]) {
                retval.push_back(lhs[i]);
                ++i;
            } else {
                retval.push_back(rhs[j]);

                if (lhsk[i] == rhsk[j]) ++i;
                ++j;
            }
        }
        retval.insert(std::end(retval), std::begin(lhs) + i, std::end(lhs));
        retval.insert(std::end(retval), std::begin(rhs) + j, std::end(rhs));

        return retval;
    }

    PartialKeys merge(const PartialKeys & lhs, const PartialKeys & rhs, std::vector<std::pair<size_t, size_t>> * matches) {
        PartialKeys retval;
        retval.reserve(lhs.size() + rhs.size());

        size_t i = 0, j = 0;
        while (i < lhs.size() && j < rhs.size()) {
            if (lhs[i] == rhs[j]) {
                if (matches) matches->emplace_back(i, j);
                retval.push_back(lhs[i]);
                ++i, ++j;
            }
            else if (lhs[i] < rhs[j])
                retval.push_back(lhs[i++]);
            else
                retval.push_back(rhs[j++]);
        }

        retval.insert(std::end(retval), std::begin(lhs) + i, std::end(lhs));
        retval.insert(std::end(retval), std::begin(rhs) + j, std::end(rhs));

        return retval;
    }

    size_t factorSpace(const Factors & space) {
        size_t retval = 1;
        for (const auto f : space) {
            // Detect wraparound
            if (std::numeric_limits<size_t>::max() / f < retval)
                return std::numeric_limits<size_t>::max();
            retval *= f;
        }
        return retval;
    }

    size_t factorSpacePartial(const PartialKeys & ids, const Factors & space) {
        size_t retval = 1;
        for (const auto id : ids) {
            // Detect wraparound
            if (std::numeric_limits<size_t>::max() / space[id] < retval)
                return std::numeric_limits<size_t>::max();
            retval *= space[id];
        }
        return retval;
    }

    PartialFactors toPartialFactors(const Factors & f) {
        PartialFactors retval;

        retval.first.resize(f.size());
        for (size_t i = 0; i < f.size(); ++i)
            retval.first[i] = i;
        retval.second = f;

        return retval;
    }

    Factors toFactors(const size_t F, const PartialFactors & pf) {
        Factors f(F);
        for (size_t i = 0; i < pf.first.size(); ++i)
            f[pf.first[i]] = pf.second[i];

        return f;
    }

    Factors toFactors(const Factors & space, size_t id) {
        Factors f(space.size());
        for (size_t i = 0; i < space.size(); ++i) {
            f[i] = id % space[i];
            id /= space[i];
        }
        return f;
    }

    Factors toFactorsPartial(const PartialKeys & keys, const Factors & space, size_t id) {
        Factors f(keys.size());
        size_t i = 0;
        for (auto key : keys) {
            f[i] = id % space[key];
            id /= space[key];
            ++i;
        }
        return f;
    }

    size_t toIndex(const Factors & space, const Factors & f) {
        size_t result = 0; size_t multiplier = 1;
        for (size_t i = 0; i < f.size(); ++i) {
            result += multiplier * f[i];
            multiplier *= space[i];
        }
        return result;
    }

    size_t toIndex(const Factors & space, const PartialFactors & f) {
        size_t result = 0; size_t multiplier = 1;
        for (size_t i = 0, j = 0; i < space.size(); ++i) {
            if (i == f.first[j]) {
                result += multiplier * f.second[j++];
                if (j == f.first.size()) break;
            }
            multiplier *= space[i];
        }
        return result;
    }

    size_t toIndexPartial(const PartialKeys & ids, const Factors & space, const Factors & f) {
        size_t result = 0; size_t multiplier = 1;
        for (size_t i = 0; i < ids.size(); ++i) {
            result += multiplier * f[ids[i]];
            multiplier *= space[ids[i]];
        }
        return result;
    }

    size_t toIndexPartial(const PartialKeys & ids, const Factors & space, const PartialFactors & pf) {
        size_t result = 0; size_t multiplier = 1;
        size_t j = 0;
        for (size_t i = 0; i < ids.size(); ++i) {
            while (pf.first[j] != ids[i]) ++j;
            result += multiplier * pf.second[j];
            multiplier *= space[ids[i]];
        }
        return result;
    }

    size_t toIndexPartial(const Factors & space, const PartialFactors & f) {
        size_t result = 0; size_t multiplier = 1;
        for (size_t i = 0; i < f.first.size(); ++i) {
            result += multiplier * f.second[i];
            multiplier *= space[f.first[i]];
        }
        return result;
    }

    // PartialFactorsEnumerator below.

    PartialFactorsEnumerator::PartialFactorsEnumerator(Factors f, PartialKeys factors) :
        F(std::move(f)), factorToSkipId_(factors.size())
    {
        factors_.first = std::move(factors);
        factors_.second.resize(factors_.first.size());
    }

    PartialFactorsEnumerator::PartialFactorsEnumerator(Factors f) :
        F(std::move(f)), factorToSkipId_(F.size())
    {
        factors_.first.resize(F.size());
        std::iota(std::begin(factors_.first), std::end(factors_.first), 0);
        factors_.second.resize(factors_.first.size());
    }

    PartialFactorsEnumerator::PartialFactorsEnumerator(Factors f, PartialKeys factors, const size_t factorToSkip) :
        PartialFactorsEnumerator(std::move(f), std::move(factors))
    {
        // Set all used agents and find the skip id.
        for (size_t i = 0; i < factors_.first.size(); ++i) {
            if (factorToSkip == factors_.first[i]) {
                factorToSkipId_ = i;
                break;
            }
        }
    }

    PartialFactorsEnumerator::PartialFactorsEnumerator(Factors f, const size_t factorToSkip) :
        PartialFactorsEnumerator(std::move(f))
    {
        factorToSkipId_ = factorToSkip;
    }

    void PartialFactorsEnumerator::advance() {
        // Start from 0 if skip is not zero, from 1 otherwise.
        size_t id = !factorToSkipId_;
        while (id < factors_.second.size()) {
            ++factors_.second[id];
            if (factors_.second[id] == F[factors_.first[id]]) {
                factors_.second[id] = 0;
                if (++id == factorToSkipId_) ++id;
            } else
                return;
        }
        factors_.second.clear();
    }

    bool PartialFactorsEnumerator::isValid() const {
        return factors_.second.size() > 0;
    }

    void PartialFactorsEnumerator::reset() {
        if (factors_.second.size() == 0)
            factors_.second.resize(factors_.first.size());
        else
            std::fill(std::begin(factors_.second), std::end(factors_.second), 0);
    }

    size_t PartialFactorsEnumerator::getFactorToSkipId() const { return factorToSkipId_; }

    PartialFactors& PartialFactorsEnumerator::operator*() { return factors_; }
}
