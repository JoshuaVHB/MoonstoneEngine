#pragma once

#include <unordered_map>

struct CharInfos
{
    float x, y;
    float width, height;
    float originX, originY;
    float advance;

};

static float fontWidth = 553;
static float fontHeight = 180;


static std::unordered_map<char, CharInfos> fontChars = {
    
    
        {'0' , CharInfos{394,44, 31, 36, 6,  30, 20}},
        {'1' , CharInfos{115,117, 23, 35, 2,  30, 20}},
        {'2' , CharInfos{222,117, 28, 34, 4,  29, 20}},
        {'3' , CharInfos{87 ,117, 28, 35, 4,  29, 20}},
        {'4' , CharInfos{425,44, 31, 36, 6,  30, 20}},
        {'5' , CharInfos{519,81, 30, 35, 5,  29, 20}},
        {'6' , CharInfos{30 ,44, 29, 37, 5,  30, 20}},
        {'7' , CharInfos{393,81, 32, 35, 6,  29, 20}},
        {'8' , CharInfos{0  ,117, 29, 35, 5,  29, 20}},
        {'9' , CharInfos{456,44, 29, 36, 5,  29, 20}},
        {' ' , CharInfos{487,152, 3, 3, 1,  1, 14}},
        {'!' , CharInfos{254,81, 17, 36, 5,  30, 8}},
        {'#' , CharInfos{345,0, 40, 37, 7,  31, 27}},
        {'$' , CharInfos{24 ,0, 32, 43, 5,  32, 22}},
        {'%' , CharInfos{385,0, 36, 37, 4,  31, 26}},
        {'&' , CharInfos{457,0, 33, 37, 6,  30, 21}},
        {'\'' , CharInfos{301,152, 16, 21, 1,  30, 14}},
        {'(' , CharInfos{122,0, 23, 42, 5,  30, 12}},
        {')' , CharInfos{145,0, 23, 42, 5,  30, 12}},
        {'*' , CharInfos{224,152, 28, 23, 6,  30, 17}},
        {'+' , CharInfos{175,152, 26, 24, 3,  22, 20}},
        {',' , CharInfos{343,152, 18, 19, 3,  8, 14}},
        {'-' , CharInfos{410,152, 25, 15, 3,  16, 20}},
        {'.' , CharInfos{435,152, 18, 15, 2,  8, 14}},
        {'/' , CharInfos{291,0, 27, 38, 5,  31, 16}},
        {':' , CharInfos{133,152, 17, 26, 1,  23, 14}},
        {';' , CharInfos{277,117, 19, 31, 3,  23, 14}},
        {'<' , CharInfos{201,152, 23, 24, 2,  22, 20}},
        {'=' , CharInfos{252,152, 25, 22, 3,  21, 20}},
        {'>' , CharInfos{150,152, 25, 25, 2,  23, 20}},
        {'?' , CharInfos{116,81, 28, 36, 5,  29, 18}},
        {'@' , CharInfos{252,0, 39, 39, 5,  31, 30}},
        {'A' , CharInfos{156,117, 33, 34, 5,  28, 23}},
        {'B' , CharInfos{485,44, 29, 36, 4,  30, 20}},
        {'C' , CharInfos{457,81, 31, 35, 5,  29, 20}},
        {'D' , CharInfos{490,0, 32, 37, 4,  30, 23}},
        {'E' , CharInfos{522,0, 31, 37, 5,  30, 20}},
        {'F' , CharInfos{0  ,44, 30, 37, 5,  30, 19}},
        {'G' , CharInfos{264,44, 33, 36, 5,  30, 22}},
        {'H' , CharInfos{196,44, 34, 36, 4,  30, 25}},
        {'I' , CharInfos{29 ,117, 29, 35, 6,  29, 17}},
        {'J' , CharInfos{330,44, 32, 36, 5,  29, 21}},
        {'K' , CharInfos{514,44, 29, 36, 4,  29, 20}},
        {'L' , CharInfos{0  ,81, 29, 36, 5,  29, 18}},
        {'M' , CharInfos{157,44, 39, 36, 5,  29, 28}},
        {'N' , CharInfos{421,0, 36, 37, 5,  30, 26}},
        {'O' , CharInfos{288,81, 36, 35, 5,  29, 26}},
        {'P' , CharInfos{200,81, 27, 36, 5,  30, 17}},
        {'Q' , CharInfos{168,0, 40, 41, 6,  29, 28}},
        {'R' , CharInfos{488,81, 31, 35, 5,  29, 20}},
        {'S' , CharInfos{189,117, 33, 34, 5,  28, 22}},
        {'T' , CharInfos{324,81, 35, 35, 6,  29, 22}},
        {'U' , CharInfos{297,44, 33, 36, 5,  29, 24}},
        {'V' , CharInfos{362,44, 32, 36, 5,  29, 22}},
        {'W' , CharInfos{113,44, 44, 36, 5,  29, 33}},
        {'X' , CharInfos{230,44, 34, 36, 6,  29, 23}},
        {'Y' , CharInfos{425,81, 32, 35, 6,  29, 20}},
        {'Z' , CharInfos{359,81, 34, 35, 6,  29, 22}},
        {'[' , CharInfos{208,0, 22, 41, 4,  29, 12}},
        {'\\', CharInfos{59 ,44, 27, 37, 4,  29, 18}},
        {']' , CharInfos{230,0, 22, 41, 5,  29, 12}},
        {'^' , CharInfos{317,152, 26, 19, 3,  31, 20}},
        {'_' , CharInfos{453,152, 34, 14, 7,  3, 20}},
        {'`' , CharInfos{390,152, 20, 18, 4,  31, 18}},
        {'a' , CharInfos{450,117, 28, 28, 5,  22, 18}},
        {'b' , CharInfos{29 ,81, 29, 36, 5,  30, 19}},
        {'c' , CharInfos{0  ,152, 27, 28, 6,  22, 16}},
        {'d' , CharInfos{58 ,81, 29, 36, 5,  30, 19}},
        {'e' , CharInfos{478,117, 28, 28, 5,  22, 18}},
        {'f' , CharInfos{318,0, 27, 38, 6,  30, 16}},
        {'g' , CharInfos{58 ,117, 29, 35, 6,  21, 17}},
        {'h' , CharInfos{144,81, 28, 36, 4,  30, 18}},
        {'i' , CharInfos{138,117, 18, 35, 4,  29, 9}},
        {'j' , CharInfos{0  ,0, 24, 44, 7,  29, 13}},
        {'k' , CharInfos{172,81, 28, 36, 5,  30, 17}},
        {'l' , CharInfos{271,81, 17, 36, 4,  30, 9}},
        {'m' , CharInfos{296,117, 35, 30, 5,  23, 25}},
        {'n' , CharInfos{27 ,152, 27, 28, 5,  22, 17}},
        {'o' , CharInfos{54 ,152, 27, 28, 5,  22, 17}},
        {'p' , CharInfos{86 ,44, 27, 37, 5,  23, 17}},
        {'q' , CharInfos{227,81, 27, 36, 6,  22, 17}},
        {'r' , CharInfos{81 ,152, 26, 28, 5,  22, 15}},
        {'s' , CharInfos{107,152, 26, 28, 5,  22, 16}},
        {'t' , CharInfos{250,117, 27, 33, 6,  27, 15}},
        {'u' , CharInfos{396,117, 27, 29, 5,  22, 17}},
        {'v' , CharInfos{506,117, 28, 28, 6,  22, 16}},
        {'w' , CharInfos{331,117, 34, 29, 6,  22, 22}},
        {'x' , CharInfos{365,117, 31, 29, 6,  23, 19}},
        {'y' , CharInfos{87 ,81, 29, 36, 6,  22, 18}},
        {'z' , CharInfos{423,117, 27, 29, 5,  22, 17}},
        {'{' , CharInfos{72 ,0, 25, 42, 7,  31, 12}},
        {'|' , CharInfos{56 ,0, 16, 43, 1,  32, 13}},
        {'}' , CharInfos{97 ,0, 25, 42, 7,  31, 12}},
        {'~' , CharInfos{361,152, 29, 18, 5,  20, 20}}
        
};