// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_SPLINES_H
#define CYD_UI_SPLINES_H


double lerp(double x0, double x1, double t) {
  return (1 - t) * x0 + t * x1;
}

double cubic_bezier(
  double x0,
  double x1,
  double x2,
  double x3,
  double t
) {
  return x0 +
    t * (-3 * x0 + 3 * x1) +
    t * t * (3 * x0 + -6 * x1 + 3 * x2) +
    t * t * t * (-1 * x0 + 3 * x1 + -3 * x2 + x3);
}

#endif //CYD_UI_SPLINES_H
