#include "LocFmt.h"

template class loc::Fmt<char>;
template class loc::Fmt<wchar_t>;
template class loc::Fmt<char8_t>;
template class loc::FmtL<char>;
template class loc::FmtL<wchar_t>;
template class loc::FmtL<char8_t>;


const loc::DefaultQtyRule loc::DefaultQtyRule::INST;
const loc::DefaultLocale loc::DefaultLocale::INST;


///// DefaultQtyRule ///////////////////////////////////////////////////////////

loc::Plural loc::DefaultQtyRule::ofUint(unsigned long long n) const
    { return (n == 1) ? Plural::ONE : Plural::MANY; }


///// DefaultLocale ////////////////////////////////////////////////////////////

const loc::PluralRule& loc::DefaultLocale::qtyRule() const
    { return DefaultQtyRule::INST; }
