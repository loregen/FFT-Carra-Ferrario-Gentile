#ifndef OMPFFT_2D_HPP
#define OMPFFT_2D_HPP


#include <iostream>
#include <omp.h>
#include "../../../FFTSolver.hpp"
#include "../../../utils/FFTUtils.hpp"

using namespace std;
namespace fftcore{

    /**
     * @brief OpenMP implementation of the 2 Dimensional FFT 
     * 
     * @author Edoardo Carrà
    */
    template<typename FloatingType = double>
    class OmpFFT_2D : public FFT_2D<FloatingType>
    {
    public:
        using typename FFT_2D<FloatingType>::RTensor_2D;
        using typename FFT_2D<FloatingType>::CTensor_2D;
        
        void fft(const CTensor_2D &, CTensor_2D &, FFTDirection) const;

        void fft(const RTensor_2D &, CTensor_2D &, FFTDirection) const;

        void fft(CTensor_2D &, FFTDirection) const;

        ~OmpFFT_2D() = default;
    };

    template <typename FloatingType>
    void OmpFFT_2D<FloatingType>::fft(const CTensor_2D &input, CTensor_2D &output, FFTDirection fftDirection) const
    {
        output = input; //deep copy
        fft(output, fftDirection);
    };

    template <typename FloatingType>
    void OmpFFT_2D<FloatingType>::fft(const RTensor_2D &, CTensor_2D &, FFTDirection) const
    {
        throw NotSupportedException("Operation is not supported");
    };

    /**
     * This compute FFT in 2 dimensions using Cooley-Tukey algorithm using OpenMP. The algorithm follows the following
     * steps:
     * - First, the rows are transformed using the Cooley-Tukey algorithm for one dimensional data. This first
     *   step is performed in row-major order. Each thread computes independently a subset of rows. 
     * - Second, the columns are transformed, accessing the elements in column-major order.
     *   Each thread computes independently a subset of columns. 
     *  
     * @author Edoardo Carrà
     */
    template <typename FloatingType>
    void OmpFFT_2D<FloatingType>::fft(CTensor_2D &input_output, FFTDirection fftDirection) const
    {

#ifndef _OPENMP
        std::cerr<< "[WARNING] OMP not found, normal execution.";
#endif

        int row, col, row1, col1;

        using Complex = std::complex<FloatingType>;
        const auto& dimensions = input_output.dimensions();

        //input dimensions check
        for(int i=0; i<2; i++){
            assert(!(dimensions[i] & (dimensions[i] - 1)) && "All dimensions of the input must be a power of 2.");
        }

        //conjugate if inverse
        if(fftDirection == FFT_INVERSE){            
            FFTUtils::conjugate(input_output);
        }


        #pragma omp parallel private(row,col,row1,col1)
        {

            Complex w, wm, t, u;
            Eigen::Index m, m2;
            Eigen::Index rev;
            // No need for num_threads to be a power of two in this case
           
            // loop over each dimension
            for(unsigned int dim=0; dim<2; dim++){
                unsigned int log2n = std::log2(dimensions[dim]);

                // threads split the different FFT to compute equally.   
                #pragma omp for
                for(Eigen::Index offset=0; offset< dimensions[dim==0?1:0]; offset++){

                    // During the first iteration, the access pattern follows row-major order. 
                    // Subsequent iterations adopt the column-major order, which is the standard 
                    // format for Eigen tensors.  
                    
                    // Normal 1-D Cooley-Tukey FFT

                    // Bit-reversal permutation
                    for (Eigen::Index i = 0; i < dimensions[dim]; ++i)
                    {
                        rev = FFTUtils::reverseBits(i, log2n);
                        if (i < rev)
                        {
                            // In each iteration, one dimension will be accessed by offset, 
                            // while the other dimension undergoes a standard Fast Fourier Transform (FFT).
                            row = dim==1? offset : i;
                            col = dim==0? offset : i; 

                            row1 = dim==1? offset : rev;
                            col1 = dim==0? offset : rev; 

                            std::swap(input_output(row,col),input_output(row1,col1));
                        }
                    }

                    for (unsigned int s = 1; s <= log2n; ++s)
                    {
                        m = 1 << s;  // 2 power s
                        m2 = m >> 1; // m2 = m/2 -1
                        wm = exp(Complex(0, -2 * M_PI / m)); // w_m = e^(-2*pi/m)

                        for(Eigen::Index k = 0; k < dimensions[dim]; k += m)
                        {
                            w = Complex(1, 0);
                            for(Eigen::Index j = 0; j < m2; ++j)
                            {

                                // In each iteration, one dimension will be accessed by offset, 
                                // while the other dimension undergoes a standard Fast Fourier Transform (FFT).
                                row = dim==1? offset : k + j + m2;
                                col = dim==0? offset : k + j + m2; 
                                
                                row1 = dim==1? offset : k + j;
                                col1 = dim==0? offset : k + j; 

                                t = w * input_output(row,col);
                                u = input_output(row1,col1);

                                input_output(row1,col1) = u + t;
                                input_output(row,col) = u - t;

                                w *= wm;
                            } // F_3
                        } // F_2
                    } // F_1
                } // loop over one dimension - implicit barrier
            } // loop over dimensions
        
        } //end parallel region

        //re-conjugate and scale if inverse
        if(fftDirection == FFT_INVERSE){
            FFTUtils::conjugate(input_output);
            //scale
            input_output = input_output * Complex(1.0/(dimensions[0]*dimensions[1]), 0);
        }  
    };
}

#endif //OmpFFT_2D_HPP