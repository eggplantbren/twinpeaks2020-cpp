#ifndef TwinPeaks_Misc_hpp
#define TwinPeaks_Misc_hpp

#include <algorithm>
#include <cassert>
#include <cmath>

namespace TwinPeaks
{

double mod(double y, double x);
int mod(int y, int x);
void wrap(double& x, double min, double max);
double logsumexp(const std::vector<double>& logv);

/* IMPLEMENTATIONS FOLLOW */

double mod(double y, double x)
{
    assert(x > 0.0);
    return (y/x - floor(y/x))*x;
}

void wrap(double& x, double min, double max)
{
    x = TwinPeaks::mod(x - min, max - min) + min;
}

int mod(int y, int x)
{
    assert(x > 0);
    if(y >= 0)
        return y - (y/x)*x;
    else
        return (x-1) - TwinPeaks::mod(-y-1, x);
}

double logsumexp(const std::vector<double>& logv)
{
    int n = static_cast<int>(logv.size());
    //if(n<=1)
    //  cout<<"Warning in logsumexp"<<endl;
    double max = *max_element(logv.begin(), logv.end());
    double answer = 0;
    // log(sum(exp(logf))   = log(sum(exp(logf - max(logf) + max(logf)))
    //          = max(logf) + log(sum(exp(logf - max(logf)))
    for(int i=0; i<n; i++)
        answer += exp(logv[i] - max);
    answer = max + log(answer);
    return answer;
}

} // namespace

#endif

