/*
 * languages.h
 *
 *  Created on: Dec 12, 2024
 *      Author: cezary.macik
 */

#ifndef INC_LANGUAGES_H_
#define INC_LANGUAGES_H_

#define _MINUS "\x5B"
#define _PRIM  "\x5C"
#define _QUE   "\x5D"
#define _SPACE "\x64"

/*
	Ą	\x5B
	Ć	\x5C
	Ę	\x5D
	Ó	\x5E
	Ń	\x5F
	Ł	\x60
	Ś	\x61
	Ż	\x62
	Ź	\x63
*/

enum{
	SYMBOL_PARTER,
	SYMBOL_SUTERENA,
	SYMBOL_BALKON,
	SYMBOL_SZATNIA,
	SYMBOL_ANTRESOLA,
	SYMBOL_PARTEROW,
	SYMBOL_PARKING,
	SYMBOL_GARAZ
} symbolsFloor;

const char *floorSymbols[][3]=
{
	{"O","P","O"},
	{"B","S","B"},
	{"G","B","G"},
	{"CL","SZ","CL"},
	{"M","A","M"},
	{"G","P","G"},
	{"P","P","P"},
	{"G","G","G"}
};

const char *floorSpecDescriptions[][3]=
{
/*0*/	{"\x64","POZIOM\x64""TECHNICZNY", 						"TECHNICAL\x64""LEVEL"},
/*1*/	{"\x64","POZIOM\x64""SPECJALNY", 						"SPECIAL\x64""LEVEL"},
/*2*/	{"\x64","PARTER\x64""WYSOKI", 							"UPPER\x64""GROUND\x64""FLOOR"},
/*3*/	{"\x64","WINDA\x64""NIECZYNNA", 						"BASEMENT"},
/*4*/	{"\x64","JAZDA\x64""ORIENTUJ""\x5B""CA", 				"INITIAL\x64""TRAVEL"},
/*5*/	{"\x64","PARTER\x64""NISKI", 							"LOWER\x64""GROUND\x64""FLOOR"},
/*6*/	{"\x64","PRZECI\x5B\x62""ENIE\x64""PROSZ\x5D" "\x64OPU\x61""CI\x5C" "\x64KABIN\x5D", "OVERLOAD\x64""PLEASE\x64""LEAVE\x64""A\x64""CAR"},
/*7*/	{"\x64","JAZDA\x64""SPECJALNA",							"PRIORITY\x64""TRAVEL"},
/*8*/	{"\x64","NAPRAWA\x64""WINDY\x64""PRZEPRASZAMY",			"MAINTENANCE"},
/*9*/	{"\x64","ZJAZD\x64""AWARYJNY",							"EMERGENCY\x64""TRAVEL"},
/*10*/	{"\x64","REMONT\x64""WINDY",							"OUT\x64""OF\x64""ORDER"},
/*11*/	{"\x64","JAZDA\x64""PO\x62""AROWA",					    "FIRE\x64""TRAVEL\x64\x64\x64\x64\x64"},
/*12*/	//{"\x64","KIERUNEK\x64""DALSZEJ\x64""JAZDY\x64""W\x64""G\x5E""R\x5D", "NEXT\x64""DIRECTION\x64""UP"},
/*13*/	//{"\x64","KIERUNEK\x64""DALSZEJ\x64""JAZDY\x64""W\x64""D\x5E""\x60",  "NEXT\x64""DIRECTION\x64""DOWN"},
/*14*/	{"\x64","\x64",""}
};

