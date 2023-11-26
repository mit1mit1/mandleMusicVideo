#include <vector>
#include <cmath>
#include "colors.h"

std::vector<PixelColor> getColors()
{

    // Rolling in the deep colors

    // e8d6cb rgb(232, 214, 203)
    PixelColor col_e8d6cb;
    col_e8d6cb.red = static_cast<unsigned char>(232);
    col_e8d6cb.green = static_cast<unsigned char>(208);
    col_e8d6cb.blue = static_cast<unsigned char>(197);
    col_e8d6cb.alpha = static_cast<unsigned char>(255);
    // // e8d6cb rgb(232, 214, 203)
    // PixelColor col_e8d6cb2;
    // col_e8d6cb2.red = static_cast<unsigned char>(225);
    // col_e8d6cb2.green = static_cast<unsigned char>(200);
    // col_e8d6cb2.blue = static_cast<unsigned char>(184);
    // col_e8d6cb2.alpha = static_cast<unsigned char>(255);
    // // e8d6cb rgb(232, 214, 203)
    // PixelColor col_e8d6cb3;
    // col_e8d6cb3.red = static_cast<unsigned char>(220);
    // col_e8d6cb3.green = static_cast<unsigned char>(192);
    // col_e8d6cb3.blue = static_cast<unsigned char>(176);
    // col_e8d6cb3.alpha = static_cast<unsigned char>(255);
    // // e8d6cb rgb(232, 214, 203)
    // PixelColor col_e8d6cb4;
    // col_e8d6cb4.red = static_cast<unsigned char>(213);
    // col_e8d6cb4.green = static_cast<unsigned char>(182);
    // col_e8d6cb4.blue = static_cast<unsigned char>(170);
    // col_e8d6cb4.alpha = static_cast<unsigned char>(255);
    // d0ada7 rgb(208, 173, 167)
    PixelColor col_d0ada7;
    col_d0ada7.red = static_cast<unsigned char>(208);
    col_d0ada7.green = static_cast<unsigned char>(173);
    col_d0ada7.blue = static_cast<unsigned char>(167);
    col_d0ada7.alpha = static_cast<unsigned char>(255);
    // // d0ada7 rgb(208, 173, 167)
    // PixelColor col_d0ada72;
    // col_d0ada72.red = static_cast<unsigned char>(200);
    // col_d0ada72.green = static_cast<unsigned char>(160);
    // col_d0ada72.blue = static_cast<unsigned char>(155);
    // col_d0ada72.alpha = static_cast<unsigned char>(255); // d0ada7 rgb(208, 173, 167)
    // PixelColor col_d0ada73;
    // col_d0ada73.red = static_cast<unsigned char>(188);
    // col_d0ada73.green = static_cast<unsigned char>(145);
    // col_d0ada73.blue = static_cast<unsigned char>(138);
    // col_d0ada73.alpha = static_cast<unsigned char>(255); // d0ada7 rgb(208, 173, 167)
    // PixelColor col_d0ada74;
    // col_d0ada74.red = static_cast<unsigned char>(179);
    // col_d0ada74.green = static_cast<unsigned char>(120);
    // col_d0ada74.blue = static_cast<unsigned char>(120);
    // col_d0ada74.alpha = static_cast<unsigned char>(255);
    // ad6a6c rgb(173, 106, 108)
    PixelColor col_ad6a6c;
    col_ad6a6c.red = static_cast<unsigned char>(173);
    col_ad6a6c.green = static_cast<unsigned char>(106);
    col_ad6a6c.blue = static_cast<unsigned char>(108);
    col_ad6a6c.alpha = static_cast<unsigned char>(255);
    // ad6a6c rgb(173, 106, 108)
    // PixelColor col_ad6a6c2;
    // col_ad6a6c2.red = static_cast<unsigned char>(155);
    // col_ad6a6c2.green = static_cast<unsigned char>(90);
    // col_ad6a6c2.blue = static_cast<unsigned char>(95);
    // col_ad6a6c2.alpha = static_cast<unsigned char>(255);
    // // ad6a6c rgb(173, 106, 108)
    // PixelColor col_ad6a6c3;
    // col_ad6a6c3.red = static_cast<unsigned char>(135);
    // col_ad6a6c3.green = static_cast<unsigned char>(70);
    // col_ad6a6c3.blue = static_cast<unsigned char>(80);
    // col_ad6a6c3.alpha = static_cast<unsigned char>(255);
    // // ad6a6c rgb(173, 106, 108)
    // PixelColor col_ad6a6c4;
    // col_ad6a6c4.red = static_cast<unsigned char>(115);
    // col_ad6a6c4.green = static_cast<unsigned char>(60);
    // col_ad6a6c4.blue = static_cast<unsigned char>(75);
    // col_ad6a6c4.alpha = static_cast<unsigned char>(255);
    // 5d2e46 rgb(93, 46, 70)
    PixelColor col_5d2e46;
    col_5d2e46.red = static_cast<unsigned char>(93);
    col_5d2e46.green = static_cast<unsigned char>(46);
    col_5d2e46.blue = static_cast<unsigned char>(70);
    col_5d2e46.alpha = static_cast<unsigned char>(255);
    // b58db6 rgb(181, 141, 182)
    PixelColor col_b58db6;
    col_b58db6.red = static_cast<unsigned char>(181);
    col_b58db6.green = static_cast<unsigned char>(141);
    col_b58db6.blue = static_cast<unsigned char>(182);
    col_b58db6.alpha = static_cast<unsigned char>(255);

    // c5afa0 rgb(197, 175, 160)
    PixelColor col_c5afa0;
    col_c5afa0.red = static_cast<unsigned char>(197);
    col_c5afa0.green = static_cast<unsigned char>(175);
    col_c5afa0.blue = static_cast<unsigned char>(160);
    col_c5afa0.alpha = static_cast<unsigned char>(255);
    // e9bcb7 rgb(233, 188, 183)
    PixelColor col_e9bcb7;
    col_e9bcb7.red = static_cast<unsigned char>(233);
    col_e9bcb7.green = static_cast<unsigned char>(188);
    col_e9bcb7.blue = static_cast<unsigned char>(183);
    col_e9bcb7.alpha = static_cast<unsigned char>(255);
    // ab756b rgb(171, 117, 107)
    PixelColor col_ab756b;
    col_ab756b.red = static_cast<unsigned char>(171);
    col_ab756b.green = static_cast<unsigned char>(117);
    col_ab756b.blue = static_cast<unsigned char>(107);
    col_ab756b.alpha = static_cast<unsigned char>(255);

    // 0081af rgb(0, 129, 175)
    PixelColor col_0081af;
    col_0081af.red = static_cast<unsigned char>(0);
    col_0081af.green = static_cast<unsigned char>(129);
    col_0081af.blue = static_cast<unsigned char>(175);
    col_0081af.alpha = static_cast<unsigned char>(255);
    // 00abe7 rgb(0, 171, 231)
    PixelColor col_00abe7;
    col_00abe7.red = static_cast<unsigned char>(0);
    col_00abe7.green = static_cast<unsigned char>(171);
    col_00abe7.blue = static_cast<unsigned char>(231);
    col_00abe7.alpha = static_cast<unsigned char>(255);
    // 2dc7ff rgb(45, 199, 255)
    PixelColor col_2dc7ff;
    col_2dc7ff.red = static_cast<unsigned char>(45);
    col_2dc7ff.green = static_cast<unsigned char>(199);
    col_2dc7ff.blue = static_cast<unsigned char>(255);
    col_2dc7ff.alpha = static_cast<unsigned char>(255);

    // 06070e rgb(6, 7, 14)
    PixelColor col_06070e;
    col_06070e.red = static_cast<unsigned char>(6);
    col_06070e.green = static_cast<unsigned char>(7);
    col_06070e.blue = static_cast<unsigned char>(14);
    col_06070e.alpha = static_cast<unsigned char>(255);
    // 29524a rgb(41, 82, 74)
    PixelColor col_29524a;
    col_29524a.red = static_cast<unsigned char>(41);
    col_29524a.green = static_cast<unsigned char>(82);
    col_29524a.blue = static_cast<unsigned char>(74);
    col_29524a.alpha = static_cast<unsigned char>(255);
    // 94a187 rgb(148, 161, 135)
    PixelColor col_94a187;
    col_94a187.red = static_cast<unsigned char>(148);
    col_94a187.green = static_cast<unsigned char>(161);
    col_94a187.blue = static_cast<unsigned char>(135);
    col_94a187.alpha = static_cast<unsigned char>(255);
    // 93acb5 rgb(147, 172, 181)
    PixelColor col_93acb5;
    col_93acb5.red = static_cast<unsigned char>(147);
    col_93acb5.green = static_cast<unsigned char>(172);
    col_93acb5.blue = static_cast<unsigned char>(181);
    col_93acb5.alpha = static_cast<unsigned char>(255);
    // 96c5f7 rgb(150, 197, 247)
    PixelColor col_96c5f7;
    col_96c5f7.red = static_cast<unsigned char>(150);
    col_96c5f7.green = static_cast<unsigned char>(197);
    col_96c5f7.blue = static_cast<unsigned char>(247);
    col_96c5f7.alpha = static_cast<unsigned char>(255);
    // a9d3ff rgb(169, 211, 255)
    PixelColor col_a9d3ff;
    col_a9d3ff.red = static_cast<unsigned char>(169);
    col_a9d3ff.green = static_cast<unsigned char>(211);
    col_a9d3ff.blue = static_cast<unsigned char>(255);
    col_a9d3ff.alpha = static_cast<unsigned char>(255);

    // f2f4ff rgb(242, 244, 255)
    PixelColor col_f2f4ff;
    col_f2f4ff.red = static_cast<unsigned char>(242);
    col_f2f4ff.green = static_cast<unsigned char>(244);
    col_f2f4ff.blue = static_cast<unsigned char>(255);
    col_f2f4ff.alpha = static_cast<unsigned char>(255);

    // ead2ac rgb(234, 210, 172)
    PixelColor col_ead2ac;
    col_ead2ac.red = static_cast<unsigned char>(234);
    col_ead2ac.green = static_cast<unsigned char>(210);
    col_ead2ac.blue = static_cast<unsigned char>(172);
    col_ead2ac.alpha = static_cast<unsigned char>(255);
    // eaba6b rgb(234, 186, 107)
    PixelColor col_eaba6b;
    col_eaba6b.red = static_cast<unsigned char>(234);
    col_eaba6b.green = static_cast<unsigned char>(186);
    col_eaba6b.blue = static_cast<unsigned char>(107);
    col_eaba6b.alpha = static_cast<unsigned char>(255);

    // 8ab0ab rgb(138, 176, 171)
    PixelColor col_8ab0ab;
    col_8ab0ab.red = static_cast<unsigned char>(138);
    col_8ab0ab.green = static_cast<unsigned char>(176);
    col_8ab0ab.blue = static_cast<unsigned char>(171);
    col_8ab0ab.alpha = static_cast<unsigned char>(255);
    // 3e505b rgb(62, 80, 91)
    PixelColor col_3e505b;
    col_3e505b.red = static_cast<unsigned char>(62);
    col_3e505b.green = static_cast<unsigned char>(80);
    col_3e505b.blue = static_cast<unsigned char>(91);
    col_3e505b.alpha = static_cast<unsigned char>(255);
    // 26413c rgb(38, 65, 60)
    PixelColor col_26413c;
    col_26413c.red = static_cast<unsigned char>(38);
    col_26413c.green = static_cast<unsigned char>(65);
    col_26413c.blue = static_cast<unsigned char>(60);
    col_26413c.alpha = static_cast<unsigned char>(255);
    // 1a1d1a rgb(26, 29, 26)
    PixelColor col_1a1d1a;
    col_1a1d1a.red = static_cast<unsigned char>(26);
    col_1a1d1a.green = static_cast<unsigned char>(29);
    col_1a1d1a.blue = static_cast<unsigned char>(26);
    col_1a1d1a.alpha = static_cast<unsigned char>(255);
    // 03120e rgb(3, 18, 14)
    PixelColor col_03120e;
    col_03120e.red = static_cast<unsigned char>(3);
    col_03120e.green = static_cast<unsigned char>(18);
    col_03120e.blue = static_cast<unsigned char>(14);
    col_03120e.alpha = static_cast<unsigned char>(255);

    std::vector<PixelColor> rollingInTheDeepColors = {
        col_e8d6cb,
        // col_e8d6cb2,
        // col_e8d6cb3,
        // col_e8d6cb4,
        col_d0ada7,
        // col_d0ada72,
        // col_d0ada73,
        // col_d0ada74,
        col_ad6a6c,
        // col_ad6a6c2,
        // col_ad6a6c3,
        // col_ad6a6c4,
        col_5d2e46,
        col_b58db6,
        col_c5afa0,
        col_e9bcb7,
        col_ab756b,
        col_0081af,
        col_00abe7,
        col_2dc7ff,
        col_06070e,
        col_29524a,
        col_94a187,
        col_93acb5,
        col_96c5f7,
        col_a9d3ff,
        col_f2f4ff,
        col_ead2ac,
        col_eaba6b,
        col_8ab0ab,
        col_3e505b,
        col_26413c,
        col_1a1d1a,
        col_03120e,
    };

    std::vector<PixelColor> colors = {};
    const int colorsBetween = 5;

    for (unsigned int i = 0; i < rollingInTheDeepColors.size(); i++)
    {
        colors.push_back(rollingInTheDeepColors[i]);
        if (i < rollingInTheDeepColors.size() - 1)
        {
            for (int k = 1; k < colorsBetween; k++)
            {
                PixelColor intermediateColor;
                intermediateColor.red = static_cast<unsigned char>(static_cast<unsigned int>(rollingInTheDeepColors[i].red) + (static_cast<unsigned int>(rollingInTheDeepColors[i + 1].red) - static_cast<unsigned int>(rollingInTheDeepColors[i].red)) * k / colorsBetween);
                intermediateColor.green = static_cast<unsigned char>(static_cast<unsigned int>(rollingInTheDeepColors[i].green) + (static_cast<unsigned int>(rollingInTheDeepColors[i + 1].green) - static_cast<unsigned int>(rollingInTheDeepColors[i].green)) * k / colorsBetween);
                intermediateColor.blue = static_cast<unsigned char>(static_cast<unsigned int>(rollingInTheDeepColors[i].blue) + (static_cast<unsigned int>(rollingInTheDeepColors[i + 1].blue) - static_cast<unsigned int>(rollingInTheDeepColors[i].blue)) * k / colorsBetween);
                intermediateColor.alpha = static_cast<unsigned char>(static_cast<unsigned int>(rollingInTheDeepColors[i].alpha) + (static_cast<unsigned int>(rollingInTheDeepColors[i + 1].alpha) - static_cast<unsigned int>(rollingInTheDeepColors[i].alpha)) * k / colorsBetween);
                colors.push_back(intermediateColor);
            }
        }
    }

    return colors;
}