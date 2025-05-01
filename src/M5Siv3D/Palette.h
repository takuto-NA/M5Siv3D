#pragma once

#include "Color.h"

// 色定数の追加
namespace Palette
{
    // Basic colors
    const Color Black(0, 0, 0);
    const Color White(255, 255, 255);
    const Color Red(255, 0, 0);
    const Color Green(0, 128, 0);
    const Color Blue(0, 0, 255);
    const Color Yellow(255, 255, 0);
    const Color Magenta(255, 0, 255);
    const Color Cyan(0, 255, 255);

    // Gray shades
    const Color Dimgray(105, 105, 105);
    const Color Gray(128, 128, 128);
    const Color Darkgray(169, 169, 169);
    const Color Silver(192, 192, 192);
    const Color Lightgray(211, 211, 211);
    const Color Gainsboro(220, 220, 220);
    const Color Whitesmoke(245, 245, 245);

    // Warm colors
    const Color Orange(255, 165, 0);
    const Color Darkorange(255, 140, 0);
    const Color Coral(255, 127, 80);
    const Color Tomato(255, 99, 71);
    const Color Orangered(255, 69, 0);
    const Color Crimson(220, 20, 60);
    const Color Firebrick(178, 34, 34);
    const Color Darkred(139, 0, 0);
    const Color Maroon(128, 0, 0);

    // Cool colors
    const Color Navy(0, 0, 128);
    const Color Darkblue(0, 0, 139);
    const Color Mediumblue(0, 0, 205);
    const Color Royalblue(65, 105, 225);
    const Color Steelblue(70, 130, 180);
    const Color Deepskyblue(0, 191, 255);
    const Color Dodgerblue(30, 144, 255);
    const Color Cornflowerblue(100, 149, 237);

    // Green shades
    const Color Darkgreen(0, 100, 0);
    const Color Forestgreen(34, 139, 34);
    const Color Seagreen(46, 139, 87);
    const Color Limegreen(50, 205, 50);
    const Color Springgreen(0, 255, 127);
    const Color Lime(0, 255, 0);

    // Purple shades
    const Color Indigo(75, 0, 130);
    const Color Purple(128, 0, 128);
    const Color Darkmagenta(139, 0, 139);
    const Color Darkviolet(148, 0, 211);
    const Color Darkorchid(153, 50, 204);
    const Color Blueviolet(138, 43, 226);

    // Special colors
    const Color DefaultLetterbox(1, 2, 3);
    const Color DefaultBackground(11, 22, 33);
} 