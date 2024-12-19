# Flags
CXXFLAGS = -Wall -Wextra -O2

# Fichiers sources
SRCS = $(wildcard src/*.cpp)

# Fichiers objets
OBJS = $(SRCS:src/%.cpp=obj/%.o)

# Règle par défaut
all: bin/raytracer

# Création de l'exécutable
bin/raytracer: $(OBJS)
	@mkdir -p bin
	g++ $(OBJS) -o $@

# Compilation des obj
obj/%.o: src/%.cpp
	@mkdir -p obj
	g++ $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf obj bin


run: all
	bin/raytracer
