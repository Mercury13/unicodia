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
    TX_STA "GbDep",
    TX_STA "CnSpec",
    TX_STA "DkAuto", // 10
    TX_STA "FiSpec",
    TX_STA "FrOv",
    TX_STA "FrUn",
    TX_STA "NlDep",
    TX_STA "NlOv", // 15
    TX_STA "NzAssoc",
    TX_STA "NoUn",
    TX_STA "NoDep",
    TX_STA "EsAuto",
    TX_STA "UsDep", // 20
    TX_STA "PartPs",
    TX_STA "PartEh",
    TX_STA "PartTw",
    TX_STA "PartXk", // 24
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
    { "AD", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Andorra
            cou::Popul::DFLT },
    { "AE", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // United Arab Emirates
            cou::Popul::DFLT },
    { "AF", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Afghanistan
            cou::Popul::DFLT },
    { "AG", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Antigua & Barbuda
            cou::Popul::DFLT },
    { "AI", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Anguilla
            cou::Popul::DFLT },
    { "AL", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Albania
            cou::Popul::DFLT },
    { "AM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Armenia
            cou::Popul::DFLT },
    { "AO", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Angola
            cou::Popul::DFLT },
    { "AQ", cou::Type::NO_MANS_LAND, cou::Location::SOUTH_POLE, // Antarctica
            cou::Popul::MAINLY_SCIENTISTS },
    { "AR", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Argentina
            cou::Popul::DFLT },
    { "AS", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_POLYNESIA, // American Samoa
            cou::Popul::DFLT },
    { "AT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Austria
            cou::Popul::DFLT },
    { "AU", cou::Type::RECOGNIZED_STATE, cou::Location::AUSTRALIA, // Australia
            cou::Popul::DFLT },
    { "AW", cou::Type::NETHERLANDS_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN, // Aruba
            cou::Popul::DFLT },
    { "AX", cou::Type::FINLAND_SPECIAL, cou::Location::BALTIC_SEA, // Åland Islands
            cou::Popul::DFLT },
    { "AZ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Azerbaijan
            cou::Popul::DFLT },
    { "BA", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Bosnia & Herzegovina
            cou::Popul::DFLT },
    { "BB", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Barbados
            cou::Popul::DFLT },
    { "BD", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Bangladesh
            cou::Popul::DFLT },
    { "BE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Belgium
            cou::Popul::DFLT },
    { "BF", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Burkina Faso
            cou::Popul::DFLT },
    { "BG", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Bulgaria
            cou::Popul::DFLT },
    { "BH", cou::Type::RECOGNIZED_STATE, cou::Location::GULF_OF_PERSIA, // Bahrain
            cou::Popul::DFLT },
    { "BI", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Burundi
            cou::Popul::DFLT },
    { "BJ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Benin
            cou::Popul::DFLT },
    { "BL", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // St. Barthélemy
            cou::Popul::DFLT },
    { "BM", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_NORTH_AMERICA, // Bermuda
            cou::Popul::SETTLERS },
    { "BN", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO, // Brunei
            cou::Popul::DFLT },
    { "BO", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Bolivia
            cou::Popul::DFLT },
    { "BQ", cou::Type::NETHERLANDS_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Caribbean Netherlands
            cou::Popul::DFLT },
    { "BR", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Brazil
            cou::Popul::DFLT },
    { "BS", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_NORTH_AMERICA, // Bahamas
            cou::Popul::DFLT },
    { "BT", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Bhutan
            cou::Popul::DFLT },
    { "BV", cou::Type::NORWAY_UNINHABITED, cou::Location::ATLANTIC_ANTARCTIC, // Bouvet Island
            cou::Popul::NATURE_RESERVE },
    { "BW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Botswana
            cou::Popul::DFLT },
    { "BY", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Belarus
            cou::Popul::DFLT },
    { "BZ", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Belize
            cou::Popul::DFLT },
    { "CA", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Canada
            cou::Popul::DFLT },
    { "CC", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::INDIAN_OCEAN, // Cocos (Keeling) Islands
            cou::Popul::SETTLERS },
    { "CD", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Congo - Kinshasa
            cou::Popul::DFLT },
    { "CF", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Central African Republic
            cou::Popul::DFLT },
    { "CG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Congo - Brazzaville
            cou::Popul::DFLT },
    { "CH", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Switzerland
            cou::Popul::DFLT },
    { "CI", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Côte d’Ivoire
            cou::Popul::DFLT },
    { "CK", cou::Type::NEWZEALAND_ASSOC, cou::Location::PACIFIC_POLYNESIA, // Cook Islands
            cou::Popul::DFLT },
    { "CL", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Chile
            cou::Popul::DFLT },
    { "CM", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Cameroon
            cou::Popul::DFLT },
    { "CN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // China
            cou::Popul::DFLT },
    { "CO", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Colombia
            cou::Popul::DFLT },
    { "CP", cou::Type::FRANCE_UNINHABITED, cou::Location::PACIFIC_NORTH_AMERICA, // Clipperton Island
            cou::Popul::UNINHABITED },
    { "CQ", cou::Type::BRITAIN_DEPENDENT, cou::Location::ENGLISH_CHANNEL, // Sark
            cou::Popul::DFLT },
    { "CR", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Costa Rica
            cou::Popul::DFLT },
    { "CU", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Cuba
            cou::Popul::DFLT },
    { "CV", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_AFRICA, // Cape Verde
            cou::Popul::DFLT },
    { "CW", cou::Type::NETHERLANDS_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN, // Curaçao
            cou::Popul::DFLT },
    { "CX", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::MALAY_ARCHIPELAGO, // Christmas Island
            cou::Popul::SETTLERS },
    { "CY", cou::Type::RECOGNIZED_STATE, cou::Location::MEDITERRANEAN_SEA, // Cyprus
            cou::Popul::DFLT },
    { "CZ", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Czechia
            cou::Popul::DFLT },
    { "DE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Germany
            cou::Popul::DFLT },
    { "DG", cou::Type::BRITAIN_OVERSEAS_ACTUALLY, cou::Location::INDIAN_OCEAN, // Diego Garcia
            cou::Popul::MAINLY_MILITARY },
    { "DJ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Djibouti
            cou::Popul::DFLT },
    { "DK", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Denmark
            cou::Popul::DFLT },
    { "DM", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Dominica
            cou::Popul::DFLT },
    { "DO", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Dominican Republic
            cou::Popul::DFLT },
    { "DZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Algeria
            cou::Popul::DFLT },
    { "EA", cou::Type::SPAIN_AUTONOMY, cou::Location::AFRICA, // Ceuta & Melilla
            cou::Popul::DFLT },
    { "EC", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Ecuador
            cou::Popul::DFLT },
    { "EE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Estonia
            cou::Popul::DFLT },
    { "EG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Egypt
            cou::Popul::DFLT },
    { "EH", cou::Type::PARTLY_WEST_SAHARA, cou::Location::AFRICA, // Western Sahara
            cou::Popul::MAINLY_NOMADS },
    { "ER", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Eritrea
            cou::Popul::DFLT },
    { "ES", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Spain
            cou::Popul::DFLT },
    { "ET", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Ethiopia
            cou::Popul::DFLT },
    { "EU", cou::Type::SUPRANATIONAL_UNION, cou::Location::EUROPE, // European Union
            cou::Popul::DFLT },
    { "FI", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Finland
            cou::Popul::DFLT },
    { "FJ", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA, // Fiji
            cou::Popul::DFLT },
    { "FK", cou::Type::BRITAIN_OVERSEAS_ACTUALLY, cou::Location::ATLANTIC_SOUTH_AMERICA, // Falkland Islands
            cou::Popul::SETTLERS },
    { "FM", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA, // Micronesia
            cou::Popul::DFLT },
    { "FO", cou::Type::DENMARK_AUTONOMY, cou::Location::ATLANTIC_EUROPE, // Faroe Islands
            cou::Popul::DFLT },
    { "FR", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // France
            cou::Popul::DFLT },
    { "GA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Gabon
            cou::Popul::DFLT },
    { "GB", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_EUROPE, // United Kingdom
            cou::Popul::DFLT },
    { "GD", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // Grenada
            cou::Popul::DFLT },
    { "GE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE_ASIA, // Georgia
            cou::Popul::DFLT },
    { "GF", cou::Type::FRANCE_OVERSEAS, cou::Location::SOUTH_AMERICA, // French Guiana
            cou::Popul::DFLT },
    { "GG", cou::Type::BRITAIN_DEPENDENT, cou::Location::ENGLISH_CHANNEL, // Guernsey
            cou::Popul::DFLT },
    { "GH", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Ghana
            cou::Popul::DFLT },
    { "GI", cou::Type::BRITAIN_OVERSEAS_ACTUALLY, cou::Location::EUROPE, // Gibraltar
            cou::Popul::DFLT },
    { "GL", cou::Type::DENMARK_AUTONOMY, cou::Location::ARCTIC_OCEAN, // Greenland
            cou::Popul::DFLT },
    { "GM", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Gambia
            cou::Popul::DFLT },
    { "GN", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Guinea
            cou::Popul::DFLT },
    { "GP", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Guadeloupe
            cou::Popul::DFLT },
    { "GQ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Equatorial Guinea
            cou::Popul::DFLT },
    { "GR", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Greece
            cou::Popul::DFLT },
    { "GS", cou::Type::BRITAIN_OVERSEAS_ACTUALLY, cou::Location::ATLANTIC_ANTARCTIC, // South Georgia & South Sandwich Islands
            cou::Popul::ABANDONED_GS },
    { "GT", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // Guatemala
            cou::Popul::DFLT },
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
    { "HU", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Hungary
            cou::Popul::DFLT },
    { "IC", cou::Type::SPAIN_AUTONOMY, cou::Location::ATLANTIC_AFRICA, // Canary Islands
            cou::Popul::DFLT },
    { "ID", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO, // Indonesia
            cou::Popul::DFLT },
    { "IE", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_BRITISH, // Ireland
            cou::Popul::DFLT },
    { "IL", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Israel
            cou::Popul::DFLT },
    { "IM", cou::Type::BRITAIN_DEPENDENT, cou::Location::ATLANTIC_BRITISH, // Isle of Man
            cou::Popul::DFLT },
    { "IN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // India
            cou::Popul::DFLT },
    { "IO", cou::Type::BRITAIN_OVERSEAS, cou::Location::INDIAN_OCEAN, // British Indian Ocean Territory
            cou::Popul::MAINLY_MILITARY },
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
    { "KP", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // North Korea
            cou::Popul::DFLT },
    { "KR", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // South Korea
            cou::Popul::DFLT },
    { "KW", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Kuwait
            cou::Popul::DFLT },
    { "KY", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Cayman Islands
            cou::Popul::DFLT },
    { "KZ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Kazakhstan
            cou::Popul::DFLT },
    { "LA", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Laos
            cou::Popul::DFLT },
    { "LB", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Lebanon
            cou::Popul::DFLT },
    { "LC", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN, // St. Lucia
            cou::Popul::DFLT },
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
    { "MR", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Mauritania
            cou::Popul::DFLT },
    { "MS", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Montserrat
            cou::Popul::SETTLERS },
    { "MT", cou::Type::RECOGNIZED_STATE, cou::Location::MEDITERRANEAN_SEA, // Malta
            cou::Popul::DFLT },
    { "MU", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA, // Mauritius
            cou::Popul::SETTLERS },
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
    { "PN", cou::Type::BRITAIN_OVERSEAS, cou::Location::PACIFIC_POLYNESIA, // Pitcairn Islands
            cou::Popul::DFLT },
    { "PR", cou::Type::US_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN, // Puerto Rico
            cou::Popul::DFLT },
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
            cou::Popul::SETTLERS },
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
    { "SD", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Sudan
            cou::Popul::DFLT },
    { "SE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE, // Sweden
            cou::Popul::DFLT },
    { "SG", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Singapore
            cou::Popul::DFLT },
    { "SH", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_OCEAN, // St. Helena
            cou::Popul::SETTLERS },
    { "SI", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE_ASIA, // Slovenia
            cou::Popul::DFLT },
    { "SJ", cou::Type::NORWAY_DEPENDENT, cou::Location::ARCTIC_OCEAN, // Svalbard & Jan Mayen
            cou::Popul::SETTLERS_SVALBARD },
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
    { "SV", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA, // El Salvador
            cou::Popul::DFLT },
    { "SX", cou::Type::NETHERLANDS_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // Sint Maarten
            cou::Popul::DFLT },
    { "SY", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA, // Syria
            cou::Popul::DFLT },
    { "SZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Eswatini
            cou::Popul::DFLT },
    { "TA", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_OCEAN, // Tristan da Cunha
            cou::Popul::SETTLERS },
    { "TC", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_NORTH_AMERICA, // Turks & Caicos Islands
            cou::Popul::DFLT },
    { "TD", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA, // Chad
            cou::Popul::DFLT },
    { "TF", cou::Type::FRANCE_UNINHABITED, cou::Location::INDIAN_ANTARCTIC, // French Southern Territories
            cou::Popul::MAINLY_SCIENTISTS },
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
    { "VE", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA, // Venezuela
            cou::Popul::DFLT },
    { "VG", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN, // British Virgin Islands
            cou::Popul::DFLT },
    { "VI", cou::Type::US_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN, // U.S. Virgin Islands
            cou::Popul::DFLT },
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
