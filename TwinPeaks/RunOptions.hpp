#ifndef TwinPeaks_RunOptions_hpp
#define TwinPeaks_RunOptions_hpp

#include <fstream>
#include <iomanip>

namespace TwinPeaks
{

// This should be fairly self explanatory
class RunOptions
{
    private:
        int num_particles;
        int mcmc_steps;
        int thin;
        double depth;

    public:

        // Just specify depth, use defaults for the others
        RunOptions(double _depth);

        // Specify everything
        RunOptions(int _num_particles, int _mcmc_steps,
                   int _thin, double _depth);

        // Output to YAML file
        void save() const;

        // Friend
        template <typename T>
        friend class Sampler;

};

/* IMPLEMENTATIONS FOLLOW */

RunOptions::RunOptions(double _depth)
:num_particles(1000)
,mcmc_steps(1000)
,thin(1)
,depth(_depth)
{

}

RunOptions::RunOptions(int _num_particles, int _mcmc_steps,
                       int _thin, double _depth)
:num_particles(_num_particles)
,mcmc_steps(_mcmc_steps)
,thin(_thin)
,depth(_depth)
{

}

void RunOptions::save() const
{
    std::fstream fout("output/run_options.yaml", std::ios::out);
    fout << std::setprecision(12);
    fout << "---\n";
    fout << "num_particles: " << num_particles << '\n';
    fout << "mcmc_steps: " << num_particles << '\n';
    fout << "thin: " << thin << '\n';
    fout << "depth: " << depth << std::endl;
    fout.close();
}

} // namespace

#endif

