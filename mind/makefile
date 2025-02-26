# Compiler
CC = gcc

# Options générales
CFLAGS = -Wall -Wextra -g -Wno-unused-parameter -Wno-format -Wno-return-type -Iinclude

# Options spécifiques à Python
PYTHON_CFLAGS = -I/usr/include/python3.12 -I/usr/include/python3.12 -fno-strict-overflow -Wsign-compare -DNDEBUG -g -O2 -Wall -Wno-unused-parameter -Wno-format -Wno-return-type -Iinclude
PYTHON_LDFLAGS = -L/usr/lib/python3.12/config-3.12-x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu -ldl -lm -lpython3.12 -lpthread

# Options pour SDL2 et SDL2_ttf (notez l'utilisation de SDL2_ttf au lieu de sdl2_ttf)
SDL_CFLAGS = $(shell pkg-config --cflags sdl2 SDL2_ttf)
SDL_LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_ttf)

# Répertoires
SRC_DIR = src
BIN_DIR = bin
INCLUDE_DIR = include

# Cibles
TARGETS = joueur_robot serveur joueur_humain
TARGETS_BIN = $(addprefix $(BIN_DIR)/, $(TARGETS))

all: $(TARGETS_BIN)

$(BIN_DIR)/joueur_robot: $(SRC_DIR)/joueur_robot.c $(SRC_DIR)/interface.c $(INCLUDE_DIR)/joueur_robot.h $(INCLUDE_DIR)/interface.h
	$(CC) $(CFLAGS) $(PYTHON_CFLAGS) $(SDL_CFLAGS) -o $@ $^ $(PYTHON_LDFLAGS) $(SDL_LDFLAGS)

$(BIN_DIR)/joueur_humain: $(SRC_DIR)/joueur_humain.c $(INCLUDE_DIR)/joueur_humain.h
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/serveur: $(SRC_DIR)/serveur.c $(INCLUDE_DIR)/serveur.h
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(BIN_DIR)/*

.PHONY: all clean
