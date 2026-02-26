#include "Execution.hpp"

#include "ProjectLibrary.hpp"
#include "../Support/FileManagement.hpp"
#include "../Support/Configuration.hpp"
#include "../Support/Guard.hpp"

void execute(ProjectLibrary& project, AlgorithmLibrary& algorithm)
{
    const bool deserialize_each_execute = Configuration::deserializeEachExecute();

    auto [buffer, file_size] = readBinaryFile(Configuration::inputFilePath());
    void* input = nullptr;
    if (!deserialize_each_execute)
        input = project.deserializeInput(buffer, file_size);

    Error::setPhase(ErrorPhase::Execution);

    const unsigned int times_to_execute = Configuration::timesToExecute();

    std::vector<long int> times;
    times.reserve(times_to_execute);

    void* output = nullptr;
    for (int i = 0; i < times_to_execute; i++)
    {
        if (deserialize_each_execute)
            input = project.deserializeInput(buffer, file_size);

        const clock_t before = std::clock();
        output = algorithm.execute(input);
        const clock_t after = std::clock();
        const clock_t elapsed = ((after - before) * 1000000) / CLOCKS_PER_SEC;
        times.push_back(elapsed);

        if (i != times_to_execute - 1)
            algorithm.freeAll();
    }

    Error::setPhase(ErrorPhase::Teardown);

    unsigned int output_len;
    const char* serialized_output = project.serializeOutput(output, &output_len);

    writeBinaryFile(Configuration::outputFilePath(), serialized_output, output_len);

    algorithm.freeAll();
    project.freeAll();

    guardInternal([&]
    {
        delete[] buffer;
    });

    writeSuccessStatusFile(times);
}