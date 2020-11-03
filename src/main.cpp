#include <iostream>
#include <string>
#include <Eigen/Dense>

#include "ThreadPool.h"
 
using Eigen::MatrixXd;
 
int main()
{
    {
        ThreadPool pool(4);

        for(int i = 0; i < 10; i++)
        {
            int scalar = i;
            MatrixXd m = MatrixXd::Random(3,3);
            std::function<void(MatrixXd, int)> funcy= [](MatrixXd mat, int a){std::cout << mat * a << std::endl;};
            
            std::cout << "Adding a job" << i << std::endl;
            pool.addJob(std::move(funcy), std::move(m), std::move(scalar));
        }
    }
    std::cout << "ThreadPool has been destructed" << std::endl;
}
