/*
 * This file is part of LSST M1M3 thermal system package.
 *
 * Developed for the LSST Data Management System.
 * This product includes software developed by the LSST Project
 * (https://www.lsst.org).
 * See the COPYRIGHT file at the top-level directory of this distribution
 * for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <Settings/FCUApplicationSettings.h>

using namespace LSST::M1M3::TS::Settings;

// Coordinates are in inches, converted to M in constructor
FCUTableRow FCUApplicationSettings::Table[cRIO::NUM_TS_ILC] = {
        {1, 32.127, 153.134},    {2, 15.197, 148.083},    {3, -15.197, 148.083},   {4, -32.127, 153.134},
        {5, 43.574, 128.593},    {6, 17.361, 128.593},    {7, -17.361, 128.593},   {8, -43.574, 128.593},
        {9, 30.582, 105.947},    {10, -2.114, 102.079},   {11, -30.582, 105.947},  {12, 95.999, 83.191},
        {13, 69.787, 83.191},    {14, 17.361, 83.191},    {15, -17.361, 83.191},   {16, -69.787, 83.191},
        {17, -95.999, 83.191},   {18, 135.433, 60.545},   {19, 109.220, 60.545},   {20, 83.007, 60.545},
        {21, 61.133, 60.527},    {22, 35.013, 60.603},    {23, 2.256, 64.281},     {24, -35.013, 60.603},
        {25, -61.133, 60.527},   {26, -83.007, 60.545},   {27, -109.220, 60.545},  {28, -135.433, 60.545},
        {29, 122.407, 45.270},   {30, 95.999, 37.789},    {31, 67.781, 41.744},    {32, 43.573, 37.789},
        {33, 17.361, 37.789},    {34, -17.361, 37.789},   {35, -43.574, 37.789},   {36, -67.781, 41.744},
        {37, -95.999, 37.789},   {38, -122.407, 45.270},  {39, 148.061, 11.244},   {40, 109.220, 15.143},
        {41, -83.007, 15.143},   {42, 35.080, 15.221},    {43, -35.080, 15.221},   {44, -83.007, 15.143},
        {45, -109.220, 15.143},  {46, -148.061, 11.244},  {47, 162.090, -11.030},  {48, 148.063, -11.318},
        {49, 95.999, -7.613},    {50, 69.787, -7.613},    {51, -69.787, -7.613},   {52, -95.999, -7.613},
        {53, -147.988, -11.351}, {54, -162.090, -11.030}, {55, 135.433, -30.259},  {56, 109.220, -30.259},
        {57, 83.007, -30.259},   {58, 56.568, -30.230},   {59, 30.582, -30.259},   {60, 15.481, -34.351},
        {61, -15.481, -34.351},  {62, -30.582, -30.259},  {63, -56.568, -30.230},  {64, -83.007, -30.259},
        {65, -109.220, -30.259}, {66, -135.433, -30.259}, {67, 122.212, -53.015},  {68, 69.787, -53.015},
        {69, 43.688, -52.969},   {70, -43.688, -52.969},  {71, -69.787, -53.015},  {72, -122.212, -53.015},
        {73, 137.048, -75.565},  {74, 83.007, -75.661},   {75, 56.795, -75.661},   {76, 29.098, -78.299},
        {77, -2.142, -71.747},   {78, -29.098, -78.299},  {79, -56.795, -75.661},  {80, -83.007, -75.661},
        {81, -137.048, -75.565}, {82, 95.999, -98.417},   {83, 43.574, -98.417},   {84, 17.361, -98.417},
        {85, -17.361, -98.417},  {86, -43.574, -98.417},  {87, -95.999, -98.417},  {88, 30.572, -113.475},
        {89, 8.760, -121.101},   {90, -8.760, -121.101},  {91, -30.572, -113.475}, {92, 71.398, -138.476},
        {93, 43.574, -143.819},  {94, 17.500, -143.444},  {95, -17.500, -143.444}, {96, -71.398, -138.476},
};

FCUApplicationSettings::FCUApplicationSettings(token) {}
