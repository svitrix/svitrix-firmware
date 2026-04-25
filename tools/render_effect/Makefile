CXX      ?= clang++
ROOT     := $(abspath $(CURDIR)/../..)
CXXFLAGS := -std=c++17 -O2 -Wall -Wno-unused-function -Wno-conversion \
            -DUNIT_TEST \
            -I. \
            -I$(ROOT)/test/mocks \
            -I$(ROOT)/lib/interfaces/src \
            -I$(ROOT)/lib/services/src \
            -I$(ROOT)/src/effects \
            -I$(ROOT)/lib/MeanFilterLib/src \
            -I$(ROOT)/src \
            -I$(ROOT)/src/data

# This dir's FastLED.h takes precedence over test/mocks/FastLED.h via -I order.
TARGET   := render_effect

# Render parameters — override on the command line, e.g. `make all-gifs FRAMES=420`.
FPS    ?= 42
FRAMES ?= 252  # 252 / 42 fps = 6 s loop
SCALE  ?= 32   # 32 → 1024x256 (matches docs/assets/*.gif)

.PHONY: all clean all-gifs
all: $(TARGET)

$(TARGET): render_effect.cpp FastLED.h
	$(CXX) $(CXXFLAGS) -o $@ render_effect.cpp

# Convenience target — make Fire.gif builds the binary if needed and renders.
%.gif: $(TARGET)
	./$(TARGET) --effect $* --fps $(FPS) --frames $(FRAMES) --scale $(SCALE) --out $@

# Render every effect listed by `./render_effect --list` at device-default fps.
all-gifs: $(TARGET)
	@./$(TARGET) --list | while read e; do \
		echo "→ $$e.gif"; \
		./$(TARGET) --effect "$$e" --fps $(FPS) --frames $(FRAMES) --scale $(SCALE) --out "$$e.gif" >/dev/null; \
	done
	@echo "Done — $$(ls *.gif 2>/dev/null | wc -l | tr -d ' ') GIFs"

clean:
	rm -f $(TARGET) *.gif
