// PetLoader.hpp
#pragma once

#include "VirtualPetState.hpp"
#include <QString>

namespace PetLoader {

bool loadPet(const QString &rootDir, VirtualPetState &outState);

}
