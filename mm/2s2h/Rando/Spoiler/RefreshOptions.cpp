#include "Spoiler.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include <filesystem>
#include "BenPort.h"

#include <libultraship/libultra/types.h>

std::vector<std::string> Rando::Spoiler::spoilerOptions;
const std::filesystem::path randomizerFolderPath(Ship::Context::GetPathRelativeToAppDirectory("randomizer",
                                                                                              appShortName));

void Rando::Spoiler::SelectSpoiler(s32 index) {
    bool generateNew = (index <= 0) || (index >= (s32)Rando::Spoiler::spoilerOptions.size());

    CVarSetInteger("gRando.SpoilerFileIndex", generateNew ? 0 : index);
    CVarSetString("gRando.SpoilerFile", generateNew ? "" : Rando::Spoiler::spoilerOptions[index].c_str());
}

// This function refreshes the list of spoiler files in the randomizer folder, this list is used in the Randomizer UI,
// and also includes an option to generate a new seed at the top of the list.
void Rando::Spoiler::RefreshOptions() {
    Rando::Spoiler::spoilerOptions.clear();

    Rando::Spoiler::spoilerOptions.push_back("Generate New Seed");
    s32 spoilerFileIndex = -1;

    // ensure the randomizer folder exists; never let a filesystem error (e.g. on console SD cards) escape
    std::error_code ec;
    if (!std::filesystem::exists(randomizerFolderPath, ec)) {
        std::filesystem::create_directories(randomizerFolderPath, ec);
    }

    // Add all files in the randomizer folder to the list of spoiler options
    for (const auto& entry : std::filesystem::directory_iterator(randomizerFolderPath, ec)) {
        if (entry.is_regular_file(ec)) {
            std::string fileName = entry.path().filename().string();
            Rando::Spoiler::spoilerOptions.push_back(fileName);

            // Check if the current file is the one set in the cvar
            if (fileName == CVarGetString("gRando.SpoilerFile", "")) {
                spoilerFileIndex = Rando::Spoiler::spoilerOptions.size() - 1;
            }
        }
    }

    // If the current spoiler file is not in the randomizer folder, reset the cvar
    if (spoilerFileIndex == -1) {
        CVarClear("gRando.SpoilerFileIndex");
        CVarClear("gRando.SpoilerFile");
    } else {
        CVarSetInteger("gRando.SpoilerFileIndex", spoilerFileIndex);
    }
}
