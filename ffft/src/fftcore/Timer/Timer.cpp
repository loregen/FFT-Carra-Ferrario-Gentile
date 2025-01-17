#include "Timer.hpp"
#include <algorithm>

namespace fftcore{

    void Timer::start(){
        _current_start = std::chrono::high_resolution_clock::now();
    }

    void Timer::stop(){
        auto current_stop = std::chrono::high_resolution_clock::now();
        _durations.push_back(current_stop - _current_start);
    }

    void Timer::reset(){
        _durations.clear();
    }

    void Timer::print_last_formatted() const {
        double time = Timer::get_last() ;
            if(time < 1e-6){
                std::cout << time*1e9 << " ns";
            }else if(time < 1e-3){
                std::cout << time*1e6 << " us";
            }else if(time < 1){
                std::cout << time*1e3 << " ms";
            }else{
                std::cout << time << " s";
            }
    }

    void Timer::print(std::string title) const{
        if(title.empty()) 
            std::cout << "\n---- Timings ----" << std::endl;
        else
            std::cout << "\n----" << title << "----" << std::endl;

        for(size_t i = 0; i < _durations.size(); ++i){
            
            double time = _durations[i].count();
            if(time < 1e-6){
                std::cout << i+1 << ": " << time*1e9 << " ns" << std::endl;
            }else if(time < 1e-3){
                std::cout << i+1 << ": " << time*1e6 << " us" << std::endl;
            }else if(time < 1){
                std::cout << i+1 << ": " << time*1e3 << " ms" << std::endl;
            }else{
                std::cout << i+1 << ": " << time << " s" << std::endl;
            }
        }
        std::cout << "-----------------\n" << std::endl;
    }

    void Timer::print() const{
       print("");
    }

    double Timer::get_last() const{
        return _durations.back().count();
    }

    double _count(fftcore::Timer::duration dur){
        return dur.count();
    }

    double Timer::get_min() const{
        std::vector<double> counts(_durations.size());

        std::transform(_durations.begin(), _durations.end(), counts.begin(), _count);
        
        auto min = std::min_element(counts.begin(), counts.end());

        // @Todo: improve with exceptions
        if(min!=counts.end()){
            return *min;
        }else{
            return 0;
        }

    }

}