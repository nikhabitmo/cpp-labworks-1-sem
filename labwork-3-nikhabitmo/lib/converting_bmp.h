#pragma once

#include <iostream>
#include <vector>

struct Color {
  float r;
  float g;
  float b;
  Color();
  Color( float r, float g, float b);
  ~Color();
};

class Image {
 public:
  Image(int width, int height);
  ~Image();
  Color GetColor(int x, int y) const;
  void SetColor(const Color& color, int x, int y);
  void Export(const std::string& path) const;
 private:
  int m_width;
  int m_height;
  std::vector<Color> m_colors;
};