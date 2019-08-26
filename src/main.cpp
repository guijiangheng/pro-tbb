#include <iostream>
#include <tbb/tbb.h>
#include <protbb/fractal.h>

using namespace protbb;
using ImagePtr = std::shared_ptr<Image>;

ImagePtr applyGamma(const ImagePtr& image, double gamma) {
  auto width = image->width;
  auto height = image->height;
  auto outImage = std::make_shared<Image>(image->name + "_gamma", width, height);

  tbb::parallel_for(0, height, [&image, &outImage, width, height, gamma](int i) {
    for (auto j = 0; j < width; ++j) {
      auto index = i * width + j;
      auto& p = image->data[index];
      auto v = 0.3 * p.bgra[2] + 0.59 * p.bgra[1] + 0.11 * p.bgra[0];
      auto res = std::pow(v, gamma);
      outImage->data[index] = Image::Pixel(res, res, res);
    }
  });

  return outImage;
}

ImagePtr applyTint(const ImagePtr& image, const double *tints) {
  auto width = image->width;
  auto height = image->height;
  auto outImage = std::make_shared<Image>(image->name + "_tinted", width, height);

  tbb::parallel_for(0, height, [&image, &outImage, width, height, tints](int i) {
    for (auto j = 0; j < width; ++j) {
      auto index = i * width + j;
      auto& p = image->data[index];
      auto b = (std::uint8_t)(p.bgra[0] + (255 - p.bgra[0]) * tints[0]);
      auto g = (std::uint8_t)(p.bgra[1] + (255 - p.bgra[1]) * tints[1]);
      auto r = (std::uint8_t)(p.bgra[2] + (255 - p.bgra[2]) * tints[2]);
      outImage->data[index] = Image::Pixel(b, g, r);
    }
  });

  return outImage;
}

void writeImage(const ImagePtr& image) {
  image->write((image->name + ".bmp").c_str());
}

void fig_1_7(const std::vector<ImagePtr>& images) {
  const double tints[] = { 0.75, 0, 0 };
  for (auto& image : images) {
    writeImage(applyTint(applyGamma(image, 1.4), tints));
  }
}

void fig_1_10(const std::vector<ImagePtr>& images) {
  const double tints[] = { 0.75, 0, 0 };

  int i = 0;
  tbb::flow::graph g;

  tbb::flow::source_node<ImagePtr> src(g, [&i, &images](ImagePtr& out) {
    if (i < images.size()) {
      out = images[i++];
      return true;
    }
    return false;
  }, false);

  tbb::flow::function_node<ImagePtr, ImagePtr> gamma(g,
    tbb::flow::unlimited,
    [](const ImagePtr& img) -> ImagePtr {
      return applyGamma(img, 1.4);
    }
  );

  tbb::flow::function_node<ImagePtr, ImagePtr> tint(g,
    tbb::flow::unlimited,
    [tints](const ImagePtr& img) -> ImagePtr {
      return applyTint(img, tints);
    }
  );

  tbb::flow::function_node<ImagePtr> write(g,
    tbb::flow::unlimited,
    [](const ImagePtr& img) {
      writeImage(img);
    }
  );

  tbb::flow::make_edge(src, gamma);
  tbb::flow::make_edge(gamma, tint);
  tbb::flow::make_edge(tint, write);
  src.activate();
  g.wait_for_all();
}

int main() {
  std::vector<ImagePtr> images;
  for (auto i = 2000; i < 20000000; i *= 10)
    images.push_back(makeFractalImage(800, 800, i));
  auto now = tbb::tick_count::now();
  fig_1_10(images);
  std::cout << "Time: " << (tbb::tick_count::now() - now).seconds() << " seconds" << std::endl;
  return 0;
}
