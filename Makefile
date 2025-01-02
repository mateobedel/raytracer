# Définition des outils et des options (tbb for multithreading)
CXX = g++
CFLAGS = -O2 -std=c++17
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -ltbb
INCLUDES = -Ilib -Ilib/imgui

# Répertoires
OBJ_DIR = obj
TARGET_DIR = bin
TARGET = $(TARGET_DIR)/raytracer
LOG_DIR = log

# Fichiers sources et objets
SRCS = $(shell find src lib -type f -name '*.cpp')
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

# Compilation des fichiers objets dans obj/
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Construction de l'exécutable
$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)


all: build

# Compilation (debug)
debug: CFLAGS += -D_DEBUG
debug: $(TARGET)

# Compilation (release)
build: $(TARGET)

# Exécution
run: $(TARGET)
	$(TARGET)

# Execution pour analyse des performances
gprof-run: CFLAGS += -pg
gprof-run: LDFLAGS += -pg
gprof-run: clean $(TARGET)
gprof-run:
	@mkdir -p $(LOG_DIR)
	GMON_OUT_PREFIX=$(LOG_DIR)/gmon ./bin/raytracer

# Creation des logs des performances
gprof-log:
	@mkdir -p $(LOG_DIR)
	gprof $(TARGET) $(LOG_DIR)/gmon.* > $(LOG_DIR)/perf.log


# Nettoyage
clean:
	@rm -rf $(OBJ_DIR) $(TARGET_DIR) $(LOG_DIR)

