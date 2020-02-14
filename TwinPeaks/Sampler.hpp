#ifndef TwinPeaks_Sampler_hpp
#define TwinPeaks_Sampler_hpp

#include "Constraints.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include "Misc.hpp"
#include "Ordering.hpp"
#include <ostream>
#include "RNG.hpp"
#include "RunOptions.hpp"
#include "sqlite3.h"
#include <vector>

namespace TwinPeaks
{

//static int callback(void *NotUsed, int argc, char **argv, char **azColName)
//{
//    int i;
//    for(i=0; i<argc; i++){
//      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//    }
//    printf("\n");
//    return 0;
//}
  

/*
* An object of this class is the current state of a sampler.
*/
template<typename T>
class Sampler
{
    private:

        // Go me
        sqlite3* db;

        // Run options
        RunOptions run_options;

        // Iteration counter
        int iteration;

        // Validity flag - can the sampler be run?
        bool valid;

        // The particles
        std::vector<T> particles;

        // The particles' scalars, tiebreakers, and ranks
        std::vector<double> xs, ys;
        std::vector<int> x_indices, y_indices;
        std::vector<int> x_ranks, y_ranks;

        // Positions along space filling curve
        std::vector<double> ds, dtbs;

        // Forbidden rectangles
        Constraints constraints;

        // Index of worst particle
        int worst;

        // Compute all ordering-related stuff
        void compute_orderings();

        // Write dead particle output to files
        void write_output() const;

        // Do one NS iteration
        void advance(RNG& rng, bool last_iteration=false);

    public:

        // Constructor where you provide the number of particles
        // and an RNG for particle initialisation
        Sampler(RunOptions _run_options, RNG& rng);

        // Custom destructor
        ~Sampler();

        // Disable other ones
        Sampler() = delete;
        Sampler(const Sampler&) = delete;
        Sampler(Sampler&&) = delete;
        Sampler& operator = (const Sampler&) = delete;

        // Run to target depth
        void run(RNG& rng);
};


/* IMPLEMENTATIONS FOLLOW */

template<typename T>
Sampler<T>::Sampler(RunOptions _run_options, RNG& rng)
:run_options(std::move(_run_options))
,iteration(1)
,valid(true)
,particles(run_options.num_particles)
,xs(run_options.num_particles)
,ys(run_options.num_particles)
,x_indices(run_options.num_particles)
,y_indices(run_options.num_particles)
,x_ranks(run_options.num_particles)
,y_ranks(run_options.num_particles)
,ds(run_options.num_particles)
,dtbs(run_options.num_particles)
{
    // Set up DB
    sqlite3_open("test.db", &db);
    std::string query = "BEGIN;\
                         CREATE TABLE particle_info \
                             (id INTEGER PRIMARY KEY,\
                              x  REAL NOT NULL,\
                              y  REAL NOT NULL,\
                              x_rank INTEGER NOT NULL,\
                              y_rank INTEGER NOT NULL) WITHOUT ROWID;\
                         CREATE INDEX x_idx ON particle_info (x);\
                         CREATE INDEX y_idx ON particle_info (y);\
                         COMMIT;";
    sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);



    // Generate particles from the prior.
    std::cout << "# Generating " << run_options.num_particles << ' ';
    std::cout << "particles from the prior..." << std::flush;

    sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr);

    for(int i=0; i<run_options.num_particles; ++i)
    {
        particles[i].from_prior(rng);
        auto scalars = particles[i].get_scalars();
        std::tie(xs[i], ys[i]) = scalars;

        
        sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);
    }

    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);

    compute_orderings();
    std::cout << "done.\n#" << std::endl;

    // Save a record of the run options
    run_options.save();
}

template<typename T>
Sampler<T>::~Sampler()
{
    sqlite3_close(db);
}


template<typename T>
void Sampler<T>::compute_orderings()
{
    // Ranks of all particles
    std::tie(x_indices, x_ranks) = indices_and_ranks(xs);
    std::tie(y_indices, y_ranks) = indices_and_ranks(ys);

    // Map ranks to total order
    for(int i=0; i<run_options.num_particles; ++i)
    {
        ds[i] = d(x_ranks[i], y_ranks[i], true);
        dtbs[i] = dtb(x_ranks[i], y_ranks[i], true);
    }

    // Find worst particle
    worst = 0;
    for(int i=1; i<run_options.num_particles; ++i)
    {
        if(ds[i] < ds[worst])
            worst = i;
        if((ds[i] == ds[worst]) && (dtbs[i] < dtbs[worst]))
            worst = i;
    }
}

template<typename T>
void Sampler<T>::run(RNG& rng)
{
    if(!valid)
    {
        std::cerr << "Cannot run sampler. It's probably already been done.";
        std::cerr << std::endl;
        return;
    }
    int iterations = static_cast<int>(run_options.num_particles
                                                *run_options.depth);
    for(int i=0; i<iterations; ++i)
        advance(rng, i==iterations-1);
    valid = false;
}


