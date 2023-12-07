#ifndef FFTUTILS_HPP
#define FFTUTILS_HPP

#include <cmath>
#include <unsupported/Eigen/CXX11/Tensor>

namespace FFTUtils{

    // Function to perform the bit reversal of a given integer n
    unsigned int reverseBits(unsigned int n, int log2n)
    {
        unsigned int result = 0;
        for (int i = 0; i < log2n; i++)
        {
            if (n & (1 << i))
            {
                result |= 1 << (log2n - 1 - i);
            }
        }
        return result;
    }

    template<typename DataType>
    void bit_reversal_permutation(Eigen::Tensor<DataType, 1> &tensor){
        for (int i = 0; i < tensor.size(); ++i)
        {
            int rev = FFTUtils::reverseBits(i, std::log2(tensor.size()));
            if (i < rev)
            {
                std::swap(tensor[i], tensor[rev]);
            }
        }
    }

    template<typename FloatingType>
    void conjugate(Eigen::Tensor<std::complex<FloatingType>, 1> &tensor){
        tensor.unaryExpr([](std::complex<FloatingType> x){return std::conj(x);});
    }


}

#endif //FFTUTILS_HPP