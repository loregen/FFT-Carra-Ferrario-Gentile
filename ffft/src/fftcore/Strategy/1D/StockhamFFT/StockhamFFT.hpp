#ifndef STOCKHAMFFT_HPP
#define STOCKHAMFFT_HPP

#include <iostream>
#include "../../../FFTSolver.hpp"
#include "../../../utils/FFTUtils.hpp"

using namespace std;
namespace fftcore{

    /**
     * @brief Sequential implementation of the 1 dimensional FFT using Stockham algorithm
     * 
     * The Stockham algorithm is a variation of the traditional Cooley-Tukey algorithm, with the 
     * advantage of avoiding the bit reversal step. Only 1D FFTs are supported for now.
     * 
     * @todo Implement R2C transform
     * @author Lorenzo Gentile
     * @date 2023-12-18
    */

    template<typename FloatingType = double>
    class StockhamFFT : public FFT_1D<FloatingType>
    {
    public:
        using typename FFT_1D<FloatingType>::RTensor_1D;
        using typename FFT_1D<FloatingType>::CTensor_1D;
        
        void fft(const CTensor_1D &, CTensor_1D &, FFTDirection) const;

        void fft(const RTensor_1D &, CTensor_1D &, FFTDirection) const;

        void fft(CTensor_1D &, FFTDirection) const;

        ~StockhamFFT() = default;
    };

    template <typename FloatingType>
    void StockhamFFT<FloatingType>::fft(const CTensor_1D &input, CTensor_1D &output, FFTDirection FFTDirection) const
    {
        output = input; //deep copy
        fft(output, FFTDirection);
    };

    template <typename FloatingType>
    void StockhamFFT<FloatingType>::fft(const RTensor_1D &, CTensor_1D &, FFTDirection) const
    {
        throw NotSupportedException("Operation is not supported");
    };

    
    template <typename FloatingType>
    void StockhamFFT<FloatingType>::fft(CTensor_1D &input_output, FFTDirection fftDirection) const
    {
        using Complex = std::complex<FloatingType>;

        CTensor_1D &input = input_output;
        CTensor_1D buffer(input.size());
        
        const TensorIdx n = input.size();
        assert(!(n & (n - 1)) && "FFT length must be a power of 2.");
        const TensorIdx n2 = n >> 1; // n2 = n/2

        const unsigned int log2n = std::log2(n);

        //conjugate if inverse
        if(fftDirection == fftcore::FFT_INVERSE){
            FFTUtils::conjugate(input);
        }

        Complex w, t, u;
        TensorIdx m, m2, r, r2;
        // Stockham iterative FFT
        for(unsigned int s = 1; s <= log2n; ++s){

            //std::cout << "s = " << s << std::endl;

            m = 1 << s;  // 2^s
            m2 = m >> 1; // m2 = m/2
            r = n / m;   // r = n/m
            r2 = r << 1; // r2 = 2r

            for(TensorIdx j = 0; j < m2; ++j){

                w = exp(Complex(0, -2 * M_PI * j / m)); // w = e^(-2*pi*j/m)

                for(TensorIdx k = 0; k < r; ++k){

                    u = input[j * r2 + k];
                    t = w * input[j * r2 + k + r];

                    buffer[j * r + k] = u + t;
                    buffer[j * r + n2 + k] = u - t;
                }
            }
            std::swap(input, buffer);
        }

        //re-conjugate and scale if inverse
        if(fftDirection == fftcore::FFT_INVERSE){
            FFTUtils::conjugate(input);
            input = input * Complex(1.0/n, 0);
        }
    };

}

#endif //STOCKHAMFFT_HPP