const char *floorDescriptionText[][3]=
{
	{"\x64","POZIOM", 				"LEVEL"},
	{"\x64","PI\x5D""TRO", 		"FLOOR"},
	{"\x64","PARTER", 				"GROUND\x64""FLOOR"},
	{"\x64","SUTERENA", 			"BASEMENT"},
	{"\x64","BALKON", 				"GALLERY"},
	{"\x64","PARKING", 			"PARKING"},
	{"\x64","GARA\x62", 			"GARAGE"},
	{"\x64","SZATNIA", 			"CLOAKROOM"},
	{"\x64","AMFITEATR", 			"AMPHITHEATRE"},
	{"\x64","HOL", 				"HALL"},
	{"\x64","ANTRESOLA", 			"MEZZANINE"},
	{"\x64","RECEPCJA", 			"RECEPTION"},
	{"\x64","WYSOKI", 				"UPPER"},
	{"\x64","NISKI", 				"LOWER"},
	{"\x64","TECHNICZNY", 			"TECHNICAL"},
	{"\x64","SPECJALNY", 			"SPECIAL"},
	{"\x64","PRZECI\x5B\x62""ENIE", "OVERLOAD"},
	{"\x64","WINDA", 				"\x64",},
	{"\x64","NIECZYNNA", 			"OUT\x64""OF\x64""\x64ORDER"},
	{"\x64","NAPRAWA", 			"MAINTENANCE\x64"},
	{"\x64","WINDY", 				"\x64"},
	{"\x64","ZJAZD", 				"\x64"},
	{"\x64","AWARYJNY", 			"EMERGENCY\x64""TRAVEL"},
	{"\x64","JAZDA", 				"\x64"},
	{"\x64","PO\x62""AROWA", 		"FIRE\x64""TRAVEL"},
	{"\x64","ORIENTUJ""\x5B""CA", 	"INITIAL\x64""\x64TRAVEL"},
	{"\x64","SPECJALNA", 			"PRIORITY\x64""TRAVEL"},
	{"\x64","D\x63""WIG", 			"\x64"},
	{"\x64","REMONT", 				"\x64"},
	{"\x64","BRAK\x64""LIFTCAN",	"LIFTCAN\x64""N\x64""D"}
};