template<typename T>
void Sampler<T>::advance(RNG& rng, bool last_iteration)
{
    bool output_message = run_options.num_particles <= 100 ||
                                iteration % run_options.num_particles == 0;

    // Progress message
    if(output_message)
    {
        std::cout << std::setprecision(12);
        std::cout << "# Iteration " << iteration << ". ";
        std::cout << "Depth ~= " << (double)iteration/run_options.num_particles << " nats.\n";
        std::cout << "# Standard deviation of sum of ranks = ";
        double tot = 0.0;
        double tot_sq = 0.0;
        int n = run_options.num_particles;
        for(int i=0; i<n; ++i)
        {
            tot += (x_ranks[i] + y_ranks[i]);
            tot_sq += pow(x_ranks[i] + y_ranks[i], 2);
        }
        std::cout << sqrt(tot_sq/n - pow(tot/n, 2)) << '.';
        std::cout << std::endl;

        std::cout << "# Number of forbidden rectangles = ";
        std::cout << constraints.size() << "." << std::endl;
    }

    // Save worst particle
    write_output();

    if(output_message)
    {
        std::cout << "#    Worst particle's scalars = (" << xs[worst] << ", ";
        std::cout << ys[worst] << ")." << std::endl;
    }

    if(last_iteration)
        return;

    // Add new forbidden rectangles
    for(int iy=0; iy<=run_options.num_particles; ++iy)
    {
        int highest_bad = -1;
        for(int ix=0; ix<=run_options.num_particles; ++ix)
        {
            // Check if the current rectangle is bad
            if(d(ix, iy) < ds[worst] ||
                        (d(ix, iy) == ds[worst] && dtb(ix, iy) < dtbs[worst]))
                highest_bad = ix;
        }

        // If it's bad, add it to the constraints
        if(iy==0 && highest_bad == run_options.num_particles) // Bottom right case
            constraints.add_rectangle(1E300, ys[y_indices[iy]]);
        else if(highest_bad == 0 && iy == run_options.num_particles) // Top left case
            constraints.add_rectangle(xs[x_indices[highest_bad]], 1E300);
        else if(highest_bad != -1) // Standard case
            constraints.add_rectangle(xs[x_indices[highest_bad]], ys[y_indices[iy]]);

    }

    if(output_message)
        std::cout << "#    Generating new particle..." << std::flush;

    // Generate new particle
    // First, clone
    int copy = 0;
    do
    {
        copy = rng.rand_int(run_options.num_particles);
    }while(run_options.num_particles > 1 && copy == worst);
    T new_particle = particles[copy];
    double new_x = xs[copy];
    double new_y = ys[copy];

    // Now do Metropolis
    int accepted = 0;
    for(int i=0; i<run_options.mcmc_steps; ++i)
    {
        // Make proposal

        // Particle
        auto proposal = new_particle;
        double logH = proposal.perturb(rng);

        // Get its scalars
        double proposal_x, proposal_y;
        std::tie(proposal_x, proposal_y) = proposal.get_scalars();

        if(constraints.test(proposal_x, proposal_y) &&
           (rng.rand() <= exp(logH)))
        {
            new_particle = proposal;
            new_x = proposal_x;
            new_y = proposal_y;
            ++accepted;
        }

    }

    // Insert new particle and recompute stuff
    particles[worst] = new_particle;
    xs[worst] = new_x;
    ys[worst] = new_y;

    if(output_message)
    {
        std::cout << "done. ";
        std::cout << "Metropolis acceptance rate = ";
        std::cout << accepted << '/' << run_options.mcmc_steps << '.' << std::endl;
    }

    // Compute new orderings.
    if(output_message)
        std::cout << "#    Computing new orderings..." << std::flush;

    compute_orderings();

    if(output_message)
        std::cout << "done." << std::endl;

    // Increment iteration counter
    ++iteration;

    if(output_message)
        std::cout << '#' << std::endl;
}


template<typename T>
void Sampler<T>::write_output() const
{
    // Open the output file
    std::fstream fout;
    if(iteration == 1)
        fout.open("output/scalars.csv", std::ios::out);
    else
        fout.open("output/scalars.csv", std::ios::out | std::ios::app);

    // Set precision
    fout << std::setprecision(12);

    // Output scalars
    fout << iteration << ',';
    fout << xs[worst] << ',';
    fout << ys[worst] << std::endl;

    // Close the output file
    fout.close();

    // Now output entire particle
    if(iteration % run_options.thin != 0)
        return;

    if(iteration / run_options.thin == 1)
        fout.open("output/particles.csv", std::ios::out);
    else
        fout.open("output/particles.csv", std::ios::out | std::ios::app);
    fout << iteration << ',';
    fout << particles[worst].render() << std::endl;
    fout.close();
}

} // namespace

#endif

