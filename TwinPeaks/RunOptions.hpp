#ifndef TwinPeaks_RunOptions_hpp
#define TwinPeaks_RunOptions_hpp

namespace TwinPeaks
{

//// Forward declare
//template<typename T>
//class Sampler<T>;


// This should be fairly self explanatory
class RunOptions
{
    private:
        int num_particles;
        int mcmc_steps;
        double depth;

    public:

        // Just specify depth, use defaults for the others
        RunOptions(double _depth);


        // Specify everything
        RunOptions(int _num_particles, int _mcmc_steps, double _depth);

//        // Friend
//        template<typename T>
//        friend class Sampler<T>;

};

/* IMPLEMENTATIONS FOLLOW */

} // namespace

#endif

