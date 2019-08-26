#pragma once

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <vector>
#include <stdexcept>

namespace protbb {

class Image {
public:
  union Pixel {
    std::uint8_t bgra[4];
    std::uint32_t value;

    Pixel() = default;

    template <typename T>
    Pixel(T b, T g, T r) noexcept : bgra { (std::uint8_t)b, (std::uint8_t)g, (std::uint8_t)r, (std::uint8_t)0 }
    { }
  };

  Image(int width, int height) {
    reset(width, height);
  }

  void write(const char* filename) const {
    if (data.empty())
      throw std::runtime_error("image is empty!");
    std::ofstream stream(filename);
    stream.write((char*)&file.type, 14);
    stream.write((char*)&info, info.size);
    stream.write((char*)&data[0], sizeof(Pixel) * data.size());
  }

  void fill(std::uint8_t r, std::uint8_t g, std::uint8_t b, int x = -1, int y = -1) {
    if (data.empty()) return;
    if (x < 0 || y < 0)
      std::fill(data.begin(), data.end(), Pixel(b, g, r));
    else {
      auto& bgra = data[width * y + x].bgra;
      bgra[3] = 0;
      bgra[2] = r;
      bgra[1] = g;
      bgra[0] = b;
    }
  }

  template <typename F>
  void fill(F f) {
    if (data.empty()) return;
    auto i = -1;
    std::for_each(data.begin(), data.end(), [&i, f, this](auto& pixel) {
      ++i;
      auto x = i / width;
      auto y = i % width;
      auto val = f(x, y);
      if (val > 255) val = 255;
      pixel = Pixel(val, val, val);
    });
  }

public:
  int width, height;
  std::vector<Pixel> data;

private:
  using BitmapFileHeader = struct {
    std::uint16_t pad;
    std::uint16_t type;
    std::uint32_t size;
    std::uint32_t reserved;
    std::uint32_t offBits;
  };

  using BitmapInfoHeader = struct {
    std::uint32_t size;
    std::int32_t  width;
    std::int32_t  height;
    std::uint16_t planes;
    std::uint16_t bitCount;
    std::uint32_t compression;
    std::uint32_t sizeImage;
    std::int32_t  xPelsPerMeter;
    std::int32_t  yPelsPerMeter;
    std::uint32_t clrUsed;
    std::uint32_t clrImportant;
  };

  BitmapFileHeader file;
  BitmapInfoHeader info;

  void reset(int width, int height) {
    if (width <= 0 || height <= 0)
      throw std::range_error("invalid image size!");

    this->width = width;
    this->height = height;
    data.resize(width * height);

    auto padSize = (4 - (sizeof(Pixel) * width) % 4) % 4;
    auto sizeData = width * height * sizeof(Pixel) + padSize * height;
    auto sizeAll = sizeData + sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);

    file.type = 0x4d42;
    file.size = sizeAll;
    file.reserved = 0;
    file.offBits = 54;

    info.size = 40;
    info.width = width;
    info.height = height;
    info.planes = 1;
    info.bitCount = 32;
    info.compression = 0;
    info.sizeImage = sizeData;
    info.xPelsPerMeter = 0;
    info.yPelsPerMeter = 0;
    info.clrUsed = 0;
    info.clrImportant = 0;
  }
};

}
