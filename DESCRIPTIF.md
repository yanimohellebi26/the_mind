# The Mind — AI-Powered Multiplayer Card Game

## Description

A networked implementation of The Mind — a cooperative card game where players must silently coordinate to play numbered cards in ascending order without communicating. The digital version adds a machine-learning player that has studied 10,000 simulated games to determine the statistically optimal moment to play each card.

## What it brings

The Mind is fundamentally a game about reading implicit signals and building shared intuition. Implementing it as a network game — with TCP socket synchronisation, mutex-protected game state, and an ML-trained robot opponent — turns it into a rigorous systems programming and machine learning challenge. The robot reveals how statistical intuition can approximate the human coordination the game is designed around.

## How it works

The server is a C program that manages game state, enforces turn rules, and broadcasts state changes to all clients via TCP sockets with mutex locks for thread safety. Human clients connect through a terminal interface. The robot player is an XGBoost model trained on a simulation dataset of 10,000 games, where the feature vector encodes the current card value, round number, number of remaining cards, and historical timing patterns. A LaTeX statistical report documents win rates, reaction times, and failure modes.

## Status

✅ Complete — robot trained, multiplayer tested, full statistical report written.

## Tech Stack

`C` · `Python` · `XGBoost` · `scikit-learn` · `TCP Sockets` · `Mutexes` · `SDL2` · `LaTeX`
