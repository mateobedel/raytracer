CFLAGS = -Wall -Wextra -O2 -std=c++17
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INCLUDES = -Ilib/imgui

# Fichier sources
SRCS = $(wildcard src/*.cpp)

all: bin/raytracer

bin/raytracer: $(SRCS)
	@mkdir -p bin
	g++ $(CFLAGS) $(INCLUDES) $(SRCS) -o $@ $(LDFLAGS)

clean:
	find bin -type f -delete

run: all
	bin/raytracer
