#include <protbb/fractal.h>

int main() {
  auto image = protbb::makeFractalImage(800, 800, 20000000);
  image->write("fractal_2000.bmp");
  return 0;
}
