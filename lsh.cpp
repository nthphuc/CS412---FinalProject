//
//  main.cpp
//  lsh_self_impl
//
//  Created by Nguyen Minh Nhut on 12/14/19.
//  Copyright B) 2019 Nguyen Minh Nhut. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <random>
#include <algorithm>
#include <chrono>
class MyVector
{
public:
    MyVector ()
    {
        dimmension = 128;
        data.clear ();
        for (int i = 0; i < dimmension; ++i)
        {
            data.push_back (0.0);
        }
    }
    ~MyVector ()
    {
    }
    int dimmension;
    std::vector < double >data;
    
    double operator[] (int index)
    {
        return this->data[index];
    }
    void setValueAt (int index, double val)
    {
        this->data[index] = val;
    }
    double dotMulti (MyVector mutiplier)
    {
        double ret = 0;
        for (int i = 0; i < mutiplier.dimmension; ++i)
        {
            ret += this->data[i] * mutiplier[i];
        }
        return ret;
    }
    std::string toString ()
    {
        std::string ret = "";
        for (int i = 0; i < dimmension; ++i)
        {
            ret += std::to_string (this->data[i]) + ", ";
        }
        return ret;
    }
    void fromString (std::string str)
    {
        int str_length = str.length ();
        
        // create an array with size as string
        // length and initialize with 0
        std::vector < int >arr;
        
        int j = 0, i, sum = 0;
        int temp = 0;
        // Traverse the string
        for (i = 0; str[i] != '\0'; i++)
        {
            
            // if str[i] is ', ' then split
            if (str[i] == ',')
            {
                int x = temp;
                int A = 0;
                int B = 256;
                int y = 1 + (x-A)*(10-1)/(B-A);
                arr.push_back (y);
                
                temp = 0;
                // Increment j to point to next
                // array location
                j++;
            }
            else
            {
                
                // subtract str[i] by 48 to convert it to int
                // Generate number by multiplying 10 and adding
                // (int)(str[i])
                //            arr[j] = arr[j] * 10 + (str[i] - 48);
                if (str[i] != ' ')
                {
                    temp = temp * 10 + (str[i] - 48);
                }
            }
        }
        for (int i = 0; i < arr.size (); ++i)
        {
            this->data[i] = arr[i];
        }
    }
    
};

class LSH
{
public:
    static const int k = 72;
    static const int L = 15;
    static std::vector <MyVector> list_multiplierA;
    static std::vector <double>list_B;
    constexpr static const double w = 4.0;
    LSH ()
    {
        if (list_B.size () == 0)
        {
            std::cout<<"afsdsdf";
            for (int i = 0; i < L; ++i)
            {
                MyVector A = generate_multiplier_vector ();
                list_multiplierA.push_back (A);
                srand (time (NULL));
                double b = rand () * w;
                list_B.push_back (b);
            }
        }
    }
    unsigned getHashValue (MyVector data, MyVector a, double b)
    {
        return std::floor ((data.dotMulti (a) + b) / w);
    }
    unsigned long long getHashValue (MyVector data)
    {
        return std::floor ((data.dotMulti (list_multiplierA[0]) + list_B[0]) / w);
    }
    
    MyVector generate_multiplier_vector ()
    {
        unsigned seed =
        std::chrono::system_clock::now ().time_since_epoch ().count ();
        std::default_random_engine generator (seed);
        
        std::normal_distribution < double >distribution (0, 1.0);
        MyVector ret;
        
        for (int i = 0; i < k; ++i)
        {
            double r = distribution (generator);
            std::cout << "some Normal-distributed(0.0,1.0) results:" << r<< std::endl;
            ret.setValueAt (i, r);
        }
        return ret;
    }
};
const int LSH::k ;
const int LSH::L ;
std::vector <MyVector> LSH::list_multiplierA;
std::vector <double>LSH::list_B;
int main (int argc, const char *argv[])
{
    // insert code here...
    std::cout << "Hello, World!\n";
    std::string stringDescriptor =
    "8.0, 24.0, 28.0, 9.0, 12.0, 7.0, 20.0, 16.0, 90.0, 32.0, 25.0, 11.0, 9.0, 7.0, 6.0, 34.0, 43.0, 26.0, 70.0, 81.0, 19.0, 29.0, 20.0, 25.0, 68.0, 25.0, 17.0, 24.0, 23.0, 6.0, 7.0, 38.0, 50.0, 4.0, 3.0, 58.0, 67.0, 11.0, 14.0, 26.0, 126.0, 28.0, 9.0, 22.0, 22.0, 67.0, 55.0, 126.0, 57.0, 22.0, 35.0, 62.0, 60.0, 126.0, 126.0, 78.0, 98.0, 31.0, 11.0, 14.0, 85.0, 35.0, 22.0, 85.0, 40.0, 4.0, 3.0, 18.0, 100.0, 51.0, 32.0, 18.0, 126.0, 117.0, 105.0, 25.0, 7.0, 14.0, 23.0, 34.0, 16.0, 36.0, 126.0, 100.0, 39.0, 53.0, 20.0, 8.0, 17.0, 27.0, 7.0, 9.0, 21.0, 11.0, 4.0, 6.0, 1.0, 5.0, 7.0, 3.0, 8.0, 14.0, 23.0, 4.0, 4.0, 5.0, 18.0, 5.0, 1.0, 3.0, 13.0, 5.0, 0.0, 2.0, 22.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0";
    MyVector descriptor;
    descriptor.fromString (stringDescriptor);
    //   std::cout << descriptor.toString () << std::endl;
    LSH myLSH;
    unsigned long long r = myLSH.getHashValue(descriptor);
    std::string s = std::to_string(r);
    std::cout << s << std::endl;
    return 0;
}
