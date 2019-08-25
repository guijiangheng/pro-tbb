#include <iostream>
#include <vector>
#include <pstl/algorithm>
#include <pstl/execution>
#include <protbb/image.h>

int main() {
  std::vector<std::string> v = { "Hello ", "Parallel STL!" };
  std::for_each(pstl::execution::par, v.begin(), v.end(), [](std::string& s) {
    std::cout << s << std::endl;
  });

  protbb::Image image(1376, 618);
  image.fill(255, 0, 0);
  image.write("a.bmp");

  return 0;
}
