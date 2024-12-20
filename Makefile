# Définition des outils et des options
CXX = g++
CFLAGS = -O2 -std=c++17
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INCLUDES = -Ilib/imgui -Ilib/Walnut

# Répertoires
OBJ_DIR = obj
TARGET_DIR = bin
TARGET = $(TARGET_DIR)/raytracer

# Fichiers sources et objets
SRCS = $(shell find src lib -type f -name '*.cpp')
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

# Règles
all: $(TARGET)

# Compilation des fichiers objets dans obj/
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Construction de l'exécutable
$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Nettoyage
clean:
	@rm -rf $(OBJ_DIR) $(TARGET_DIR)

# Exécution
run: $(TARGET)
	$(TARGET)