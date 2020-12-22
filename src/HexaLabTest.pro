DEPENDPATH += . ../src/eigen .
INCLUDEPATH += . ../src/eigen .
CONFIG += console stl c++11

QMAKE_CXXFLAGS += "-Wint-in-bool-context -Wattributes"

TEMPLATE = app
# Mac specific Config required to avoid to make application bundles
CONFIG -= app_bundle

SOURCES += app.cpp \
  builder.cpp \
  mesh.cpp \
  loader.cpp \
  plane_filter.cpp \
  quality_filter.cpp \
  test.cpp \
  color_map.cpp \
  hexalab_js.cpp \
  peeling_filter.cpp \
  pick_filter.cpp

HEADERS = app.h \
  builder.h\
  loader.h\
  # mesh_navigator.h\
  ifilter.h\
  mesh.h \
  model.h \
  plane_filter.h \
  quality_filter.h \ 
  peeling_filter.h \
  hex_quality.h \
  pick_filter.h \
  common.h \
  hex_quality_color_maps.h
