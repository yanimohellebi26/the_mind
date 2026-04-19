# The Mind — AI-Powered Multiplayer Card Game

## Tagline
Networked C implementation of "The Mind" with an XGBoost-trained robot player that approximates human coordination.

## Description
A networked implementation of the cooperative card game "The Mind" in C, featuring a multi-threaded TCP server with mutex-protected game state, human and robot clients. The robot player uses an XGBoost regression model trained on 10,000 simulated games to predict optimal play timing. Includes full statistics collection, analysis, and LaTeX report generation.

## Motivation
A rigorous systems programming challenge combining TCP socket synchronization, concurrent state management, and statistical AI — training a machine to approximate human intuition.

## Tech Stack
- **Server/Client**: C, TCP Sockets, pthreads, mutexes
- **AI/ML**: Python, XGBoost, scikit-learn, joblib
- **Build**: Makefile
- **Documentation**: LaTeX

## Key Features
- Multi-threaded TCP server with mutex-protected shared game state
- Human and robot clients with separate decision logic
- XGBoost regression model trained on 10,000 simulated games
- Feature vector: card value, last played card, player count → predicted wait time
- Full statistics collection, analysis, and LaTeX report generation

## Skills Demonstrated
- Systems programming in C (sockets, threads, synchronization)
- Concurrent programming (mutexes, shared state)
- Machine learning model training (XGBoost)
- Network protocol design
- Statistical analysis and report generation

## Category
`Systems Programming` · `AI/ML` · `Game` · `Networking` · `C`

## Status
Complete

## Complexity
⭐⭐⭐⭐⭐ Advanced

## Links
- GitHub: https://github.com/yanimohellebi26/the_mind
