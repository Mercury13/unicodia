#include "UcData.h"


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
    { "AR", cou::Type::RECOGNIZED_STATE, cou::Location::ASIA }, // Argentina
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
    { "BM", cou::Type::BRITAIN_OVERSEAS, cou::Location::ATLANTIC_OCEAN }, // Bermuda
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
    { "EH", cou::Type::DISPUTE_MOROCCO_INDEPENDENT, cou::Location::AFRICA }, // Western Sahara
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
    { "GL", cou::Type::DENMARK_AUTONOMY, cou::Location::ATLANTIC_NORTH_AMERICA }, // Greenland
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
    { "IS", cou::Type::RECOGNIZED_STATE, cou::Location::ATLANTIC_OCEAN }, // Iceland
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
};
