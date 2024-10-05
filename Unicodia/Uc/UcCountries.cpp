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


constinit const cou::Country ALL_COUNTRIES[] {
    { "AC", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_OCEAN,  // Ascension Island
            cou::Popul::MAINLY_MILITARY },
                // ↑ WP: Dry and barren, the island had little appeal for passing ships
                // except for collecting fresh meat
                // WP: RAF Ascension Island
    { "AD", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Andorra
            cou::Popul::DFLT },
                // ↑ Continent
    { "AE", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // United Arab Emirates
            cou::Popul::DFLT },
                // ↑ Continent
    { "AF", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Afghanistan
            cou::Popul::DFLT },
                // ↑ Continent
    { "AG", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Antigua & Barbuda
            cou::Popul::DFLT },
                // ↑ WP: until after a combination of European and African diseases, malnutrition,
                // and slavery eventually extirpated most of the native population
                // So let it be permament population, though indigenous are probably exterminated
    { "AI", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Anguilla
            cou::Popul::DFLT },
                // ↑ WP has no precise data, but Anguilla was probably settled by some Caribs
                // later exterminated due to their fierce nature
    { "AL", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Albania
            cou::Popul::DFLT },
                // ↑ Continent
    { "AM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Armenia
            cou::Popul::DFLT },
                // ↑ Continent
    { "AO", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Angola
            cou::Popul::DFLT },
                // ↑ Continent
    { "AQ", cou::Type::NO_MANS_LAND, cou::Location::SOUTH_POLE, // Antarctica
            cou::Popul::MAINLY_SCIENTISTS },
                // ↑ Obvious
    { "AR", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Argentina
            cou::Popul::DFLT },
                // ↑ Continent
    { "AS", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_POLYNESIA, // American Samoa
            cou::Popul::DFLT },
                // ↑ The 1789 visit by Lapérouse ended in an attack → HAVE indigenous
    { "AT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Austria
            cou::Popul::DFLT },
                // ↑ Continent
    { "AU", cou::Type::RECOGNIZED_STATE, cou::Location::AUSTRALIA, // Australia
            cou::Popul::DFLT },
                // ↑ Obvious
    { "AW", cou::Type::NETHERLANDS_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN, // Aruba
            cou::Popul::DFLT },
                // ↑ WP: Last indigenous Aruban (≈1840 killed in accident)
    { "AX", cou::Type::FINLAND_SPECIAL, cou::Location::BALTIC_SEA, // Åland Islands
            cou::Popul::DFLT },
                // ↑ WP: Construction of the Kastelholm Castle began in the 1380s.
    { "AZ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Azerbaijan
            cou::Popul::DFLT },
                // ↑ Continent
    { "BA", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Bosnia & Herzegovina
            cou::Popul::DFLT },
                // ↑ Continent
    { "BB", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Barbados
            cou::Popul::SETTLERS },
                // ↑ WP: The Kalinago (called "Caribs" by the Spanish) visited the island regularly,
                // although there is no evidence of permanent settlement.
    { "BD", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Bangladesh
            cou::Popul::DFLT },
                // ↑ Continent
    { "BE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Belgium
            cou::Popul::DFLT },
                // ↑ Continent
    { "BF", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Burkina Faso
            cou::Popul::DFLT },
                // ↑ Continent
    { "BG", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Bulgaria
            cou::Popul::DFLT },
                // ↑ Continent
    { "BH", cou::Type::RECOGNIZED_STATE, cou::Location::GULF_OF_PERSIA, // Bahrain
            cou::Popul::DFLT },
                // ↑ WP: From the sixth to third century BC, Bahrain was part
                // of the Achaemenid Empire.
    { "BI", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Burundi
            cou::Popul::DFLT },
                // ↑ Continent
    { "BJ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Benin
            cou::Popul::DFLT },
                // ↑ Continent
    { "BL", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // St. Barthélemy
            cou::Popul::SETTLERS },
                // ↑ WP: it is believed that the island was not inhabited permanently
                // due to its lack of fresh water sources
    { "BM", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_NORTH_AMERICA, // Bermuda
            cou::Popul::SETTLERS },
                // ↑ WP: Bermuda had no Indigenous population when it was discovered,
                // nor during initial British settlement a century later.
    { "BN", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO, // Brunei
            cou::Popul::DFLT },
                // ↑ Cannot quote WP but in 1400 there was smth
    { "BO", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Bolivia
            cou::Popul::DFLT },
                // ↑ Continent
    { "BQ", cou::Type::NETHERLANDS_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Caribbean Netherlands
            cou::Popul::DFLT },
                // ↑ WP: Remnants of Bonaire's indigenous population can be seen in some of the
                // island's current inhabitants.
    { "BR", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Brazil
            cou::Popul::DFLT },
                // ↑ Continent
    { "BS", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_NORTH_AMERICA, // Bahamas
            cou::Popul::DFLT },
                // ↑ WP: The Spanish did however exploit the native Lucayan peoples
    { "BT", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Bhutan
            cou::Popul::DFLT },
                // ↑ Continent
    { "BV", cou::Type::NORWAY_UNINHABITED, cou::Location::ATLANTIC_ANTARCTIC, // Bouvet Island
            cou::Popul::NATURE_RESERVE },
                // ↑ Obvious: a small island in such an inhospitable place
                // WP: It is a protected nature reserve.
    { "BW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Botswana
            cou::Popul::DFLT },
                // ↑ Continent
    { "BY", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Belarus
            cou::Popul::DFLT },
                // ↑ Continent
    { "BZ", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Belize
            cou::Popul::DFLT },
                // ↑ Continent
    { "CA", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Canada
            cou::Popul::DFLT },
                // ↑ Continent
    { "CC", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::INDIAN_OCEAN, // Cocos (Keeling) Islands
            cou::Popul::SETTLERS },
                // ↑ WP: no settlement occurred until the early 19th century
    { "CD", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Congo - Kinshasa
            cou::Popul::DFLT },
                // ↑ Continent
    { "CF", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Central African Republic
            cou::Popul::DFLT },
                // ↑ Continent
    { "CG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Congo - Brazzaville
            cou::Popul::DFLT },
                // ↑ Continent
    { "CH", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Switzerland
            cou::Popul::DFLT },
                // ↑ Continent
    { "CI", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Côte d’Ivoire
            cou::Popul::DFLT },
                // ↑ Continent
    { "CK", cou::Type::NEWZEALAND_ASSOC, cou::Location::PACIFIC_POLYNESIA, // Cook Islands
            cou::Popul::DFLT },
                // ↑ WP: trouble broke out between the sailors and the Islanders and many
                // were killed on both sides
    { "CL", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Chile
            cou::Popul::DFLT },
                // ↑ Continent
    { "CM", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Cameroon
            cou::Popul::DFLT },
                // ↑ Continent
    { "CN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // China
            cou::Popul::DFLT },
                // ↑ Continent
    { "CO", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Colombia
            cou::Popul::DFLT },
                // ↑ Continent
    { "CP", cou::Type::FRANCE_UNINHABITED, cou::Location::PACIFIC_NORTH_AMERICA, // Clipperton Island
            cou::Popul::UNINHABITED },
                // ↑ Just nothing in WP about the people
    { "CQ", cou::Type::BRITAIN_DEPENDENT, cou::Location::ENGLISH_CHANNEL, // Sark
            cou::Popul::DFLT },
                // ↑ WP: In 933, Sark was included in the Duchy of Normandy
    { "CR", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Costa Rica
            cou::Popul::DFLT },
                // ↑ Continent
    { "CU", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Cuba
            cou::Popul::DFLT },
                // ↑ WP: The Indigenous Taíno were forced to work
    { "CV", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_AFRICA, // Cape Verde
            cou::Popul::SETTLERS },
                // ↑ WP: Before the arrival of Europeans, the Cape Verde Islands were uninhabited.
    { "CW", cou::Type::NETHERLANDS_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN, // Curaçao
            cou::Popul::DFLT },
                // ↑ WP: The Spaniards enslaved most of the Caquetios (Arawak)
    { "CX", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::MALAY_ARCHIPELAGO, // Christmas Island
            cou::Popul::SETTLERS },
                // ↑ WP: In writing his account, he found the island uninhabited.
    { "CY", cou::Type::RECOGNIZED_STATE, cou::Location::MEDITERRANEAN_SEA, // Cyprus
            cou::Popul::DFLT },
                // ↑ Obvious: cradle of Western civilization
    { "CZ", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Czechia
            cou::Popul::DFLT },
                // ↑ Continent
    { "DE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Germany
            cou::Popul::DFLT },
                // ↑ Continent
    { "DG", cou::Type::MAURITIUS_GB, cou::Location::INDIAN_OCEAN, // Diego Garcia
            cou::Popul::MAINLY_MILITARY },
                // ↑ WP: No tangible evidence exists of people … before the arrival of Europeans
    { "DJ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Djibouti
            cou::Popul::DFLT },
                // ↑ Continent
    { "DK", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Denmark
            cou::Popul::DFLT },
                // ↑ Continent
    { "DM", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Dominica
            cou::Popul::DFLT },
                // ↑ WP: But, as European explorers and settlers entered the region, indigenous
                // refugees from surrounding islands settled Dominica and pushed out the
                // Spanish settlers.
                // So let it be
    { "DO", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Dominican Republic
            cou::Popul::DFLT },
                // ↑ Another island though close!!!
                // WP: Initially, after friendly relationships, the Taínos resisted the conquest
    { "DZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Algeria
            cou::Popul::DFLT },
                // ↑ Continent
    { "EA", cou::Type::SPAIN_AUTONOMY, cou::Location::AFRICA, // Ceuta & Melilla
            cou::Popul::DFLT },
                // ↑ Don’t want to quote but Ceuta is an ancient city
    { "EC", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Ecuador
            cou::Popul::DFLT },
                // ↑ Continent
    { "EE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Estonia
            cou::Popul::DFLT },
                // ↑ Continent
    { "EG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Egypt
            cou::Popul::DFLT },
                // ↑ Continent
    { "EH", cou::Type::PARTLY_WEST_SAHARA, cou::Location::AFRICA, // Western Sahara
            cou::Popul::MAINLY_NOMADS },
                // ↑ Don’t want to write, it’s a sad story
    { "ER", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Eritrea
            cou::Popul::DFLT },
                // ↑ Continent
    { "ES", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Spain
            cou::Popul::DFLT },
                // ↑ Continent
    { "ET", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Ethiopia
            cou::Popul::DFLT },
                // ↑ Continent
    { "EU", cou::Type::SUPRANATIONAL_UNION, cou::Location::EUROPE, // European Union
            cou::Popul::DFLT },
                // ↑ Continent
    { "FI", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Finland
            cou::Popul::DFLT },
                // ↑ Continent
    { "FJ", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA, // Fiji
            cou::Popul::DFLT },
                // ↑ WP: Early interaction with Europeans
    { "FK", cou::Type::BRITAIN_OVERSEAS_ACTUALLY, cou::Location::ATLANTIC_SOUTH_AMERICA, // Falkland Islands
            cou::Popul::SETTLERS },
                // ↑ WP: the islands were uninhabited when Europeans first explored them.
    { "FM", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA, // Micronesia
            cou::Popul::DFLT },
                // ↑ WP: Yap is known for its stone money
    { "FO", cou::Type::DENMARK_AUTONOMY, cou::Location::ATLANTIC_EUROPE, // Faroe Islands
            cou::Popul::DFLT },
                // ↑ WP: Christianity was introduced to the islands in the late 10th
                // and early 11th centuries
    { "FR", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // France
            cou::Popul::DFLT },
                // ↑ Continent
    { "GA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Gabon
            cou::Popul::DFLT },
                // ↑ Continent
    { "GB", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_EUROPE, // United Kingdom
            cou::Popul::DFLT },
                // ↑ Obvious
    { "GD", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Grenada
            cou::Popul::DFLT },
                // ↑ WP: Grenada was inhabited by the indigenous peoples from South America
    { "GE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE_ASIA, // Georgia
            cou::Popul::DFLT },
                // ↑ Continent
    { "GF", cou::Type::FRANCE_OVERSEAS, cou::Location::SOUTH_AMERICA, // French Guiana
            cou::Popul::DFLT },
                // ↑ Continent
    { "GG", cou::Type::BRITAIN_DEPENDENT, cou::Location::ENGLISH_CHANNEL, // Guernsey
            cou::Popul::DFLT },
                // ↑ Don’t want to write but long history
    { "GH", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Ghana
            cou::Popul::DFLT },
                // ↑ Continent
    { "GI", cou::Type::BRITAIN_OVERSEAS_ACTUALLY, cou::Location::EUROPE, // Gibraltar
            cou::Popul::DFLT },
                // ↑ Barren cliff with very long history, everyone
                // attempted to control it
    { "GL", cou::Type::DENMARK_AUTONOMY, cou::Location::ARCTIC_OCEAN, // Greenland
            cou::Popul::DFLT },
                // ↑ AFAIK, long history of colonization
    { "GM", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Gambia
            cou::Popul::DFLT },
                // ↑ Continent
    { "GN", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Guinea
            cou::Popul::DFLT },
                // ↑ Continent
    { "GP", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Guadeloupe
            cou::Popul::DFLT },
                // ↑ WP: Several attempts at colonisation by the Spanish in the 16th
                // century failed due to attacks from the native people
    { "GQ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Equatorial Guinea
            cou::Popul::DFLT },
                // ↑ Continent
    { "GR", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Greece
            cou::Popul::DFLT },
                // ↑ Continent
    { "GS", cou::Type::BRITAIN_OVERSEAS_ACTUALLY, cou::Location::ATLANTIC_ANTARCTIC, // South Georgia & South Sandwich Islands
            cou::Popul::ABANDONED_GS },
                // ↑ Don’t want to write but checked long ago, special strings!
    { "GT", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Guatemala
            cou::Popul::DFLT },
                // ↑ Continent
    { "GU", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_MICRONESIA, // Guam
            cou::Popul::DFLT },
    { "GW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Guinea-Bissau
            cou::Popul::DFLT },
    { "GY", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Guyana
            cou::Popul::DFLT },
    { "HK", cou::Type::CHINA_SPECIAL, cou::Location::ASIA, // Hong Kong SAR China
            cou::Popul::DFLT },
    { "HM", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::INDIAN_ANTARCTIC, // Heard & McDonald Islands
            cou::Popul::UNINHABITED },
    { "HN", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Honduras
            cou::Popul::DFLT },
    { "HR", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Croatia
            cou::Popul::DFLT },
    { "HT", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Haiti
            cou::Popul::DFLT },
                // ↑ WP: Relations with the native peoples, initially good, broke down
                // and the settlers were later killed by the Taíno.
    { "HU", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Hungary
            cou::Popul::DFLT },
    { "IC", cou::Type::SPAIN_AUTONOMY, cou::Location::ATLANTIC_AFRICA, // Canary Islands
            cou::Popul::DFLT },
                // ↑ WP: These invasions were "brutal cultural and military clashes between the
                // indigenous population and the Castilians" lasting over a century
    { "ID", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO, // Indonesia
            cou::Popul::DFLT },
    { "IE", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_BRITISH, // Ireland
            cou::Popul::DFLT },
                // ↑ Obvious
    { "IL", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Israel
            cou::Popul::DFLT },
                // ↑ Continent
    { "IM", cou::Type::BRITAIN_DEPENDENT, cou::Location::ATLANTIC_BRITISH, // Isle of Man
            cou::Popul::DFLT },
                // ↑ Long history, believe me
    { "IN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // India
            cou::Popul::DFLT },
                // ↑ Continent
    { "IO", cou::Type::BRITAIN_OVERSEAS_FORMER, cou::Location::INDIAN_OCEAN, // British Indian Ocean Territory
            cou::Popul::MAINLY_MILITARY },
                // ↑ Long story ended in 2024
    { "IQ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Iraq
            cou::Popul::DFLT },
    { "IR", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Iran
            cou::Popul::DFLT },
    { "IS", cou::Type::RECOGNIZED_STATE, cou::Location::ARCTIC_OCEAN, // Iceland
            cou::Popul::DFLT },
    { "IT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Italy
            cou::Popul::DFLT },
    { "JE", cou::Type::BRITAIN_DEPENDENT, cou::Location::ENGLISH_CHANNEL, // Jersey
            cou::Popul::DFLT },
    { "JM", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Jamaica
            cou::Popul::DFLT },
                // ↑ WP: Many slaves managed to escape, forming autonomous communities
                // in remote and easily defended areas in the interior of Jamaica,
                // mixing with the remaining Taino;
    { "JO", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Jordan
            cou::Popul::DFLT },
    { "JP", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_ASIA, // Japan
            cou::Popul::DFLT },
    { "KE", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Kenya
            cou::Popul::DFLT },
    { "KG", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Kyrgyzstan
            cou::Popul::DFLT },
    { "KH", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Cambodia
            cou::Popul::DFLT },
    { "KI", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA, // Kiribati
            cou::Popul::DFLT },
    { "KM", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA, // Comoros
            cou::Popul::DFLT },
    { "KN", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // St. Kitts & Nevis
            cou::Popul::DFLT },
                // ↑ WP: The French and English, intent on exploitation of the island's resources,
                // encountered resistance from the native Caribs (Kalinago),
    { "KP", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // North Korea
            cou::Popul::DFLT },
    { "KR", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // South Korea
            cou::Popul::DFLT },
    { "KW", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Kuwait
            cou::Popul::DFLT },
    { "KY", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Cayman Islands
            cou::Popul::SETTLERS },
                // ↑ WP: As of 2017, no evidence has been found that the islands
                // had been occupied before their discovery by Europeans
    { "KZ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Kazakhstan
            cou::Popul::DFLT },
                // ↑ Continent
    { "LA", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Laos
            cou::Popul::DFLT },
                // ↑ Continent
    { "LB", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Lebanon
            cou::Popul::DFLT },
                // ↑ Continent
    { "LC", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // St. Lucia
            cou::Popul::DFLT },
                // ↑ WP: In 1605, an English vessel called the Oliphe Blossome was blown off-course
                // on its way to Guyana, and the 67 colonists started a settlement on Saint Lucia,
                // after initially being welcomed by the Carib chief Anthonie.
    { "LI", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Liechtenstein
            cou::Popul::DFLT },
    { "LK", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_ASIA, // Sri Lanka
            cou::Popul::DFLT },
    { "LR", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Liberia
            cou::Popul::DFLT },
    { "LS", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Lesotho
            cou::Popul::DFLT },
    { "LT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Lithuania
            cou::Popul::DFLT },
    { "LU", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Luxembourg
            cou::Popul::DFLT },
    { "LV", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Latvia
            cou::Popul::DFLT },
    { "LY", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Libya
            cou::Popul::DFLT },
    { "MA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Morocco
            cou::Popul::DFLT },
    { "MC", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Monaco
            cou::Popul::DFLT },
    { "MD", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Moldova
            cou::Popul::DFLT },
    { "ME", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Montenegro
            cou::Popul::DFLT },
    { "MF", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // St. Martin
            cou::Popul::DFLT },
                // ↑ WP: When Europeans began to explore the Caribbean, Carib society
                // had almost completely displaced the Arawaks.
    { "MG", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA, // Madagascar
            cou::Popul::DFLT },
    { "MH", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA, // Marshall Islands
            cou::Popul::DFLT },
    { "MK", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // North Macedonia
            cou::Popul::DFLT },
    { "ML", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Mali
            cou::Popul::DFLT },
    { "MM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Myanmar (Burma)
            cou::Popul::DFLT },
    { "MN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Mongolia
            cou::Popul::DFLT },
    { "MO", cou::Type::CHINA_SPECIAL, cou::Location::ASIA, // Macao SAR China
            cou::Popul::DFLT },
    { "MP", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_MICRONESIA, // Northern Mariana Islands
            cou::Popul::DFLT },
    { "MQ", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Martinique
            cou::Popul::DFLT },
                // ↑ WP: The Indigenous people Columbus encountered called Martinique "Matinino".
    { "MR", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Mauritania
            cou::Popul::DFLT },
    { "MS", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Montserrat
            cou::Popul::SETTLERS },
                // ↑ WP: after being told that the island was unoccupied because of raids by the Caribs.
    { "MT", cou::Type::RECOGNIZED_STATE, cou::Location::MEDITERRANEAN_SEA, // Malta
            cou::Popul::DFLT },
    { "MU", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA, // Mauritius
            cou::Popul::SETTLERS },
                // ↑ WP: In 1507, Portuguese sailors came to the uninhabited island
                // and established a visiting base.
    { "MV", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_ASIA, // Maldives
            cou::Popul::DFLT },
    { "MW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Malawi
            cou::Popul::DFLT },
    { "MX", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Mexico
            cou::Popul::DFLT },
    { "MY", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA_MALAY_ARCHIPELAGO, // Malaysia
            cou::Popul::DFLT },
    { "MZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Mozambique
            cou::Popul::DFLT },
    { "NA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Namibia
            cou::Popul::DFLT },
    { "NC", cou::Type::FRANCE_OVERSEAS, cou::Location::PACIFIC_AUSTRALIA, // New Caledonia
            cou::Popul::DFLT },
    { "NE", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Niger
            cou::Popul::DFLT },
    { "NF", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::PACIFIC_AUSTRALIA, // Norfolk Island
            cou::Popul::SETTLERS },
                // ↑ WP: Norfolk Island was uninhabited when first settled by Europeans,
                // but evidence of earlier habitation was obvious.
    { "NG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Nigeria
            cou::Popul::DFLT },
    { "NI", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Nicaragua
            cou::Popul::DFLT },
    { "NL", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Netherlands
            cou::Popul::DFLT },
    { "NO", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Norway
            cou::Popul::DFLT },
    { "NP", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Nepal
            cou::Popul::DFLT },
    { "NR", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA, // Nauru
            cou::Popul::DFLT },
    { "NU", cou::Type::NEWZEALAND_ASSOC, cou::Location::PACIFIC_POLYNESIA, // Niue
            cou::Popul::DFLT },
    { "NZ", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA, // New Zealand
            cou::Popul::DFLT },
    { "OM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Oman
            cou::Popul::DFLT },
    { "PA", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Panama
            cou::Popul::DFLT },
    { "PE", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Peru
            cou::Popul::DFLT },
    { "PF", cou::Type::FRANCE_OVERSEAS, cou::Location::PACIFIC_POLYNESIA, // French Polynesia
            cou::Popul::DFLT },
    { "PG", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO, // Papua New Guinea
            cou::Popul::DFLT },
    { "PH", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO, // Philippines
            cou::Popul::DFLT },
    { "PK", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Pakistan
            cou::Popul::DFLT },
    { "PL", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Poland
            cou::Popul::DFLT },
    { "PM", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_NORTH_AMERICA, // St. Pierre & Miquelon
            cou::Popul::SETTLERS },
                // ↑ WP: However, it is not thought that they settled on the islands permanently.
    { "PN", cou::Type::BRITAIN_OVERSEAS, cou::Location::PACIFIC_POLYNESIA, // Pitcairn Islands
            cou::Popul::SETTLERS },
                // ↑ WP: The islands were uninhabited when they were discovered by Europeans.
    { "PR", cou::Type::US_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN, // Puerto Rico
            cou::Popul::DFLT },
                // ↑ WP: At the time of Christopher Columbus's arrival in the New World in 1493,
                // the dominant indigenous culture was that of the Taínos.
    { "PS", cou::Type::PARTLY_PALESTINE, cou::Location::ASIA, // Palestinian Territories
            cou::Popul::DFLT },
    { "PT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Portugal
            cou::Popul::DFLT },
    { "PW", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA, // Palau
            cou::Popul::DFLT },
    { "PY", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Paraguay
            cou::Popul::DFLT },
    { "QA", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Qatar
            cou::Popul::DFLT },
    { "RE", cou::Type::FRANCE_OVERSEAS, cou::Location::INDIAN_AFRICA, // Réunion
            cou::Popul::SETTLERS }, // WP: The island has been inhabited since the 17th century
    { "RO", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Romania
            cou::Popul::DFLT },
    { "RS", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Serbia
            cou::Popul::DFLT },
    { "RU", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE_ASIA, // Russia
            cou::Popul::DFLT },
    { "RW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Rwanda
            cou::Popul::DFLT },
    { "SA", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Saudi Arabia
            cou::Popul::DFLT },
    { "SB", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA, // Solomon Islands
            cou::Popul::DFLT },
    { "SC", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA, // Seychelles
            cou::Popul::SETTLERS },
                // ↑ WP: Seychelles was uninhabited until the 18th century when
                // Europeans arrived with Indians
    { "SD", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Sudan
            cou::Popul::DFLT },
    { "SE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Sweden
            cou::Popul::DFLT },
    { "SG", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Singapore
            cou::Popul::DFLT },
    { "SH", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_OCEAN, // St. Helena
            cou::Popul::SETTLERS },
                // ↑ WP: The Portuguese found the island uninhabited
    { "SI", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Slovenia
            cou::Popul::DFLT },
    { "SJ", cou::Type::NORWAY_DEPENDENT, cou::Location::ARCTIC_OCEAN, // Svalbard & Jan Mayen
            cou::Popul::SETTLERS_SVALBARD },
                // WP: was discovered multiple times → probably no men
    { "SK", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Slovakia
            cou::Popul::DFLT },
    { "SL", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Sierra Leone
            cou::Popul::DFLT },
    { "SM", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // San Marino
            cou::Popul::DFLT },
    { "SN", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Senegal
            cou::Popul::DFLT },
    { "SO", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Somalia
            cou::Popul::DFLT },
    { "SR", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Suriname
            cou::Popul::DFLT },
    { "SS", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // South Sudan
            cou::Popul::DFLT },
    { "ST", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_AFRICA, // São Tomé & Príncipe
            cou::Popul::SETTLERS },
                // ↑ WP: The islands of São Tomé and Príncipe were uninhabited when
                // the Portuguese arrived sometime around 1470
    { "SV", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // El Salvador
            cou::Popul::DFLT },
    { "SX", cou::Type::NETHERLANDS_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Sint Maarten
                // ↑ See French part
            cou::Popul::DFLT },
    { "SY", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Syria
            cou::Popul::DFLT },
    { "SZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Eswatini
            cou::Popul::DFLT },
    { "TA", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_OCEAN, // Tristan da Cunha
            cou::Popul::SETTLERS },
                // ↑ WP: The uninhabited islands were first recorded as sighted in 1506
    { "TC", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_NORTH_AMERICA, // Turks & Caicos Islands
            cou::Popul::DFLT },
                // ↑ WP: In either case, by 1512 the Spanish had begun capturing the Taíno and Lucayans
    { "TD", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Chad
            cou::Popul::DFLT },
    { "TF", cou::Type::FRANCE_UNINHABITED, cou::Location::INDIAN_ANTARCTIC, // French Southern Territories
            cou::Popul::MAINLY_SCIENTISTS },
                // ↑ Antarctic :)
    { "TG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Togo
            cou::Popul::DFLT },
    { "TH", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Thailand
            cou::Popul::DFLT },
    { "TJ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Tajikistan
            cou::Popul::DFLT },
    { "TK", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_POLYNESIA, // Tokelau
            cou::Popul::DFLT },
    { "TL", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO, // Timor-Leste
            cou::Popul::DFLT },
    { "TM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Turkmenistan
            cou::Popul::DFLT },
    { "TN", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Tunisia
            cou::Popul::DFLT },
    { "TO", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_POLYNESIA, // Tonga
            cou::Popul::DFLT },
    { "TR", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Türkiye
            cou::Popul::DFLT },
    { "TT", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Trinidad & Tobago
            cou::Popul::DFLT },
                // ↑ WP: …intending to subdue the Indigenous population of the island.
    { "TV", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_POLYNESIA, // Tuvalu
            cou::Popul::DFLT },
    { "TW", cou::Type::PARTLY_TAIWAN, cou::Location::PACIFIC_ASIA, // Taiwan
            cou::Popul::DFLT },
    { "TZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Tanzania
            cou::Popul::DFLT },
    { "UA", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Ukraine
            cou::Popul::DFLT },
    { "UG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Uganda
            cou::Popul::DFLT },
    { "UM", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_POLYNESIA, // U.S. Outlying Islands
            cou::Popul::MAINLY_MILITARY },
    { "UN", cou::Type::INTERNATIONAL_ORGANIZATION, cou::Location::NEWYORK_HQ, // United Nations
            cou::Popul::NOT_APPLICABLE },
    { "US", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // United States
            cou::Popul::DFLT },
    { "UY", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Uruguay
            cou::Popul::DFLT },
    { "UZ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Uzbekistan
            cou::Popul::DFLT },
    { "VA", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Vatican City
            cou::Popul::MAINLY_PRIESTS },
    { "VC", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // St. Vincent & Grenadines
            cou::Popul::DFLT },
                // ↑ WP: The Kalinago people aggressively opposed European settlement on Saint Vincent.
    { "VE", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Venezuela
            cou::Popul::DFLT },
    { "VG", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // British Virgin Islands
            cou::Popul::DFLT },
                // ↑ WP: There is no record of any native Amerindian population in the British Virgin
                // Islands during this period; it is thought that they either fled to safer islands
                // or were killed.
                // So let it be
    { "VI", cou::Type::US_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN, // U.S. Virgin Islands
            cou::Popul::DFLT },
                // ↑ WP: The Kalinago (Island Caribs) arrived around the mid-15th century AD.
                // and some more Amerindians before
    { "VN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Vietnam
            cou::Popul::DFLT },
    { "VU", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA, // Vanuatu
            cou::Popul::DFLT },
    { "WF", cou::Type::FRANCE_OVERSEAS, cou::Location::PACIFIC_POLYNESIA, // Wallis & Futuna
            cou::Popul::DFLT },
    { "WS", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_POLYNESIA, // Samoa
            cou::Popul::DFLT },
    { "XK", cou::Type::PARTLY_KOSOVO, cou::Location::EUROPE, // Kosovo
            cou::Popul::DFLT },
    { "YE", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Yemen
            cou::Popul::DFLT },
    { "YT", cou::Type::FRANCE_OVERSEAS, cou::Location::INDIAN_AFRICA, // Mayotte
            cou::Popul::DFLT },
    { "ZA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // South Africa
            cou::Popul::DFLT },
    { "ZM", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Zambia
            cou::Popul::DFLT },
    { "ZW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Zimbabwe
            cou::Popul::DFLT },
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
