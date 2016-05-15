/*  This file is part of Axolotl.
 *
 * Axolotl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Axolotl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Axolotl.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AXOLOTL_UTIL_ANSI_H__
#define __AXOLOTL_UTIL_ANSI_H__

namespace util
{
    namespace ansi
    {
        constexpr char clear[] = "\x1B[0m";
        constexpr char bold[] = "\x1B[1m";
        constexpr char no_bold[] = "\x1B[22m";
        constexpr char underline[] = "\x1B[4m";
        constexpr char no_underline[] = "\x1B[24m";

        namespace colors
        {
            // Use :
            // cat colors | tr -s ' ' | cut -d ' ' -f 1,2
            // | sed -ne 's/\(.*\)#\(..\)\(..\)\(..\)/echo constexpr char \1[] = \\"\\\\x1B[38\\;2\\;$[0x\2]\\;$[0x\3]\\;$[0x\4]\\m\\"\\;/p'
            // | bash | xclip -selection c

            constexpr char Black [] = "\x1B[38;2;0;0;0m";
            constexpr char Navy [] = "\x1B[38;2;0;0;128m";
            constexpr char DarkBlue [] = "\x1B[38;2;0;0;139m";
            constexpr char MediumBlue [] = "\x1B[38;2;0;0;205m";
            constexpr char Blue [] = "\x1B[38;2;0;0;255m";
            constexpr char DarkGreen [] = "\x1B[38;2;0;100;0m";
            constexpr char Green [] = "\x1B[38;2;0;128;0m";
            constexpr char Teal [] = "\x1B[38;2;0;128;128m";
            constexpr char DarkCyan [] = "\x1B[38;2;0;139;139m";
            constexpr char DeepSkyBlue [] = "\x1B[38;2;0;191;255m";
            constexpr char DarkTurquoise [] = "\x1B[38;2;0;206;209m";
            constexpr char MediumSpringGreen [] = "\x1B[38;2;0;250;154m";
            constexpr char Lime [] = "\x1B[38;2;0;255;0m";
            constexpr char SpringGreen [] = "\x1B[38;2;0;255;127m";
            constexpr char Aqua [] = "\x1B[38;2;0;255;255m";
            constexpr char Cyan [] = "\x1B[38;2;0;255;255m";
            constexpr char MidnightBlue [] = "\x1B[38;2;25;25;112m";
            constexpr char DodgerBlue [] = "\x1B[38;2;30;144;255m";
            constexpr char LightSeaGreen [] = "\x1B[38;2;32;178;170m";
            constexpr char ForestGreen [] = "\x1B[38;2;34;139;34m";
            constexpr char SeaGreen [] = "\x1B[38;2;46;139;87m";
            constexpr char DarkSlateGray [] = "\x1B[38;2;47;79;79m";
            constexpr char DarkSlateGrey [] = "\x1B[38;2;47;79;79m";
            constexpr char LimeGreen [] = "\x1B[38;2;50;205;50m";
            constexpr char MediumSeaGreen [] = "\x1B[38;2;60;179;113m";
            constexpr char Turquoise [] = "\x1B[38;2;64;224;208m";
            constexpr char RoyalBlue [] = "\x1B[38;2;65;105;225m";
            constexpr char SteelBlue [] = "\x1B[38;2;70;130;180m";
            constexpr char DarkSlateBlue [] = "\x1B[38;2;72;61;139m";
            constexpr char MediumTurquoise [] = "\x1B[38;2;72;209;204m";
            constexpr char Indigo [] = "\x1B[38;2;75;0;130m";
            constexpr char DarkOliveGreen [] = "\x1B[38;2;85;107;47m";
            constexpr char CadetBlue [] = "\x1B[38;2;95;158;160m";
            constexpr char CornflowerBlue [] = "\x1B[38;2;100;149;237m";
            constexpr char RebeccaPurple [] = "\x1B[38;2;102;51;153m";
            constexpr char MediumAquaMarine [] = "\x1B[38;2;102;205;170m";
            constexpr char DimGray [] = "\x1B[38;2;105;105;105m";
            constexpr char DimGrey [] = "\x1B[38;2;105;105;105m";
            constexpr char SlateBlue [] = "\x1B[38;2;106;90;205m";
            constexpr char OliveDrab [] = "\x1B[38;2;107;142;35m";
            constexpr char SlateGray [] = "\x1B[38;2;112;128;144m";
            constexpr char SlateGrey [] = "\x1B[38;2;112;128;144m";
            constexpr char LightSlateGray [] = "\x1B[38;2;119;136;153m";
            constexpr char LightSlateGrey [] = "\x1B[38;2;119;136;153m";
            constexpr char MediumSlateBlue [] = "\x1B[38;2;123;104;238m";
            constexpr char LawnGreen [] = "\x1B[38;2;124;252;0m";
            constexpr char Chartreuse [] = "\x1B[38;2;127;255;0m";
            constexpr char Aquamarine [] = "\x1B[38;2;127;255;212m";
            constexpr char Maroon [] = "\x1B[38;2;128;0;0m";
            constexpr char Purple [] = "\x1B[38;2;128;0;128m";
            constexpr char Olive [] = "\x1B[38;2;128;128;0m";
            constexpr char Gray [] = "\x1B[38;2;128;128;128m";
            constexpr char Grey [] = "\x1B[38;2;128;128;128m";
            constexpr char SkyBlue [] = "\x1B[38;2;135;206;235m";
            constexpr char LightSkyBlue [] = "\x1B[38;2;135;206;250m";
            constexpr char BlueViolet [] = "\x1B[38;2;138;43;226m";
            constexpr char DarkRed [] = "\x1B[38;2;139;0;0m";
            constexpr char DarkMagenta [] = "\x1B[38;2;139;0;139m";
            constexpr char SaddleBrown [] = "\x1B[38;2;139;69;19m";
            constexpr char DarkSeaGreen [] = "\x1B[38;2;143;188;143m";
            constexpr char LightGreen [] = "\x1B[38;2;144;238;144m";
            constexpr char MediumPurple [] = "\x1B[38;2;147;112;219m";
            constexpr char DarkViolet [] = "\x1B[38;2;148;0;211m";
            constexpr char PaleGreen [] = "\x1B[38;2;152;251;152m";
            constexpr char DarkOrchid [] = "\x1B[38;2;153;50;204m";
            constexpr char YellowGreen [] = "\x1B[38;2;154;205;50m";
            constexpr char Sienna [] = "\x1B[38;2;160;82;45m";
            constexpr char Brown [] = "\x1B[38;2;165;42;42m";
            constexpr char DarkGray [] = "\x1B[38;2;169;169;169m";
            constexpr char DarkGrey [] = "\x1B[38;2;169;169;169m";
            constexpr char LightBlue [] = "\x1B[38;2;173;216;230m";
            constexpr char GreenYellow [] = "\x1B[38;2;173;255;47m";
            constexpr char PaleTurquoise [] = "\x1B[38;2;175;238;238m";
            constexpr char LightSteelBlue [] = "\x1B[38;2;176;196;222m";
            constexpr char PowderBlue [] = "\x1B[38;2;176;224;230m";
            constexpr char FireBrick [] = "\x1B[38;2;178;34;34m";
            constexpr char DarkGoldenRod [] = "\x1B[38;2;184;134;11m";
            constexpr char MediumOrchid [] = "\x1B[38;2;186;85;211m";
            constexpr char RosyBrown [] = "\x1B[38;2;188;143;143m";
            constexpr char DarkKhaki [] = "\x1B[38;2;189;183;107m";
            constexpr char Silver [] = "\x1B[38;2;192;192;192m";
            constexpr char MediumVioletRed [] = "\x1B[38;2;199;21;133m";
            constexpr char IndianRed [] = "\x1B[38;2;205;92;92m";
            constexpr char Peru [] = "\x1B[38;2;205;133;63m";
            constexpr char Chocolate [] = "\x1B[38;2;210;105;30m";
            constexpr char Tan [] = "\x1B[38;2;210;180;140m";
            constexpr char LightGray [] = "\x1B[38;2;211;211;211m";
            constexpr char LightGrey [] = "\x1B[38;2;211;211;211m";
            constexpr char Thistle [] = "\x1B[38;2;216;191;216m";
            constexpr char Orchid [] = "\x1B[38;2;218;112;214m";
            constexpr char GoldenRod [] = "\x1B[38;2;218;165;32m";
            constexpr char PaleVioletRed [] = "\x1B[38;2;219;112;147m";
            constexpr char Crimson [] = "\x1B[38;2;220;20;60m";
            constexpr char Gainsboro [] = "\x1B[38;2;220;220;220m";
            constexpr char Plum [] = "\x1B[38;2;221;160;221m";
            constexpr char BurlyWood [] = "\x1B[38;2;222;184;135m";
            constexpr char LightCyan [] = "\x1B[38;2;224;255;255m";
            constexpr char Lavender [] = "\x1B[38;2;230;230;250m";
            constexpr char DarkSalmon [] = "\x1B[38;2;233;150;122m";
            constexpr char Violet [] = "\x1B[38;2;238;130;238m";
            constexpr char PaleGoldenRod [] = "\x1B[38;2;238;232;170m";
            constexpr char LightCoral [] = "\x1B[38;2;240;128;128m";
            constexpr char Khaki [] = "\x1B[38;2;240;230;140m";
            constexpr char AliceBlue [] = "\x1B[38;2;240;248;255m";
            constexpr char HoneyDew [] = "\x1B[38;2;240;255;240m";
            constexpr char Azure [] = "\x1B[38;2;240;255;255m";
            constexpr char SandyBrown [] = "\x1B[38;2;244;164;96m";
            constexpr char Wheat [] = "\x1B[38;2;245;222;179m";
            constexpr char Beige [] = "\x1B[38;2;245;245;220m";
            constexpr char WhiteSmoke [] = "\x1B[38;2;245;245;245m";
            constexpr char MintCream [] = "\x1B[38;2;245;255;250m";
            constexpr char GhostWhite [] = "\x1B[38;2;248;248;255m";
            constexpr char Salmon [] = "\x1B[38;2;250;128;114m";
            constexpr char AntiqueWhite [] = "\x1B[38;2;250;235;215m";
            constexpr char Linen [] = "\x1B[38;2;250;240;230m";
            constexpr char LightGoldenRodYellow [] = "\x1B[38;2;250;250;210m";
            constexpr char OldLace [] = "\x1B[38;2;253;245;230m";
            constexpr char Red [] = "\x1B[38;2;255;0;0m";
            constexpr char Fuchsia [] = "\x1B[38;2;255;0;255m";
            constexpr char Magenta [] = "\x1B[38;2;255;0;255m";
            constexpr char DeepPink [] = "\x1B[38;2;255;20;147m";
            constexpr char OrangeRed [] = "\x1B[38;2;255;69;0m";
            constexpr char Tomato [] = "\x1B[38;2;255;99;71m";
            constexpr char HotPink [] = "\x1B[38;2;255;105;180m";
            constexpr char Coral [] = "\x1B[38;2;255;127;80m";
            constexpr char DarkOrange [] = "\x1B[38;2;255;140;0m";
            constexpr char LightSalmon [] = "\x1B[38;2;255;160;122m";
            constexpr char Orange [] = "\x1B[38;2;255;165;0m";
            constexpr char LightPink [] = "\x1B[38;2;255;182;193m";
            constexpr char Pink [] = "\x1B[38;2;255;192;203m";
            constexpr char Gold [] = "\x1B[38;2;255;215;0m";
            constexpr char PeachPuff [] = "\x1B[38;2;255;218;185m";
            constexpr char NavajoWhite [] = "\x1B[38;2;255;222;173m";
            constexpr char Moccasin [] = "\x1B[38;2;255;228;181m";
            constexpr char Bisque [] = "\x1B[38;2;255;228;196m";
            constexpr char MistyRose [] = "\x1B[38;2;255;228;225m";
            constexpr char BlanchedAlmond [] = "\x1B[38;2;255;235;205m";
            constexpr char PapayaWhip [] = "\x1B[38;2;255;239;213m";
            constexpr char LavenderBlush [] = "\x1B[38;2;255;240;245m";
            constexpr char SeaShell [] = "\x1B[38;2;255;245;238m";
            constexpr char Cornsilk [] = "\x1B[38;2;255;248;220m";
            constexpr char LemonChiffon [] = "\x1B[38;2;255;250;205m";
            constexpr char FloralWhite [] = "\x1B[38;2;255;250;240m";
            constexpr char Snow [] = "\x1B[38;2;255;250;250m";
            constexpr char Yellow [] = "\x1B[38;2;255;255;0m";
            constexpr char LightYellow [] = "\x1B[38;2;255;255;224m";
            constexpr char Ivory [] = "\x1B[38;2;255;255;240m";
            constexpr char White [] = "\x1B[38;2;255;255;255m";
        }
    }
}

#endif // __AXOLOTL_UTIL_ANSI_H__
