#include <iostream>
#include <fstream>
#include "converting_bmp.h"
#include <string>
Color::Color()
    : r(0), g(0), b(0) {
}

Color::Color(float r, float g, float b)
    : r(r), g(g), b(b) {
}

Color::~Color() {

}

Image::Image(int width, int height)
    : m_width(width), m_height(height), m_colors(std::vector<Color>(width * height)) {
}

Image::~Image() {

}

Color Image::GetColor(int x, int y) const {
  return m_colors[y * m_width + x];
}

void Image::SetColor(const Color& color, int x, int y) {
  m_colors[y * m_width + x].r = color.r;
  m_colors[y * m_width + x].g = color.g;
  m_colors[y * m_width + x].b = color.b;
}

void Image::Export(const std::string& path) const {
  std::ofstream file;

  file.open(path, std::ios::out | std::ios::binary);

  if (!file.is_open()) {
    std::cerr << "The file couldn't be opened";
    exit(216);
  }
  unsigned char bmpPad[3] = {0, 0, 0};
  const int paddingAmount = ((4 - (m_width * 3) % 4) % 4);
  const int fileHeaderSize = 14;
  const int informationHeaderSize = 40;
  const int rgb = 3;
  const uint32_t
      fileSize = fileHeaderSize + informationHeaderSize + m_width * m_height * rgb + paddingAmount * m_height;
  unsigned char fileHeader[fileHeaderSize];
  // File Type
  fileHeader[0] = 'B';
  fileHeader[1] = 'M';
  // File size
  fileHeader[2] = fileSize;
  fileHeader[3] = fileSize >> 8;
  fileHeader[4] = fileSize >> 16;
  fileHeader[5] = fileSize >> 24;
  // Reserved 1 (Not used)
  fileHeader[6] = 0;
  fileHeader[7] = 0;
  // Reserved 2 (Not used)
  fileHeader[8] = 0;
  fileHeader[9] = 0;
  // Pixel data offset
  fileHeader[10] = fileHeaderSize + informationHeaderSize;
  fileHeader[11] = 0;
  fileHeader[12] = 0;
  fileHeader[13] = 0;

  unsigned char informationHeader[informationHeaderSize];

  // Header size
  informationHeader[0] = informationHeaderSize;
  informationHeader[1] = 0;
  informationHeader[2] = 0;
  informationHeader[3] = 0;
  // Image width
  informationHeader[4] = m_width;
  informationHeader[5] = m_width >> 8;
  informationHeader[6] = m_width >> 16;
  informationHeader[7] = m_width >> 24;
  // Image height
  informationHeader[8] = m_height;
  informationHeader[9] = m_height >> 8;
  informationHeader[10] = m_height >> 16;
  informationHeader[11] = m_height >> 24;
  // Planes
  informationHeader[12] = 1;
  informationHeader[13] = 0;
  // Bits per pixel (RGB)
  informationHeader[14] = 24;
  informationHeader[15] = 0;
  // Compression (no compression)
  informationHeader[16] = 0;
  informationHeader[17] = 0;
  informationHeader[18] = 0;
  informationHeader[19] = 0;
  // Image size (no compression)
  informationHeader[20] = 0;
  informationHeader[21] = 0;
  informationHeader[22] = 0;
  informationHeader[23] = 0;
  // X pixels per meter
  informationHeader[24] = 0;
  informationHeader[25] = 0;
  informationHeader[26] = 0;
  informationHeader[27] = 0;
  // Y pixels per meter
  informationHeader[28] = 0;
  informationHeader[29] = 0;
  informationHeader[30] = 0;
  informationHeader[31] = 0;
  // Total colors (Color palette isn't used)
  informationHeader[32] = 0;
  informationHeader[33] = 0;
  informationHeader[34] = 0;
  informationHeader[35] = 0;
  // Important colors (Generally ignored)
  informationHeader[36] = 0;
  informationHeader[37] = 0;
  informationHeader[38] = 0;
  informationHeader[39] = 0;

  file.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
  file.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

  for (int y = 0; y < m_height; ++y) {
    for (int x = 0; x < m_width; ++x) {
      unsigned char r = static_cast<unsigned char>(GetColor(x, y).r * 255.0f);
      unsigned char g = static_cast<unsigned char>(GetColor(x, y).g * 255.0f);
      unsigned char b = static_cast<unsigned char>(GetColor(x, y).b * 255.0f);
      unsigned char color[] = {b, g, r};
      file.write(reinterpret_cast<char*>(color), 3);
    }
    file.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
  }
  file.close();
  std::cout << "File was successfully created" << std::endl;
}