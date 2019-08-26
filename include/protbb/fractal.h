#pragma once

#include <cmath>
#include <memory>
#include <protbb/image.h>

namespace protbb {

class Fractal {
public:
  Fractal(int x, int y, double m = 2000000.0) : mySize{x, y}, myMagn(m)
  { }

  double calPixel(int x0, int y0) {
    auto fx0 = double(x0) - double(mySize[0]) / 2;
    auto fy0 = double(y0) - double(mySize[1]) / 2;
    fx0 = fx0 / myMagn + cx;
    fy0 = fy0 / myMagn + cy;

    double res = 0, x = 0, y = 0;
    for(int iter = 0; x * x + y * y <= 4 && iter < maxIter; ++iter) {
      auto val = x * x - y * y + fx0;
      y = 2 * x * y + fy0;
      x = val;
      res += exp(-sqrt(x*x+y*y));
    }

    return res;
  }

private:
  const int mySize[2];
  double cx = -0.7436;
  const double cy = 0.1319;
  const double myMagn;
  const int maxIter = 1000;
};

static std::shared_ptr<Image> makeFractalImage(int width, int height, double magn = 2000000) {
  auto name = std::string("fractal_") + std::to_string((int)magn);
  auto image = std::make_shared<Image>(name, width, height);
  Fractal fractal(width, height, magn);
  image->fill([&fractal](int x, int y) { return fractal.calPixel(x, y); });
  return image;
}

}
