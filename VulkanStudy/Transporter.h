#pragma once

#include <fstream>

auto GetBinary(const wchar_t* filename) {
  FILE* fp;
  if (_wfopen_s(&fp, filename, L"rb") != 0) throw std::runtime_error("File not found");

  fseek(fp, 0, SEEK_END);
  auto size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  auto buf = std::make_unique<unsigned char[]>(size);
  fread(buf.get(), sizeof(unsigned char), size, fp);
  fclose(fp);
  return std::pair<std::unique_ptr<unsigned char[]>, size_t>(std::move(buf), size);
}
