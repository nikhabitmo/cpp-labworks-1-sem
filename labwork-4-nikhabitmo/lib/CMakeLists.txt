add_library(parsing parsing.cpp parsing.h)
add_library(hamarc hamarc.cpp hamarc.h)
add_library(archive archive.cpp archive.h hamarc)
