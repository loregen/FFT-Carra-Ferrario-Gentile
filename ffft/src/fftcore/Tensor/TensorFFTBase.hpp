#ifndef TENSORFFTBASE_HPP
#define TENSORFFTBASE_HPP

#include <unsupported/Eigen/CXX11/Tensor>

namespace fftcore{


    /**
     * @brief A wrapper for Eigen::Tensor
     * 
     * TensorFFTBase behaves as a wrapper for Eigen::Tensor objects.
     * It's meant to be used as it is or as a base class for type specific
     * wrappers.  This hides the use of Eigen tensors and allows the user to define their own
     * cleaning and manipulation methods for input/output specific to their application through the
     * extension of the TensorFFTBase class.
     * 
     * @see TimeTensor, FreqTensor
     * @author Daniele Ferrario
     */
    template<typename DataType, int Rank>
    class TensorFFTBase{
        public:
            /**
            * Constructor for a Tensor. The constructor must be passed rank integers 
            * indicating the sizes of the instance along each of the the rank dimensions.
            */
            template <typename... Args>
            TensorFFTBase(Args... args){
                _tensor = Eigen::Tensor<DataType, Rank>(args...);
            };

            int get_rank();

            const Eigen::Tensor<DataType, Rank>& get_tensor() const;
            Eigen::Tensor<DataType, Rank>& get_tensor() ;


            ~TensorFFTBase() = default;
        private:
            int _rank = Rank;
            Eigen::Tensor<DataType, Rank> _tensor;
    };

    /**
    * Returns the rank of the tensor.
    */
    template<typename DataType, int Rank>
    int TensorFFTBase<DataType, Rank>::get_rank(){
        return _rank;
    }

    /**
    * Returns the actual tensor object.
    */
    template<typename DataType, int Rank>
    Eigen::Tensor<DataType, Rank>& TensorFFTBase<DataType, Rank>::get_tensor() {
            return _tensor;
    };

    template<typename DataType, int Rank>
    const Eigen::Tensor<DataType, Rank>& TensorFFTBase<DataType, Rank>::get_tensor() const{
            return _tensor;
    };

}

#endif // TENSORFFTBASE_HPP