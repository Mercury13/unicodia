#include "UcData.h"

#define TX_STA "Lib.Sta."

constinit const ec::Array<const char*, cou::Type> cou::typeKeys {
    TX_STA "Rec",
    TX_STA "NoMan",
    TX_STA "Union",
    TX_STA "Org",
    TX_STA "AuExt",  // 5
    TX_STA "GbOv",
    TX_STA "GbOv!",
    TX_STA "GbOvF",
    TX_STA "GbDep",
    TX_STA "CnSpec",
    TX_STA "DkAuto", // 10
    TX_STA "FiSpec",
    TX_STA "FrOv",
    TX_STA "FrUn",
    TX_STA "MuGb",
    TX_STA "NlDep", // 15
    TX_STA "NlOv",
    TX_STA "NzAssoc",
    TX_STA "NoUn",
    TX_STA "NoDep",
    TX_STA "EsAuto", // 20
    TX_STA "UsDep",
    TX_STA "PartPs",
    TX_STA "PartEh",
    TX_STA "PartTw",
    TX_STA "PartXk", // 25
};


#define TX_LOC "Lib.Loc."

constinit const ec::Array<const char*, cou::Location> cou::locKeys {
    TX_LOC "Eu",
    TX_LOC "As",
    TX_LOC "Af",
    TX_LOC "Na",
    TX_LOC "Sa", // 5
    TX_LOC "Au",
    TX_LOC "Sp",
    TX_LOC "EuAs",
    TX_LOC "Oa",
    TX_LOC "OaEu", // 10
    TX_LOC "OaGb",
    TX_LOC "OaAf",
    TX_LOC "OaNa",
    TX_LOC "OaSa",
    TX_LOC "OaCs", // 15
    TX_LOC "OaAq",
    TX_LOC "OaEc",
    TX_LOC "OaBs",
    TX_LOC "OaMs",
    TX_LOC "OpPo", // 20
    TX_LOC "OpMi",
    TX_LOC "OpAs",
    TX_LOC "OpAu",
    TX_LOC "OpNa",
    TX_LOC "Ma", // 25
    TX_LOC "AsMa",
    TX_LOC "Oi",
    TX_LOC "OiAf",
    TX_LOC "OiAs",
    TX_LOC "OiAq", // 30
    TX_LOC "OiGp",
    TX_LOC "On",
    TX_LOC "NyHq", // 33
};


#define TX_POP "Lib.Pop."

constinit const ec::Array<const char*, cou::Popul> cou::popKeys {
    TX_POP "Dfl",
    TX_POP "Set",
    TX_POP "SetSv",
    TX_POP "Mil",
    TX_POP "Sci",
    TX_POP "Nom",
    TX_POP "Pri",
    TX_POP "Uni",
    TX_POP "Nat",
    TX_POP "GS",
    "",
};


#define TX_ORI "Lib.Ori."


constinit const ec::Array<const char*, cou::Origin> cou::originKeys {
    // Misc
    TX_ORI "Com",
    TX_ORI "Aut",
    TX_ORI "Exo",
    TX_ORI "Old",
    TX_ORI "Fre",
    TX_ORI "Eur",
    // Langs
    TX_ORI "en",
    TX_ORI "es",
    TX_ORI "fr",
    TX_ORI "la",
    TX_ORI "nl",
    TX_ORI "ru",
    // Special cases
    TX_ORI "BQ",
    TX_ORI "CQ",
    TX_ORI "KZ",
    TX_ORI "SX",
    TX_ORI "UA",
    TX_ORI "UZ",
    TX_ORI "XK",
};


