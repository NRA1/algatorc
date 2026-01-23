#include "Execution.hpp"
#include "../Support/FileManagement.hpp"
#include "../Support/Configuration.hpp"

std::pair<void*, std::vector<long int>> execute(const AlgorithmLibrary& algorithm, void* input)
{
    const int times_to_execute = Configuration::timesToExecute();

    std::vector<long int> times;
    times.reserve(times_to_execute);

    void* output = nullptr;
    for (int i = 0; i < times_to_execute; i++)
    {
        const clock_t before = std::clock();
        output = algorithm.execute(input);
        const clock_t after = std::clock();
        const clock_t elapsed = ((after - before) * 1000000) / CLOCKS_PER_SEC;
        times.push_back(elapsed);

        if (i != times_to_execute - 1)
            algorithm.freeAll();
    }

    return {output, times};
}