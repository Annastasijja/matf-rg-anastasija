#include "CinemaApp.hpp"

#include <memory>

int main(int argc, char **argv) { return std::make_unique<CinemaApp>()->run(argc, argv); }