constinit const cou::Country ALL_COUNTRIES[] {
    { "AC", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_OCEAN,  // Ascension Island
            cou::Origin::L_EN, {},
            cou::Popul::MAINLY_MILITARY },
                // ↑ WP: Dry and barren, the island had little appeal for passing ships
                // except for collecting fresh meat
                // WP: RAF Ascension Island
    { "AD", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Andorra
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "AE", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // United Arab Emirates
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "AF", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Afghanistan
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "AG", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Antigua & Barbuda
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: until after a combination of European and African diseases, malnutrition,
                // and slavery eventually extirpated most of the native population
                // So let it be permament population, though indigenous are probably exterminated
    { "AI", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Anguilla
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP has no precise data, but Anguilla was probably settled by some Caribs
                // later exterminated due to their fierce nature
    { "AL", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Albania
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "AM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Armenia
            cou::Origin::EXO, {},  // Hayastan
            cou::Popul::DFLT },
                // ↑ Continent
    { "AO", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Angola
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "AQ", cou::Type::NO_MANS_LAND, cou::Location::SOUTH_POLE, // Antarctica
            cou::Origin::L_FR, "[A]ntarcti[q]ue",
            cou::Popul::MAINLY_SCIENTISTS },
                // ↑ Obvious
    { "AR", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Argentina
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "AS", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_POLYNESIA, // American Samoa
            cou::Origin::L_EN, {},
            cou::Popul::DFLT },
                // ↑ The 1789 visit by Lapérouse ended in an attack → HAVE indigenous
    { "AT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Austria
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "AU", cou::Type::RECOGNIZED_STATE, cou::Location::AUSTRALIA, // Australia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Obvious
    { "AW", cou::Type::NETHERLANDS_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN, // Aruba
            cou::Origin::OLD, "[A]ruba, [W]est Indies",
            cou::Popul::DFLT },
                // ↑ WP: Last indigenous Aruban (≈1840 killed in accident)
    { "AX", cou::Type::FINLAND_SPECIAL, cou::Location::BALTIC_SEA, // Åland Islands
            cou::Origin::FREE, {},
            cou::Popul::DFLT },
                // ↑ WP: Construction of the Kastelholm Castle began in the 1380s.
    { "AZ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Azerbaijan
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "BA", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Bosnia & Herzegovina
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "BB", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Barbados
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: The Kalinago (called "Caribs" by the Spanish) visited the island regularly,
                // although there is no evidence of permanent settlement.
    { "BD", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Bangladesh
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "BE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Belgium
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "BF", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Burkina Faso
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "BG", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Bulgaria
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "BH", cou::Type::RECOGNIZED_STATE, cou::Location::GULF_OF_PERSIA, // Bahrain
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: From the sixth to third century BC, Bahrain was part
                // of the Achaemenid Empire.
    { "BI", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Burundi
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "BJ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Benin
            cou::Origin::FREE, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "BL", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // St. Barthélemy
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: it is believed that the island was not inhabited permanently
                // due to its lack of fresh water sources
    { "BM", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_NORTH_AMERICA, // Bermuda
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: Bermuda had no Indigenous population when it was discovered,
                // nor during initial British settlement a century later.
    { "BN", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO, // Brunei
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Cannot quote WP but in 1400 there was smth
    { "BO", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Bolivia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "BQ", cou::Type::NETHERLANDS_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Caribbean Netherlands
            cou::Origin::SPEC_BQ, {},
            cou::Popul::DFLT },
                // ↑ WP: Remnants of Bonaire's indigenous population can be seen in some of the
                // island's current inhabitants.
    { "BR", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Brazil
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "BS", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_NORTH_AMERICA, // Bahamas
            cou::Origin::L_EN, {},
            cou::Popul::DFLT },
                // ↑ WP: The Spanish did however exploit the native Lucayan peoples
    { "BT", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Bhutan
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "BV", cou::Type::NORWAY_UNINHABITED, cou::Location::ATLANTIC_ANTARCTIC, // Bouvet Island
            cou::Origin::EURO, {},
            cou::Popul::NATURE_RESERVE },
                // ↑ Obvious: a small island in such an inhospitable place
                // WP: It is a protected nature reserve.
    { "BW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Botswana
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "BY", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Belarus
            cou::Origin::L_RU, "[By]elorussia",
            cou::Popul::DFLT },
                // ↑ Continent
    { "BZ", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Belize
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "CA", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Canada
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "CC", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::INDIAN_OCEAN, // Cocos (Keeling) Islands
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: no settlement occurred until the early 19th century
    { "CD", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Congo - Kinshasa
            cou::Origin::COMMON, "[C]ongo, [D]emocratic",
            cou::Popul::DFLT },
                // ↑ Continent
    { "CF", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Central African Republic
            cou::Origin::L_EN, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "CG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Congo - Brazzaville
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "CH", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Switzerland
            cou::Origin::L_LA, "[C]onfoederatio [H]elvetica",
            cou::Popul::DFLT },
                // ↑ Continent
    { "CI", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Côte d’Ivoire
            cou::Origin::AUTO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "CK", cou::Type::NEWZEALAND_ASSOC, cou::Location::PACIFIC_POLYNESIA, // Cook Islands
            cou::Origin::EURO, {},
            cou::Popul::DFLT },
                // ↑ WP: trouble broke out between the sailors and the Islanders and many
                // were killed on both sides
    { "CL", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Chile
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "CM", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Cameroon
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "CN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // China
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "CO", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Colombia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "CP", cou::Type::FRANCE_UNINHABITED, cou::Location::PACIFIC_NORTH_AMERICA, // Clipperton Island
            cou::Origin::EURO, {},
            cou::Popul::UNINHABITED },
                // ↑ Just nothing in WP about the people
    { "CQ", cou::Type::BRITAIN_DEPENDENT, cou::Location::ENGLISH_CHANNEL, // Sark
            cou::Origin::SPEC_CQ, "Ser[cq]",
            cou::Popul::DFLT },
                // ↑ WP: In 933, Sark was included in the Duchy of Normandy
    { "CR", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Costa Rica
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "CU", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Cuba
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: The Indigenous Taíno were forced to work
    { "CV", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_AFRICA, // Cape Verde
            cou::Origin::AUTO, {},
            cou::Popul::SETTLERS },
                // ↑ WP: Before the arrival of Europeans, the Cape Verde Islands were uninhabited.
    { "CW", cou::Type::NETHERLANDS_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN, // Curaçao
            cou::Origin::OLD, "[C]uraçao, [W]est Indies",
            cou::Popul::DFLT },
                // ↑ WP: The Spaniards enslaved most of the Caquetios (Arawak)
    { "CX", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::MALAY_ARCHIPELAGO, // Christmas Island
            cou::Origin::FREE, "",
            cou::Popul::SETTLERS },
                // ↑ WP: In writing his account, he found the island uninhabited.
    { "CY", cou::Type::RECOGNIZED_STATE, cou::Location::MEDITERRANEAN_SEA, // Cyprus
            cou::Origin::EXO, "",
            cou::Popul::DFLT },
                // ↑ Obvious: cradle of Western civilization
    { "CZ", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Czechia
            cou::Origin::EXO, "",
            cou::Popul::DFLT },
                // ↑ Continent
    { "DE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Germany
            cou::Origin::AUTO, "[De]utschland",
            cou::Popul::DFLT },
                // ↑ Continent
    { "DG", cou::Type::MAURITIUS_GB, cou::Location::INDIAN_OCEAN, // Diego Garcia
            cou::Origin::EURO, {},
            cou::Popul::MAINLY_MILITARY },
                // ↑ WP: No tangible evidence exists of people … before the arrival of Europeans
    { "DJ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Djibouti
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "DK", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Denmark
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "DM", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Dominica
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: But, as European explorers and settlers entered the region, indigenous
                // refugees from surrounding islands settled Dominica and pushed out the
                // Spanish settlers.
                // So let it be
    { "DO", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Dominican Republic
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Another island though close!!!
                // WP: Initially, after friendly relationships, the Taínos resisted the conquest
    { "DZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Algeria
            cou::Origin::AUTO, "[Dz]ayer",
            cou::Popul::DFLT },
                // ↑ Continent
    { "EA", cou::Type::SPAIN_AUTONOMY, cou::Location::AFRICA, // Ceuta & Melilla
            cou::Origin::L_ES, "[Á]frica Septentrional [E]spañola",
            cou::Popul::DFLT },
                // ↑ Don’t want to quote but Ceuta is an ancient city
    { "EC", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Ecuador
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "EE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Estonia
            cou::Origin::AUTO, "[Ee]sti",
            cou::Popul::DFLT },
                // ↑ Continent
    { "EG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Egypt
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "EH", cou::Type::PARTLY_WEST_SAHARA, cou::Location::AFRICA, // Western Sahara
            cou::Origin::L_ES, "[E]spañol Sa[h]ara",
            cou::Popul::MAINLY_NOMADS },
                // ↑ Don’t want to write, it’s a sad story
    { "ER", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Eritrea
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "ES", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Spain
            cou::Origin::AUTO, "[Es]paña",
            cou::Popul::DFLT },
                // ↑ Continent
    { "ET", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Ethiopia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "EU", cou::Type::SUPRANATIONAL_UNION, cou::Location::EUROPE, // European Union
            cou::Origin::COMMON, "[Eu]rope / [E]uropean [U]nion",
            cou::Popul::DFLT },
                // ↑ Continent
    { "FI", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Finland
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "FJ", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA, // Fiji
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: Early interaction with Europeans
    { "FK", cou::Type::BRITAIN_OVERSEAS_ACTUALLY, cou::Location::ATLANTIC_SOUTH_AMERICA, // Falkland Islands
            cou::Origin::L_EN, {},
            cou::Popul::SETTLERS },
                // ↑ WP: the islands were uninhabited when Europeans first explored them.
    { "FM", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA, // Micronesia
            cou::Origin::L_EN, "[F]ederated [M]icronesia",
            cou::Popul::DFLT },
                // ↑ WP: Yap is known for its stone money
    { "FO", cou::Type::DENMARK_AUTONOMY, cou::Location::ATLANTIC_EUROPE, // Faroe Islands
            cou::Origin::AUTO, "[Fø]royar",
            cou::Popul::DFLT },
                // ↑ WP: Christianity was introduced to the islands in the late 10th
                // and early 11th centuries
    { "FR", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // France
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "GA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Gabon
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "GB", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_EUROPE, // United Kingdom
            cou::Origin::AUTO, "[G]reat [B]ritain",
            cou::Popul::DFLT },
                // ↑ Obvious
    { "GD", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Grenada
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: Grenada was inhabited by the indigenous peoples from South America
    { "GE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE_ASIA, // Georgia
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "GF", cou::Type::FRANCE_OVERSEAS, cou::Location::SOUTH_AMERICA, // French Guiana
            cou::Origin::L_FR, "[G]uyane [F]rançaise",
            cou::Popul::DFLT },
                // ↑ Continent
    { "GG", cou::Type::BRITAIN_DEPENDENT, cou::Location::ENGLISH_CHANNEL, // Guernsey
            cou::Origin::FREE, {},
            cou::Popul::DFLT },
                // ↑ Don’t want to write but long history
    { "GH", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Ghana
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "GI", cou::Type::BRITAIN_OVERSEAS_ACTUALLY, cou::Location::EUROPE, // Gibraltar
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Barren cliff with very long history, everyone
                // attempted to control it
    { "GL", cou::Type::DENMARK_AUTONOMY, cou::Location::ARCTIC_OCEAN, // Greenland
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ AFAIK, long history of colonization
    { "GM", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Gambia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "GN", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Guinea
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "GP", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Guadeloupe
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: Several attempts at colonisation by the Spanish in the 16th
                // century failed due to attacks from the native people
    { "GQ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Equatorial Guinea
            cou::Origin::L_FR, "[G]uinée é[q]uatoriale",
            cou::Popul::DFLT },
                // ↑ Continent
    { "GR", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Greece
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "GS", cou::Type::BRITAIN_OVERSEAS_ACTUALLY, cou::Location::ATLANTIC_ANTARCTIC, // South Georgia & South Sandwich Islands
            cou::Origin::COMMON, "[G]eorgia [S]andwich",
            cou::Popul::ABANDONED_GS },
                // ↑ Don’t want to write but checked long ago, special strings!
    { "GT", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Guatemala
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "GU", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_MICRONESIA, // Guam
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: The Spanish-Chamorro Wars on Guam began in 1670
    { "GW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Guinea-Bissau
            cou::Origin::AUTO, "[G]ine-Bisaa[w]o",
            cou::Popul::DFLT },
                // ↑ Continent
    { "GY", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Guyana
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "HK", cou::Type::CHINA_SPECIAL, cou::Location::ASIA, // Hong Kong SAR China
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "HM", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::INDIAN_ANTARCTIC, // Heard & McDonald Islands
            cou::Origin::EURO, {},
            cou::Popul::UNINHABITED },
                // ↑ Antarctic
    { "HN", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Honduras
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "HR", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Croatia
            cou::Origin::AUTO, "[Hr]vatska",
            cou::Popul::DFLT },
                // ↑ Continent
    { "HT", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Haiti
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: Relations with the native peoples, initially good, broke down
                // and the settlers were later killed by the Taíno.
    { "HU", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Hungary
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "IC", cou::Type::SPAIN_AUTONOMY, cou::Location::ATLANTIC_AFRICA, // Canary Islands
            cou::Origin::L_ES, "[I]slas [C]anarias",
            cou::Popul::DFLT },
                // ↑ WP: These invasions were "brutal cultural and military clashes between the
                // indigenous population and the Castilians" lasting over a century
    { "ID", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO, // Indonesia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Obvious
    { "IE", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_BRITISH, // Ireland
            cou::Origin::L_EN, {},
            cou::Popul::DFLT },
                // ↑ Obvious
    { "IL", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Israel
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "IM", cou::Type::BRITAIN_DEPENDENT, cou::Location::ATLANTIC_BRITISH, // Isle of Man
            cou::Origin::L_EN, {},
            cou::Popul::DFLT },
                // ↑ Long history, believe me
    { "IN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // India
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "IO", cou::Type::BRITAIN_OVERSEAS_FORMER, cou::Location::INDIAN_OCEAN, // British Indian Ocean Territory
            cou::Origin::L_EN, "[I]ndian [O]cean",
            cou::Popul::MAINLY_MILITARY },
                // ↑ Long story ended in 2024
    { "IQ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Iraq
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "IR", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Iran
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "IS", cou::Type::RECOGNIZED_STATE, cou::Location::ARCTIC_OCEAN, // Iceland
            cou::Origin::AUTO, "[Ís]land",
            cou::Popul::DFLT },
                // ↑ Obvious
    { "IT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Italy
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "JE", cou::Type::BRITAIN_DEPENDENT, cou::Location::ENGLISH_CHANNEL, // Jersey
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Some medieval history
    { "JM", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Jamaica
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: Many slaves managed to escape, forming autonomous communities
                // in remote and easily defended areas in the interior of Jamaica,
                // mixing with the remaining Taino;
    { "JO", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Jordan
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "JP", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_ASIA, // Japan
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Obvious
    { "KE", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Kenya
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "KG", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Kyrgyzstan
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "KH", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Cambodia
            cou::Origin::OLD, "[Kh]mer Republic",
            cou::Popul::DFLT },
                // ↑ Continent
    { "KI", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA, // Kiribati
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ labour ships associated with the coercive labour recruitment practices,
                // known as blackbirding.
    { "KM", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA, // Comoros
            cou::Origin::AUTO, "[K]o[m]ori",
            cou::Popul::DFLT },
                // ↑ They also brought Islam to the people of the Zanj including the Comoros.
    { "KN", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // St. Kitts & Nevis
            cou::Origin::EURO, {},
            cou::Popul::DFLT },
                // ↑ WP: The French and English, intent on exploitation of the island's resources,
                // encountered resistance from the native Caribs (Kalinago),
    { "KP", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // North Korea
            cou::Origin::FREE, "[K]orea, [P]eople’s Republic",
            cou::Popul::DFLT },
                // ↑ Continent
    { "KR", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // South Korea
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "KW", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Kuwait
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "KY", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Cayman Islands
            cou::Origin::FREE, {},
            cou::Popul::SETTLERS },
                // ↑ WP: As of 2017, no evidence has been found that the islands
                // had been occupied before their discovery by Europeans
    { "KZ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Kazakhstan
            cou::Origin::SPEC_KZ, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "LA", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Laos
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "LB", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Lebanon
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "LC", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // St. Lucia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: In 1605, an English vessel called the Oliphe Blossome was blown off-course
                // on its way to Guyana, and the 67 colonists started a settlement on Saint Lucia,
                // after initially being welcomed by the Carib chief Anthonie.
    { "LI", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Liechtenstein
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "LK", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_ASIA, // Sri Lanka
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Obvious
    { "LR", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Liberia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "LS", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Lesotho
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "LT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Lithuania
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "LU", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Luxembourg
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "LV", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Latvia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "LY", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Libya
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "MA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Morocco
            cou::Origin::L_FR, "[Ma]roc",
            cou::Popul::DFLT },
                // ↑ Continent
    { "MC", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Monaco
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "MD", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Moldova
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "ME", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Montenegro
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "MF", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // St. Martin
            cou::Origin::FREE, "[M]artin, [F]rench",
            cou::Popul::DFLT },
                // ↑ WP: When Europeans began to explore the Caribbean, Carib society
                // had almost completely displaced the Arawaks.
    { "MG", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA, // Madagascar
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: The written history of Madagascar began with the Arabs
    { "MH", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA, // Marshall Islands
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Micronesian made stick charts, that’s enough
    { "MK", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // North Macedonia
            cou::Origin::AUTO, "[M]a[k]edonija",
            cou::Popul::DFLT },
                // ↑ Continent
    { "ML", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Mali
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "MM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Myanmar (Burma)
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "MN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Mongolia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "MO", cou::Type::CHINA_SPECIAL, cou::Location::ASIA, // Macao SAR China
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ The region is first known to have been settled during the Han dynasty.
    { "MP", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_MICRONESIA, // Northern Mariana Islands
            cou::Origin::FREE, "[M]arianas [P]acific",
            cou::Popul::DFLT },
                // ↑ In the 18th century the people of the northern Marianas
                // were forced by Spain to relocate
    { "MQ", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Martinique
            cou::Origin::L_FR, {},
            cou::Popul::DFLT },
                // ↑ WP: The Indigenous people Columbus encountered called Martinique "Matinino".
    { "MR", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Mauritania
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "MS", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Montserrat
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: after being told that the island was unoccupied because of raids by the Caribs.
    { "MT", cou::Type::RECOGNIZED_STATE, cou::Location::MEDITERRANEAN_SEA, // Malta
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Maltese order etc → let it be
    { "MU", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA, // Mauritius
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: In 1507, Portuguese sailors came to the uninhabited island
                // and established a visiting base.
    { "MV", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_ASIA, // Maldives
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Dives Akuru
    { "MW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Malawi
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "MX", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Mexico
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "MY", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA_MALAY_ARCHIPELAGO, // Malaysia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Partly continent
    { "MZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Mozambique
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "NA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Namibia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "NC", cou::Type::FRANCE_OVERSEAS, cou::Location::PACIFIC_AUSTRALIA, // New Caledonia
            cou::Origin::L_EN, {},
            cou::Popul::DFLT },
                // ↑ WP: As trade in sandalwood declined, it was replaced by a new business
                // enterprise, "blackbirding"
    { "NE", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Niger
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "NF", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::PACIFIC_AUSTRALIA, // Norfolk Island
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: Norfolk Island was uninhabited when first settled by Europeans,
                // but evidence of earlier habitation was obvious.
    { "NG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Nigeria
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "NI", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Nicaragua
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "NL", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Netherlands
                cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "NO", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Norway
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "NP", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Nepal
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "NR", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA, // Nauru
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: The islanders traded food for alcoholic palm wine and firearms.
    { "NU", cou::Type::NEWZEALAND_ASSOC, cou::Location::PACIFIC_POLYNESIA, // Niue
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: After many years of trying to land a European missionary,
                // they abducted a Niuean named Nukai Peniamina and trained him as a pastor
    { "NZ", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA, // New Zealand
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Maori
    { "OM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Oman
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "PA", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Panama
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "PE", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Peru
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "PF", cou::Type::FRANCE_OVERSEAS, cou::Location::PACIFIC_POLYNESIA, // French Polynesia
            cou::Origin::L_FR, "[P]olynésie [f]rançaise ",
            cou::Popul::DFLT },
                // ↑ WP: sailed through Polynesia sighting an inhabited island on 10 February
    { "PG", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO, // Papua New Guinea
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Miklukho-Maklay
    { "PH", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO, // Philippines
            cou::Origin::L_EN, {},
            cou::Popul::DFLT },
                // ↑ see Tglg
    { "PK", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Pakistan
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "PL", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Poland
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "PM", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_NORTH_AMERICA, // St. Pierre & Miquelon
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: However, it is not thought that they settled on the islands permanently.
    { "PN", cou::Type::BRITAIN_OVERSEAS, cou::Location::PACIFIC_POLYNESIA, // Pitcairn Islands
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: The islands were uninhabited when they were discovered by Europeans.
    { "PR", cou::Type::US_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN, // Puerto Rico
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: At the time of Christopher Columbus's arrival in the New World in 1493,
                // the dominant indigenous culture was that of the Taínos.
    { "PS", cou::Type::PARTLY_PALESTINE, cou::Location::ASIA, // Palestinian Territories
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "PT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Portugal
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "PW", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA, // Palau
            cou::Origin::OLD, "[P]ele[w]",
            cou::Popul::DFLT },
                // ↑ WP: 65.2% Palauan
    { "PY", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Paraguay
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "QA", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Qatar
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "RE", cou::Type::FRANCE_OVERSEAS, cou::Location::INDIAN_AFRICA, // Réunion
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: The island has been inhabited since the 17th century
    { "RO", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Romania
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "RS", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Serbia
            cou::Origin::FREE, "[R]epublic of [S]erbia",
            cou::Popul::DFLT },
                // ↑ Continent
    { "RU", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE_ASIA, // Russia
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "RW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Rwanda
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SA", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Saudi Arabia
            cou::Origin::L_EN, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SB", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA, // Solomon Islands
            cou::Origin::OLD, "[S]olomon [B]ritish",
            cou::Popul::DFLT },
                // ↑ WP: 95.3% Melanesian
    { "SC", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA, // Seychelles
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: Seychelles was uninhabited until the 18th century when
                // Europeans arrived with Indians
    { "SD", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Sudan
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Sweden
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SG", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Singapore
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SH", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_OCEAN, // St. Helena
            cou::Origin::L_EN, {},
            cou::Popul::SETTLERS },
                // ↑ WP: The Portuguese found the island uninhabited
    { "SI", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Slovenia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SJ", cou::Type::NORWAY_DEPENDENT, cou::Location::ARCTIC_OCEAN, // Svalbard & Jan Mayen
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS_SVALBARD },
                // WP: was discovered multiple times → probably no men
    { "SK", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Slovakia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SL", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Sierra Leone
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SM", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // San Marino
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SN", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Senegal
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SO", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Somalia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SR", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Suriname
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SS", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // South Sudan
            cou::Origin::L_EN, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "ST", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_AFRICA, // São Tomé & Príncipe
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: The islands of São Tomé and Príncipe were uninhabited when
                // the Portuguese arrived sometime around 1470
    { "SV", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // El Salvador
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SX", cou::Type::NETHERLANDS_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Sint Maarten
            cou::Origin::SPEC_SX, "[SX]M",
            cou::Popul::DFLT },
                // ↑ See French part
    { "SY", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Syria
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "SZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Eswatini
            cou::Origin::OLD, "[S]wa[z]iland",
            cou::Popul::DFLT },
                // ↑ Continent
    { "TA", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_OCEAN, // Tristan da Cunha
            cou::Origin::COMMON, {},
            cou::Popul::SETTLERS },
                // ↑ WP: The uninhabited islands were first recorded as sighted in 1506
    { "TC", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_NORTH_AMERICA, // Turks & Caicos Islands
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: In either case, by 1512 the Spanish had begun capturing the Taíno and Lucayans
    { "TD", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Chad
            cou::Origin::L_FR, "[T]cha[d]",
            cou::Popul::DFLT },
                // ↑ Continent
    { "TF", cou::Type::FRANCE_UNINHABITED, cou::Location::INDIAN_ANTARCTIC, // French Southern Territories
            cou::Origin::L_FR, "[T]erres australes [f]rançaises ",
            cou::Popul::MAINLY_SCIENTISTS },
                // ↑ Antarctic :)
    { "TG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Togo
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "TH", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Thailand
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "TJ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Tajikistan
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "TK", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_POLYNESIA, // Tokelau
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Was intermittently populated and has own language → let it be
    { "TL", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO, // Timor-Leste
            cou::Origin::AUTO, {},
            cou::Popul::DFLT },
                // ↑ WP: From perhaps the thirteenth century, the island exported sandalwood
    { "TM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Turkmenistan
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "TN", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Tunisia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "TO", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_POLYNESIA, // Tonga
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ According to Tongan mythology, the demigod Maui drew up
    { "TR", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Türkiye
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "TT", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Trinidad & Tobago
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: …intending to subdue the Indigenous population of the island.
    { "TV", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_POLYNESIA, // Tuvalu
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: Mendaña made contact with the islanders but was unable to land
    { "TW", cou::Type::PARTLY_TAIWAN, cou::Location::PACIFIC_ASIA, // Taiwan
            cou::Origin::EXO, {},
            cou::Popul::DFLT },
                // ↑ Obvious
    { "TZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Tanzania
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "UA", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Ukraine
            cou::Origin::SPEC_UA, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "UG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Uganda
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "UM", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_POLYNESIA, // U.S. Outlying Islands
            cou::Origin::L_EN, "[U]S [M]inor Outlying Islands",
            cou::Popul::MAINLY_MILITARY },
                // ↑ WP: Currently, none of the islands have any known permanent residents
    { "UN", cou::Type::INTERNATIONAL_ORGANIZATION, cou::Location::NEWYORK_HQ, // United Nations
            cou::Origin::L_EN, {},
            cou::Popul::NOT_APPLICABLE },
                // ↑ Obvious
    { "US", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // United States
            cou::Origin::L_EN, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "UY", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Uruguay
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "UZ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Uzbekistan
            cou::Origin::SPEC_UZ, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "VA", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Vatican City
            cou::Origin::COMMON, {},
            cou::Popul::MAINLY_PRIESTS },
                // ↑ Obvious
    { "VC", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // St. Vincent & Grenadines
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: The Kalinago people aggressively opposed European settlement on Saint Vincent.
    { "VE", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Venezuela
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "VG", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // British Virgin Islands
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: There is no record of any native Amerindian population in the British Virgin
                // Islands during this period; it is thought that they either fled to safer islands
                // or were killed.
                // So let it be
    { "VI", cou::Type::US_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN, // U.S. Virgin Islands
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: The Kalinago (Island Caribs) arrived around the mid-15th century AD.
                // and some more Amerindians before
    { "VN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Vietnam
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "VU", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA, // Vanuatu
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: Despite Queirós's intention, relations with the Ni-Vanuatu turned
                // violent within days.
    { "WF", cou::Type::FRANCE_OVERSEAS, cou::Location::PACIFIC_POLYNESIA, // Wallis & Futuna
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ WP: with the arrival of French missionaries in 1837, who converted
                // the population to Roman Catholicism
    { "WS", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_POLYNESIA, // Samoa
            cou::Origin::OLD, "Western Samoa",
            cou::Popul::DFLT },
                // ↑ WP: for recruiting local men to serve as crewmen on their ships
    { "XK", cou::Type::PARTLY_KOSOVO, cou::Location::EUROPE, // Kosovo
            cou::Origin::SPEC_XK, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "YE", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Yemen
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "YT", cou::Type::FRANCE_OVERSEAS, cou::Location::INDIAN_AFRICA, // Mayotte
            cou::Origin::FREE, {},
            cou::Popul::DFLT },
                // ↑ WP: In 1500, the Maore sultanate was established
    { "ZA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // South Africa
            cou::Origin::L_NL, "[Z]uid-[A]frika",
            cou::Popul::DFLT },
                // ↑ Continent
    { "ZM", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Zambia
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
    { "ZW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Zimbabwe
            cou::Origin::COMMON, {},
            cou::Popul::DFLT },
                // ↑ Continent
};


const cou::Country* cou::find(TwoLetters key)
{
    auto beg = std::begin(ALL_COUNTRIES);
    auto end = std::end(ALL_COUNTRIES);
    auto place = std::lower_bound(beg, end, key,
                [](const Country& x, TwoLetters y) {
                    return (x.name < y);
                });
    if (place != end && place->name == key)
        return place;
    return nullptr;
}