const char *errorDescriptionText[][3]=
{
/*E00*/ 	{"\x64","USTERKA\x64""D\x63""WIGU",										"LIFT\x64""ERROR\x64\x64\x64\x64\x64\x64\x64"},
/*E01*/ 	{"\x64","PRZERWANY\x64" "OBW\x5E""D""\x64" "BEZPIECZE\x5F""STWA",			"SAFETY\x64""CIRCUIT\x64""FAULT"},
/*E02*/ 	{"\x64","PRZERWANY\x64" "OBW\x5E""D""\x64""DRZWI",							"DOOR\x64""CIRCUIT\x64""FAULT"},
/*E03*/		{"\x64","PRZEKROCZONY\x64" "CZAS\x64" "ZAMYKANIA\x64" "DRZWI",				"DOOR\x64""CLOSING\x64""FAULT"},
/*E04*/		{"\x64","ZATRZYMANIE\x64" "POZA\x64" "STREF\x5B\x64" "PRZYSTANKU",			"CAR\x64""\x64OUT\x64""OF\x64""FLOOR"},
/*E05*/		{"\x64","BLOKADA\x64" "PRZYCISKU\x64" "WEZWA\x5F",							"BUTTON\x64""BLOCKED"},
/*E06*/		{"\x64","PRZEKROCZONY\x64" "CZAS\x64" "RUSZANIA\x64" "Z\x64" "PRZYSTANKU",	"STARTING\x64""TIME\x64""FAULT"},
/*E07*/		{"\x64","RESTART\x64" "STEROWANIA",										"CONTROLLER\x64""RESET"},
/*E08*/		{"\x64","B\x60" "\x5B""D\x64" "WEWN\x5D" "TRZNY\x64" "STEROWANIA",			"SOFTWARE\x64""ERROR"},
/*E09*/		{"\x64","BLOKADA\x64" "ZAMYKANIA\x64" "DRZWI",								"DOOR\x64""BLOCKED\x64\x64\x64"},
/*E10*/		{"\x64","PRZEKROCZONY\x64" "CZAS\x64" "JAZDY",								"TRAVEL\x64""TIME\x64""FAULT"},
/*E11*/ 	{"\x64","NIEPRAWID\x60""OWE\x64" "SYGNA\x60""Y\x64" "ODWZOROWANIA",		"FLOOR\x64""SELECTOR\x64""FAULT"},
/*E12*/ 	{"\x64","PRZEGRZANIE\x64" "NAP\x5D""DU\x64" "D\x63""WIGU",					"MOTOR\x64""OVERHEATED"},
/*E13*/ 	{"\x64","PRZEKROCZONY\x64" "CZAS\x64" "OTWIERANIA\x64" "DRZWI",			"DOOR\x64""OPENING\x64""FAULT"},
/*E14*/		{"\x64","AKTYWNY\x64" "SYGNA\x60\x64" "NAWROTU\x64" "DRZWI",				"DOOR\x64""REVERSE\x64""ERROR"},
/*E15*/		{"\x64","AWARIA\x64" "SYSTEMU\x64" "UCM",									"UCM\x64""DEVICE\x64""ERROR"},
/*E16*/		{"\x64","AWARIA\x64" "STYCZNIK\x5E""W\x64" "NAP\x5D""DU",					"MAIN\x64""CONTACTORS\x64""FAULT"},
/*E17*/		{"\x64","AWARIA\x64" "STYCZNIK\x5E""W\x64" "NAP\x5D""DU",					"MAIN\x64""CONTACTORS\x64""FAULT"},
/*E18*/		{"\x64","BRAK\x64" "KOMUNIKACJI\x64" "Z\x64" "ODWZOROWANIEM",				"FLOOR\x64""SELECTOR\x64""FAULT"},
/*E19*/		{"\x64","AWARIA\x64" "ODWZOROWANIA\x64" "ENKODEROWEGO",					"FLOOR\x64""SELECTOR\x64""FAULT"},
/*E20*/		{"\x64","ZATRZYMANIE\x64" "KABINY\x64" "POZA\x64" "PRZYSTANKIEM",			"CAR\x64""\x64OUT\x64""OF\x64""FLOOR"},
/*E21*/		{"\x64","ZATRZYMANIE\x64" "KABINY\x64" "POZA\x64" "PRZYSTANKIEM",			"CAR\x64""\x64OUT\x64""OF\x64""FLOOR"},
/*E22*/		{"\x64","AWARIA\x64" "SYSTEMU\x64" "KONTROLI\x64" "DRZWI",					"DOOR\x64""CIRCUIT\x64""CONTROL\x64""FAUL"},
/*E23*/		{"\x64","USTERKA\x64" "OBWODU\x64" "BEZPIECZE\x5F""STWA\x64" "DRZWI",		"DOOR\x64""CIRCUIT\x64""FAULT"},
/*E24*/		{"\x64","USTERKA\x64""D\x63""WIGU",										"LIFT\x64""ERROR\x64\x64\x64\x64\x64\x64\x64"},
/*E25*/		{"\x64","ZABLOKOWANE\x64" "DRZWI\x64" "SZYBOWE",							"SHAFT\x64""DOOR\x64""BLOCKED"},
/*E26*/		{"\x64","AWARIA\x64" "STYCZNIK\x5E""W\x64" "NAP\x5D""DU",					"MAIN\x64""CONTACTORS\x64""FAULT"},
/*E27*/		{"\x64","PRZECI\x5B\x62""ENIE\x64""KABINY",											"OVERLOAD\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64"},
/*E28*/		{"\x64","PE\x60""NE\x64" "OBCI\x5B\x62""ENIE\x64" "WEZWANIA\x64" "WY\x60""ACZONE","FULL\x64""LOAD\x64""CAR\x64""CALLS\x64""ONLY"},
/*E29*/		{"\x64","ZA\x60\x5B""CZONO\x64" "STOP\x64" "W\x64" "KABINIE",				"CAR\x64" "STOP\x64""PRESSED"},
/*E30*/		{"\x64","AWARIA\x64" "KRZYWKI\x64" "RYGLOWANIA",							"LOCKING\x64""CAM\x64""FAULT"},
/*E31*/		{"\x64","NIEPRAWID\x60""OWE\x64" "SYGNA\x60""Y\x64" "ODWZOROWANIA",		"PULSE\x64""COUNTER\x64""FAULT"},
/*E32*/		{"\x64","NIEPRAWID\x60""OWE\x64" "SYGNA\x60""Y\x64" "ODWZOROWANIA",		"FINAL\x64""SWITCH\x64""FAULT"},
/*E33*/		{"\x64","PRZEKROCZONY\x64" "CZAS\x64" "KOREKCJI\x64" "NA\x64" "PRZYSTANKU","RELEVELING\x64""FAULT"},
/*E34*/		{"\x64","AKTYWNY\x64" "\x60\x5B""CZNIK\x64" "KRA\x5F""COWY",				"TOP\x64""LIMIT\x64""SWITCH\x64""FAULT"},
/*E35*/		{"\x64","PRZEKROCZONA\x64" "PREDKO\x61\x5C\x64" "DOJAZDOWA",				"SPEED\x64""CONTROL\x64""FAULT"},
/*E36*/		{"\x64","AWARIA\x64" "MODU\x60""U\x64" "BEZPIECZE\x5F""STWA",				"SAFETY\x64""UNIT\x64""FAULT"},
/*E37*/		{"\x64","AWARIA\x64" "\x60\x5B""CZNIK\x5E""W\x64" "DRZWI\x64" "KABINOWYCH","DOOR\x64""LIMIT\x64""SWITCHES\x64""FAULT"},
/*E38*/		{"\x64","BRAK\x64" "KOMUNIKACJI\x64" "Z\x64" "ODWZOROWANIEM",				"FLOOR\x64""SELECTOR\x64""FAULT"},
/*E39*/		{"\x64","AWARIA\x64" "PODCHWYT\x5E""W",									"PAWL\x64""DEVICE\x64""FAULT"},
/*E40*/		{"\x64","USTERKA\x64" "MAGISTRALI\x64",									"LIFTCAN\x64""ERROR\x64\x64\x64\x64\x64\x64\x64"},
/*E41*/		{"\x64","WINDA\x64" "WY\x60\x5B""CZONA",									"LIFT\x64""OUT\x64""OF\x64""ORDER"},
/*E42*/		{"\x64","WINDA\x64""W\x64""NAPRAWIE",															"LIFT\x64""IN\x64""MAINTENANCE"},
/*E43*/		{"\x64","JAZDA\x64""PO\x62""AROWA",															"FIRE\x64""TRAVEL\x64\x64\x64\x64\x64\x64"},
/*E44*/		{"\x64","WEZWANIA\x64" "WY\x60\x5B""CZONE",								"FLOOR\x64""CALLS\x64""OFF"},
/*E45*/		{"\x64","JAZDA\x64""SPECJALNA",															"PRIORITY\x64""TRAVEL"},
/*E46*/		{"\x64","ZJAZD\x64""AWARYJNY",															"EMERGENCY\x64""TRAVEL"},
/*E47*/		{"\x64","USTERKA\x64""D\x63""WIGU",										"LIFT\x64""ERROR\x64\x64\x64\x64\x64\x64\x64"},
/*E48*/		{"\x64","USTERKA\x64""D\x63""WIGU",										"LIFT\x64""ERROR\x64\x64\x64\x64\x64\x64\x64"},
/*E49*/		{"\x64","USTERKA\x64""D\x63""WIGU",										"LIFT\x64""ERROR\x64\x64\x64\x64\x64\x64\x64"}
};

#endif /* INC_LANGUAGES_H_ */
