#ifndef TEST_TEMPLATE_HPP
#define TEST_TEMPLATE_HPP

#include <iostream>
#include "ffft/fftcore.hpp"

using namespace fftcore;

/**
 * @brief A templeted function to compare two different strategies. The method prints formatted results to be tabulated with benchmark.sh.
 * @author Lorenzo Gentile
*/
template <int dim, class FFTStrategy1, class FFTStrategy2>
void test_fft(int argc, char *argv[]){

    if(argc != dim + 1)
    {
        std::cout << "Usage: " << argv[0] << " <dim1> <dim2> ... <dimN>" << std::endl;
        return;
    }

    using FloatingType1 = typename FFTStrategy1::FloatTypeAlias;
    using FloatingType2 = typename FFTStrategy2::FloatTypeAlias;

    FFTSolver<dim, FloatingType1> solver(std::make_unique<FFTStrategy1>());
    FFTSolver<dim, FloatingType2> solver_baseline(std::make_unique<FFTStrategy2>());

    Eigen::array<Eigen::Index, dim> dimensions;

    for(int i = 0; i < dim; i++ )
    {
        std::cout << argv[i+1] << ","; 
        dimensions[i] = 1 << atoi(argv[i+1]);
    }

    CTensorBase<dim, FloatingType1> tensor(dimensions);
    tensor.get_tensor().setRandom();

    CTensorBase<dim, FloatingType2> tensor_baseline(dimensions);
    tensor_baseline.get_tensor() = tensor.get_tensor().template cast<std::complex<FloatingType2>>(); // cast to different floating type

    /*
     * FORWARD 
     */

    solver.compute_fft(tensor, FFT_FORWARD);
    double f1 = solver.get_timer().get_last();
    solver.get_timer().print_last_formatted();
    std::cout<<",";

    solver_baseline.compute_fft(tensor_baseline, FFT_FORWARD);
    double f2 = solver_baseline.get_timer().get_last();
    solver_baseline.get_timer().print_last_formatted();
    std::cout<<",";
    
    // norm inf of the error after the transformation 
    Eigen::Tensor<double, 0> error_forward = (tensor.get_tensor().abs() - tensor_baseline.get_tensor().abs()).maximum();
    // norm L1
    //Eigen::Tensor<double, 0> error_forward = (tensor.get_tensor().abs() - tensor_baseline.get_tensor().abs()).sum();


    /*
     * INVERSE 
     */

    solver.compute_fft(tensor, FFT_INVERSE);
    double i1 = solver.get_timer().get_last();
    solver.get_timer().print_last_formatted();
    std::cout<<",";
    
    solver_baseline.compute_fft(tensor_baseline, FFT_INVERSE);
    double i2 = solver_baseline.get_timer().get_last();
    solver_baseline.get_timer().print_last_formatted();
    std::cout<<",";

    // norm inf of the error after the inverse
    Eigen::Tensor<double, 0> error_inverse = (tensor.get_tensor().abs() - tensor_baseline.get_tensor().abs()).maximum();

    // norm L1
    //Eigen::Tensor<double, 0> error_inverse = (tensor.get_tensor().abs() - tensor_baseline.get_tensor().abs()).sum();


    // print speedup
    std::cout<<f2/f1<<","<<i2/i1<<",";

    // print error inverse 
    std::cout << error_forward << ",";
    std::cout << error_inverse << std::endl;

    return;
}

#endif //TEST_TEMPLATE_HPP