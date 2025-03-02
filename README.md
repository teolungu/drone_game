# Overview

This project is a drone simulation framework designed for testing, development, and visualization. It includes core simulation components, graphics rendering, and various utilities.

# Project Structure

├── src/            # Source code for the simulation 
│   ├── components/ # Modular components for drone operations 
│   ├── core/       # Core logic and algorithms 
│   ├── utils/      # Utility functions 
│   ├── main.cpp    # Entry point of the application 
│   ├── lab_m1/     # Experimental modules (Lab 1) 
│   ├── lab_m2/     # Experimental modules (Lab 2) 
│   ├── lab_extra/  # Additional experimental features 
│ 
├── docs/           # Documentation files
│   ├── home.md     # Main documentation
│   ├── dev/        # Developer guides
│   ├── user/       # User guides
│
├── assets/         # 3D models, shaders, and textures
│   ├── models/     # 3D models for visualization
│   ├── shaders/    # Graphics shaders
│   ├── textures/   # Textures for rendering
│
├── build/          # Build artifacts and compiled files
│
├── deps/           # External dependencies
│   ├── api/        # API dependencies
│   ├── prebuilt/   # Prebuilt libraries
│
├── infra/          # Infrastructure and build utilities
│   ├── utils.cmake # CMake utilities
│
├── tools/          # Development tools and scripts
│   ├── deps-*.sh   # Dependency installation scripts
│   ├── gather_licenses.py # Script for managing licenses
│   ├── install-cmake.sh   # CMake installation script
│
├── CMakeLists.txt  # CMake configuration file
