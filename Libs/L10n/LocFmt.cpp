#include "LocFmt.h"

template class loc::Fmt<char>;
template class loc::Fmt<wchar_t>;
template class loc::Fmt<char8_t>;
template class loc::FmtL<char>;
template class loc::FmtL<wchar_t>;
template class loc::FmtL<char8_t>;

const loc::DefaultQtyRule loc::DefaultQtyRule::INST;
const loc::DefaultLocale loc::DefaultLocale::INST;
const loc::Locale* loc::activeFmtLocale = &loc::DefaultLocale::INST;

const std::string_view loc::pluralNames[loc::Plural_N] =
    { key::ZERO, key::ONE, key::TWO, key::FEW, key::MANY, key::OTHER, key::REST };

///// DefaultQtyRule ///////////////////////////////////////////////////////////

loc::Plural loc::DefaultQtyRule::ofUint(unsigned long long n) const
    { return (n == 1) ? Plural::ONE : Plural::MANY; }


///// DefaultLocale ////////////////////////////////////////////////////////////

const loc::PluralRule& loc::DefaultLocale::cardinalRule() const
    { return DefaultQtyRule::INST; }


///// Checkers /////////////////////////////////////////////////////////////////

loc::Plural loc::ZsgnChecker::check(const PluralRule& plu) const
    { return plu.ofInt(v); }

loc::Plural loc::ZunsChecker::check(const PluralRule& plu) const
    { return plu.ofUint(v); }

///// Checks, unit tests, etc //////////////////////////////////////////////////

static_assert(loc::Plural::REST < loc::Plural::BAD);
static_assert(loc::key::ST_ONE_OTHER == loc::key::OTHER[0]);
