#include "UcData.h"

#define TX_STA "Lib.Sta."

constinit const ec::Array<const char*, cou::Type> cou::typeKeys {
    TX_STA "Rec",
    TX_STA "NoMan",
    TX_STA "Union",
    TX_STA "Org",
    TX_STA "AuExt",  // 5
    TX_STA "AuUn",
    TX_STA "GbOv",
    TX_STA "GbDep",
    TX_STA "CnSpec",
    TX_STA "DkAuto", // 10,
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
    TX_STA "DisGbMu",
    TX_STA "PartPs",
    TX_STA "PartEh",
    TX_STA "PartTw",
    TX_STA "PartXk",
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


constinit const cou::Country ALL_COUNTRIES[] {
    { "AC", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_OCEAN }, // Ascension Island
    { "AD", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Andorra
    { "AE", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // United Arab Emirates
    { "AF", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Afghanistan
    { "AG", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN }, // Antigua & Barbuda
    { "AI", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN }, // Anguilla
    { "AL", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Albania
    { "AM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Armenia
    { "AO", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Angola
    { "AQ", cou::Type::NO_MANS_LAND, cou::Location::SOUTH_POLE }, // Antarctica
    { "AR", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA }, // Argentina
    { "AS", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_POLYNESIA }, // American Samoa
    { "AT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Austria
    { "AU", cou::Type::RECOGNIZED_STATE, cou::Location::AUSTRALIA }, // Australia
    { "AW", cou::Type::NETHERLANDS_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN }, // Aruba
    { "AX", cou::Type::FINLAND_SPECIAL, cou::Location::BALTIC_SEA }, // Åland Islands
    { "AZ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Azerbaijan
    { "BA", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Bosnia & Herzegovina
    { "BB", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN }, // Barbados
    { "BD", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Bangladesh
    { "BE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Belgium
    { "BF", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Burkina Faso
    { "BG", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Bulgaria
    { "BH", cou::Type::RECOGNIZED_STATE, cou::Location::GULF_OF_PERSIA }, // Bahrain
    { "BI", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Burundi
    { "BJ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Benin
    { "BL", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN }, // St. Barthélemy
    { "BM", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_NORTH_AMERICA }, // Bermuda
    { "BN", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO }, // Brunei
    { "BO", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA }, // Bolivia
    { "BQ", cou::Type::NETHERLANDS_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN }, // Caribbean Netherlands
    { "BR", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA }, // Brazil
    { "BS", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_NORTH_AMERICA }, // Bahamas
    { "BT", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Bhutan
    { "BV", cou::Type::NORWAY_UNINHABITED, cou::Location::ATLANTIC_ANTARCTIC }, // Bouvet Island
    { "BW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Botswana
    { "BY", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Belarus
    { "BZ", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA }, // Belize
    { "CA", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA }, // Canada
    { "CC", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::INDIAN_OCEAN }, // Cocos (Keeling) Islands
    { "CD", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Congo - Kinshasa
    { "CF", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Central African Republic
    { "CG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Congo - Brazzaville
    { "CH", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Switzerland
    { "CI", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Côte d’Ivoire
    { "CK", cou::Type::NEWZEALAND_ASSOC, cou::Location::PACIFIC_POLYNESIA }, // Cook Islands
    { "CL", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA }, // Chile
    { "CM", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Cameroon
    { "CN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // China
    { "CO", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA }, // Colombia
    { "CP", cou::Type::FRANCE_UNINHABITED, cou::Location::PACIFIC_NORTH_AMERICA }, // Clipperton Island
    { "CR", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA }, // Costa Rica
    { "CU", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN }, // Cuba
    { "CV", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_AFRICA }, // Cape Verde
    { "CW", cou::Type::NETHERLANDS_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN }, // Curaçao
    { "CX", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::MALAY_ARCHIPELAGO }, // Christmas Island
    { "CY", cou::Type::RECOGNIZED_STATE, cou::Location::MEDITERRANEAN_SEA }, // Cyprus
    { "CZ", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Czechia
    { "DE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Germany
    { "DG", cou::Type::DISPUTE_BRITAIN_MAURITIUS, cou::Location::INDIAN_OCEAN }, // Diego Garcia
    { "DJ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Djibouti
    { "DK", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Denmark
    { "DM", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN }, // Dominica
    { "DO", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN }, // Dominican Republic
    { "DZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Algeria
    { "EA", cou::Type::SPAIN_AUTONOMY, cou::Location::AFRICA }, // Ceuta & Melilla
    { "EC", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA }, // Ecuador
    { "EE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Estonia
    { "EG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Egypt
    { "EH", cou::Type::PARTLY_WEST_SAHARA, cou::Location::AFRICA }, // Western Sahara
    { "ER", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Eritrea
    { "ES", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Spain
    { "ET", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Ethiopia
    { "EU", cou::Type::SUPRANATIONAL_UNION, cou::Location::EUROPE }, // European Union
    { "FI", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Finland
    { "FJ", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA }, // Fiji
    { "FK", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_SOUTH_AMERICA }, // Falkland Islands
    { "FM", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA }, // Micronesia
    { "FO", cou::Type::DENMARK_AUTONOMY, cou::Location::ATLANTIC_EUROPE }, // Faroe Islands
    { "FR", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // France
    { "GA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Gabon
    { "GB", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_EUROPE }, // United Kingdom
    { "GD", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN }, // Grenada
    { "GE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE_ASIA }, // Georgia
    { "GF", cou::Type::FRANCE_OVERSEAS, cou::Location::SOUTH_AMERICA }, // French Guiana
    { "GG", cou::Type::BRITAIN_DEPENDENT, cou::Location::ENGLISH_CHANNEL }, // Guernsey
    { "GH", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Ghana
    { "GI", cou::Type::BRITAIN_OVERSEAS, cou::Location::EUROPE }, // Gibraltar
    { "GL", cou::Type::DENMARK_AUTONOMY, cou::Location::ARCTIC_OCEAN }, // Greenland
    { "GM", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Gambia
    { "GN", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Guinea
    { "GP", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN }, // Guadeloupe
    { "GQ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Equatorial Guinea
    { "GR", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Greece
    { "GS", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_ANTARCTIC }, // South Georgia & South Sandwich Islands
    { "GT", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA }, // Guatemala
    { "GU", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_MICRONESIA }, // Guam
    { "GW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Guinea-Bissau
    { "GY", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA }, // Guyana
    { "HK", cou::Type::CHINA_SPECIAL, cou::Location::ASIA }, // Hong Kong SAR China
    { "HM", cou::Type::AUSTRALIA_EXTERNAL_UNINHABITED, cou::Location::INDIAN_ANTARCTIC }, // Heard & McDonald Islands
    { "HN", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA }, // Honduras
    { "HR", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Croatia
    { "HT", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN }, // Haiti
    { "HU", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Hungary
    { "IC", cou::Type::SPAIN_AUTONOMY, cou::Location::ATLANTIC_AFRICA }, // Canary Islands
    { "ID", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO }, // Indonesia
    { "IE", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_BRITISH }, // Ireland
    { "IL", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Israel
    { "IM", cou::Type::BRITAIN_DEPENDENT, cou::Location::ATLANTIC_BRITISH }, // Isle of Man
    { "IN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // India
    { "IO", cou::Type::BRITAIN_OVERSEAS, cou::Location::INDIAN_OCEAN }, // British Indian Ocean Territory
    { "IQ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Iraq
    { "IR", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Iran
    { "IS", cou::Type::RECOGNIZED_STATE, cou::Location::ARCTIC_OCEAN }, // Iceland
    { "IT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Italy
    { "JE", cou::Type::BRITAIN_DEPENDENT, cou::Location::ENGLISH_CHANNEL }, // Jersey
    { "JM", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN }, // Jamaica
    { "JO", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Jordan
    { "JP", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_ASIA }, // Japan
    { "KE", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Kenya
    { "KG", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Kyrgyzstan
    { "KH", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Cambodia
    { "KI", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA }, // Kiribati
    { "KM", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA }, // Comoros
    { "KN", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN }, // St. Kitts & Nevis
    { "KP", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // North Korea
    { "KR", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // South Korea
    { "KW", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA}, // Kuwait
    { "KY", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN }, // Cayman Islands
    { "KZ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Kazakhstan
    { "LA", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Laos
    { "LB", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Lebanon
    { "LC", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN }, // St. Lucia
    { "LI", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Liechtenstein
    { "LK", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_ASIA }, // Sri Lanka
    { "LR", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Liberia
    { "LS", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Lesotho
    { "LT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Lithuania
    { "LU", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Luxembourg
    { "LV", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Latvia
    { "LY", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Libya
    { "MA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Morocco
    { "MC", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Monaco
    { "MD", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Moldova
    { "ME", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Montenegro
    { "MF", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN }, // St. Martin
    { "MG", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA }, // Madagascar
    { "MH", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA }, // Marshall Islands
    { "MK", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // North Macedonia
    { "ML", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Mali
    { "MM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Myanmar (Burma)
    { "MN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Mongolia
    { "MO", cou::Type::CHINA_SPECIAL, cou::Location::ASIA }, // Macao SAR China
    { "MP", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_MICRONESIA }, // Northern Mariana Islands
    { "MQ", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN }, // Martinique
    { "MR", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Mauritania
    { "MS", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN }, // Montserrat
    { "MT", cou::Type::RECOGNIZED_STATE, cou::Location::MEDITERRANEAN_SEA }, // Malta
    { "MU", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA }, // Mauritius
    { "MV", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_ASIA }, // Maldives
    { "MW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Malawi
    { "MX", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA }, // Mexico
    { "MY", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA_MALAY_ARCHIPELAGO }, // Malaysia
    { "MZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Mozambique
    { "NA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Namibia
    { "NC", cou::Type::FRANCE_OVERSEAS, cou::Location::PACIFIC_AUSTRALIA }, // New Caledonia
    { "NE", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Niger
    { "NF", cou::Type::AUSTRALIA_EXTERNAL, cou::Location::PACIFIC_AUSTRALIA }, // Norfolk Island
    { "NG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Nigeria
    { "NI", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA }, // Nicaragua
    { "NL", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Netherlands
    { "NO", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Norway
    { "NP", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Nepal
    { "NR", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA }, // Nauru
    { "NU", cou::Type::NEWZEALAND_ASSOC, cou::Location::PACIFIC_POLYNESIA }, // Niue
    { "NZ", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA }, // New Zealand
    { "OM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Oman
    { "PA", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA }, // Panama
    { "PE", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA }, // Peru
    { "PF", cou::Type::FRANCE_OVERSEAS, cou::Location::PACIFIC_POLYNESIA }, // French Polynesia
    { "PG", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO }, // Papua New Guinea
    { "PH", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO }, // Philippines
    { "PK", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Pakistan
    { "PL", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Poland
    { "PM", cou::Type::FRANCE_OVERSEAS, cou::Location::ATLANTIC_NORTH_AMERICA }, // St. Pierre & Miquelon
    { "PN", cou::Type::BRITAIN_OVERSEAS, cou::Location::PACIFIC_POLYNESIA }, // Pitcairn Islands
    { "PR", cou::Type::US_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN }, // Puerto Rico
    { "PS", cou::Type::PARTLY_PALESTINE, cou::Location::ASIA }, // Palestinian Territories
    { "PT", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Portugal
    { "PW", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_MICRONESIA }, // Palau
    { "PY", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA }, // Paraguay
    { "QA", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Qatar
    { "RE", cou::Type::FRANCE_OVERSEAS, cou::Location::INDIAN_AFRICA }, // Réunion
    { "RO", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Romania
    { "RS", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Serbia
    { "RU", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE_ASIA }, // Russia
    { "RW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Rwanda
    { "SA", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Saudi Arabia
    { "SB", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA }, // Solomon Islands
    { "SC", cou::Type::RECOGNIZED_STATE, cou::Location::INDIAN_AFRICA }, // Seychelles
    { "SD", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Sudan
    { "SE", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Sweden
    { "SG", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Singapore
    { "SH", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_OCEAN }, // St. Helena
    { "SI", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE_ASIA }, // Slovenia
    { "SJ", cou::Type::NORWAY_DEPENDENT, cou::Location::ARCTIC_OCEAN }, // Svalbard & Jan Mayen
    { "SK", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Slovakia
    { "SL", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Sierra Leone
    { "SM", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // San Marino
    { "SN", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Senegal
    { "SO", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Somalia
    { "SR", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA }, // Suriname
    { "SS", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // South Sudan
    { "ST", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_AFRICA }, // São Tomé & Príncipe
    { "SV", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA }, // El Salvador
    { "SX", cou::Type::NETHERLANDS_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN }, // Sint Maarten
    { "SY", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Syria
    { "SZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Eswatini
    { "TA", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_OCEAN }, // Tristan da Cunha
    { "TC", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_NORTH_AMERICA }, // Turks & Caicos Islands
    { "TD", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Chad
    { "TF", cou::Type::FRANCE_UNINHABITED, cou::Location::INDIAN_ANTARCTIC }, // French Southern Territories
    { "TG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Togo
    { "TH", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Thailand
    { "TJ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Tajikistan
    { "TK", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_POLYNESIA }, // Tokelau
    { "TL", cou::Type::RECOGNIZED_STATE, cou::Location::MALAY_ARCHIPELAGO }, // Timor-Leste
    { "TM", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Turkmenistan
    { "TN", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Tunisia
    { "TO", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_POLYNESIA }, // Tonga
    { "TR", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Türkiye
    { "TT", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN }, // Trinidad & Tobago
    { "TV", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_POLYNESIA }, // Tuvalu
    { "TW", cou::Type::PARTLY_TAIWAN, cou::Location::PACIFIC_ASIA }, // Taiwan
    { "TZ", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Tanzania
    { "UA", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Ukraine
    { "UG", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Uganda
    { "UM", cou::Type::US_DEPENDENT, cou::Location::PACIFIC_POLYNESIA }, // U.S. Outlying Islands
    { "UN", cou::Type::INTERNATIONAL_ORGANIZATION, cou::Location::NEWYORK_HQ }, // United Nations
    { "US", cou::Type::RECOGNIZED_STATE, cou::Location::NORTH_AMERICA }, // United States
    { "UY", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA }, // Uruguay
    { "UZ", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Uzbekistan
    { "VA", cou::Type::RECOGNIZED_STATE, cou::Location::EUROPE }, // Vatican City
    { "VC", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_CARIBBEAN }, // St. Vincent & Grenadines
    { "VE", cou::Type::RECOGNIZED_STATE, cou::Location::SOUTH_AMERICA }, // Venezuela
    { "VG", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_CARIBBEAN }, // British Virgin Islands
    { "VI", cou::Type::US_DEPENDENT, cou::Location::ATLANTIC_CARIBBEAN }, // U.S. Virgin Islands
    { "VN", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Vietnam
    { "VU", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_AUSTRALIA }, // Vanuatu
    { "WF", cou::Type::FRANCE_OVERSEAS, cou::Location::PACIFIC_POLYNESIA }, // Wallis & Futuna
    { "WS", cou::Type::RECOGNIZED_STATE, cou::Location::PACIFIC_POLYNESIA }, // Samoa
    { "XK", cou::Type::PARTLY_KOSOVO, cou::Location::EUROPE }, // Kosovo
    { "YE", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Yemen
    { "YT", cou::Type::FRANCE_OVERSEAS, cou::Location::INDIAN_AFRICA }, // Mayotte
    { "ZA", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // South Africa
    { "ZM", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Zambia
    { "ZW", cou::Type::RECOGNIZED_STATE, cou::Location::AFRICA }, // Zimbabwe
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
